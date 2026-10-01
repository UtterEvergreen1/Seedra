#!/usr/bin/env python3
"""Multi-step search plans: what to do after the first xray fails, and when to reset.

    python multistep.py --data DATA_DIR --tag small            (needs layouts_<scale>.npy from `layouts` mode)

A plan is what you do after spawning, depending on the 64 x 64 spawn cell your map coordinates fall in:

* reset at once, or
* go to a spot, dig in, and run a sequence of xrays there (down, and/or directional looks, in a fixed order),
  stopping at the first hit, or
* one xray at spot A, and if it shows nothing, walk to spot B and do one xray there.

Kernels only give the chance of each xray on its own. Whether "A fails, then B hits" depends on the joint outcome
within one layout, so this script works on real layouts (`EXE_StrongholdMining layouts`): for every layout, one
byte per spot on an 8-block grid of offsets from the stronghold chunk (bit per method). Layouts do not depend on
biomes, so for any group of worlds

    P(A fails, B hits | group) = sum over start chunks s of P(s | group) * P_layout(B - 16 s hit, A - 16 s miss)

and the pair term comes from one matrix product over all layouts. Spots are on the 8-block grid.

Plans are chosen on the training worlds and scored on the test worlds (same 70/30 split as report.py), minimising
expected seconds per stronghold found (Dinkelbach over a grid of break-even values). Worlds whose layout is in the
layouts file are also simulated with their own real layout as a cross-check.

Writes DATA_DIR/reports/<tag>/multistep.md, multistep_plans_<variant>.csv and multistep.json, and refreshes
reports/summary.md.
"""
from __future__ import annotations

import argparse
import itertools
import json
import sys
import time
from dataclasses import dataclass
from pathlib import Path

import numpy as np
import pandas as pd
from scipy import signal

sys.path.insert(0, str(Path(__file__).resolve().parent))
import shlib as S  # noqa: E402

G8 = S.WORLD_SIDE // 8  # world spots on the 8-block grid: x = 8 * i - 432, i in [0, 108)
LH = 25  # layout window: offsets -200..200 in steps of 8 -> indices 0..50
LS = 2 * LH + 1
LC = LS * LS
DS = 2 * LS - 1  # displacement window between two spots (in 8-block units): -50..50
BIT = {"dig": 0, "down_flat": 1, "down_3d": 2, "portal30": 3, "look_n": 4, "look_e": 5, "look_s": 6, "look_w": 7}
LOOK_NAMES = ["north", "east", "south", "west"]
VARIANTS = {"flat": "down_flat", "3d": "down_3d"}


def spot_xz(i: int) -> tuple[int, int]:
    """World (x, z) of flat 8-grid spot index."""
    iz, ix = divmod(int(i), G8)
    return 8 * ix - S.WORLD_HALF, 8 * iz - S.WORLD_HALF


# -------------------------------------------------------------------------------------------- layouts


@dataclass
class Layouts:
    """Per-variant layout statistics. Actions: 0 = xray down, 1..4 = look north/east/south/west."""
    n: int
    none: np.ndarray  # (32, LS, LS): P(no action in set S sees it) for an action set bitmask S
    marg: np.ndarray  # (5, LS, LS): P(action sees it)
    joint: np.ndarray  # (5, 5, LC, LC) float32 views: P(a sees it from cell e AND b sees it from cell f)
    codes: np.ndarray  # (N, LC) uint8 5-bit codes (for simulating with real layouts)
    sample: np.ndarray  # (N,) dataset sample index of each layout
    chunk: np.ndarray  # (N, 2) start chunk (cx, cz)


def load_layouts(data: Path, tag: str, bit_down: int, log) -> Layouts:
    raw = np.load(data / f"layouts_{tag}.npy", mmap_mode="r")
    index = np.load(data / f"layouts_index_{tag}.npy")
    n = raw.shape[0]
    bits = [bit_down] + [BIT[f"look_{d}"] for d in "nesw"]
    codes = np.zeros((n, LC), np.uint8)
    for i0 in range(0, n, 20000):
        blk = np.asarray(raw[i0:i0 + 20000]).reshape(-1, LC)
        c = np.zeros_like(blk)
        for a, b in enumerate(bits):
            c |= ((blk >> b) & 1) << a
        codes[i0:i0 + 20000] = c
    counts = np.zeros((32, LC), np.int64)
    cell = np.arange(LC)
    for i0 in range(0, n, 20000):
        c = codes[i0:i0 + 20000].astype(np.int64)
        counts += np.bincount((c * LC + cell).ravel(), minlength=32 * LC).reshape(32, LC)
    v = np.arange(32)
    none = np.stack([counts[(v & s) == 0].sum(0) for s in range(32)]) / n
    marg = np.stack([counts[(v >> a) & 1 == 1].sum(0) for a in range(5)]) / n

    # joint[a, b, e, f] = P(bit a at e and bit b at f), via one Gram matrix over the cells that are ever active
    active = [np.flatnonzero(marg[a] > 0) for a in range(5)]
    offs = np.cumsum([0] + [len(x) for x in active])
    gram = np.zeros((offs[-1], offs[-1]), np.float32)
    t0 = time.time()
    for i0 in range(0, n, 8192):
        c = codes[i0:i0 + 8192]
        X = np.hstack([((c[:, active[a]] >> a) & 1).astype(np.float32) for a in range(5)])
        gram += X.T @ X
    gram /= n
    log(f"joint matrix over {offs[-1]} active cells from {n:,} layouts in {time.time() - t0:.0f}s")
    joint = np.zeros((5, 5, LC, LC), np.float32)
    for a in range(5):
        for b in range(5):
            joint[a, b][np.ix_(active[a], active[b])] = gram[offs[a]:offs[a + 1], offs[b]:offs[b + 1]]
    return Layouts(n, none.reshape(32, LS, LS), marg.reshape(5, LS, LS), joint, codes, index[:, 0].astype(np.int64),
                   index[:, 1:3].astype(np.int64))


# -------------------------------------------------------------------------------------------- plans


@dataclass(frozen=True)
class Plan:
    """kind: 'reset', 'seq' (actions in order at spot1) or 'pair' (one action at spot1, then one at spot2)."""
    kind: str
    spot1: int = -1
    actions: tuple = ()
    spot2: int = -1
    action2: int = -1

    def describe(self) -> str:
        if self.kind == "reset":
            return "reset at once"
        x, z = spot_xz(self.spot1)
        steps = [f"go to ({x}, {z}), dig in, {action_name(self.actions[0])}"]
        steps += [f"if nothing: {action_name(a)}" for a in self.actions[1:]]
        if self.kind == "pair":
            x2, z2 = spot_xz(self.spot2)
            steps.append(f"if nothing: walk {np.hypot(x2 - x, z2 - z):.0f} blocks to ({x2}, {z2}), dig in, "
                         f"{action_name(self.action2)}")
        return "; ".join(steps) + "; then reset"


def action_name(a: int) -> str:
    return "xray down" if a == 0 else f"look {LOOK_NAMES[a - 1]}"


@dataclass
class Cfg:
    reset: float
    look: float
    dig: float
    xray: float
    look_dir: float
    speed: float
    budget: float

    def cost(self, a: int) -> float:
        return self.xray if a == 0 else self.look_dir


SEQS = [p for k in range(1, 6) for p in itertools.permutations(range(5), k)]  # 325 ordered action lists


def seq_sets(seq: tuple) -> list[int]:
    """Action-set bitmask before each step and after the last: [0, {a1}, {a1, a2}, ...]."""
    out, s = [0], 0
    for a in seq:
        s |= 1 << a
        out.append(s)
    return out


# -------------------------------------------------------------------------------------------- groups


@dataclass
class Group:
    name: str
    share: float  # share of training worlds
    hist: np.ndarray  # (54, 54) chunk probabilities [cz, cx]
    walk: np.ndarray  # (G8 * G8,) mean travel seconds from the players to each spot
    none: np.ndarray | None = None  # (32, G8 * G8): P(no action in S sees it from the spot)


def group_maps(g: Group, lay: Layouts) -> None:
    """P(no action in set S sees it) for every spot, for this group's start-chunk distribution."""
    lattice = np.zeros((G8, G8))
    lattice[::2, ::2] = g.hist  # chunk c -> 8-grid index 2 * (c + 27)
    hit = 1.0 - lay.none  # zero outside the layout window
    full = np.stack([signal.fftconvolve(lattice, hit[s]) for s in range(32)])
    g.none = (1.0 - np.clip(full[:, LH:LH + G8, LH:LH + G8], 0.0, 1.0)).reshape(32, -1).astype(np.float32)


ALL_XZ = np.array([spot_xz(i) for i in range(G8 * G8)], dtype=np.float64)


@dataclass
class Choice:
    """Best plan per break-even value lam (grid): value = lam * P - cost, with P and cost on training worlds."""
    value: np.ndarray  # (L,)
    p: np.ndarray
    cost: np.ndarray
    plans: list


def best_seq(g: Group, cfg: Cfg, lams: np.ndarray, seqs: list[tuple]) -> Choice:
    none = g.none
    base = g.walk + cfg.dig
    best_v = np.full(len(lams), -np.inf)
    best_p, best_c, plans = np.zeros(len(lams)), np.zeros(len(lams)), [None] * len(lams)
    for seq in seqs:
        sets = seq_sets(seq)
        cost = base.copy()
        t = base + cfg.look
        ok = np.ones_like(base, bool)
        for k, a in enumerate(seq):
            cost += cfg.cost(a) * none[sets[k]]
            t = t + cfg.cost(a)
        ok &= t <= cfg.budget
        p = 1.0 - none[sets[-1]]
        v = lams[:, None] * p[None, :] - cost[None, :]
        v[:, ~ok] = -np.inf
        j = np.argmax(v, axis=1)
        vj = v[np.arange(len(lams)), j]
        better = vj > best_v
        for li in np.flatnonzero(better):
            best_v[li], best_p[li], best_c[li] = vj[li], p[j[li]], cost[j[li]]
            plans[li] = Plan("seq", int(j[li]), seq)
    return Choice(best_v, best_p, best_c, plans)


class PairSearch:
    """'One xray at A; if nothing, walk to B and one xray there' for all pairs of actions."""

    def __init__(self, lay: Layouts):
        self.lay = lay
        # displacement gather: Jd[u, D] = J[u, u + D], u = cell of A in the layout window, D = B - A (8-grid units)
        uz, ux = np.divmod(np.arange(LC), LS)
        dz, dx = np.divmod(np.arange(DS * DS), DS)
        fz = uz[:, None] + dz[None, :] - (LS - 1)
        fx = ux[:, None] + dx[None, :] - (LS - 1)
        self.valid = (fz >= 0) & (fz < LS) & (fx >= 0) & (fx < LS)
        # flat index into J (LC x LC) raveled: e * LC + f with e = uz * LS + ux
        self.flat = np.where(self.valid, (uz * LS + ux)[:, None] * LC + fz * LS + fx, 0).astype(np.int64)

    def jd(self, a: int, b: int) -> np.ndarray:
        j = self.lay.joint[a, b].ravel()[self.flat]
        j[~self.valid] = 0.0
        return j  # (LC, DS * DS)

    @staticmethod
    def chunk_weights(g: Group, spots: np.ndarray) -> np.ndarray:
        """H[k, u] = P(start chunk s | group) where spot k sits at layout cell u of chunk s."""
        iz, ix = np.divmod(spots, G8)
        uz, ux = np.divmod(np.arange(LC), LS)
        sz = iz[:, None] + LH - uz[None, :]  # 8-grid index of the chunk origin = 2 * (c + 27)
        sx = ix[:, None] + LH - ux[None, :]
        ok = (sz >= 0) & (sx >= 0) & (sz % 2 == 0) & (sx % 2 == 0) & (sz < G8) & (sx < G8)
        H = np.zeros(ok.shape, np.float32)
        H[ok] = g.hist[sz[ok] // 2, sx[ok] // 2]
        return H

    def best(self, g: Group, cfg: Cfg, lams: np.ndarray, pairs: list[tuple], jds: dict, a_cands: dict,
             nb: int) -> Choice:
        none = g.none
        best_v = np.full(len(lams), -np.inf)
        best_p, best_c, plans = np.zeros(len(lams)), np.zeros(len(lams)), [None] * len(lams)
        Hs = {a: self.chunk_weights(g, a_cands[a]) for a in {p[0] for p in pairs}}
        for a, b in pairs:
            A = a_cands[a]
            pa = 1.0 - none[1 << a][A]
            pb = 1.0 - none[1 << b]
            B = np.argsort(-pb)[:nb]  # candidate second spots: the best nb spots for action b
            M = Hs[a] @ jds[(a, b)]  # (nA, DS * DS): P(a hits at A and b hits at A + D)
            iza, ixa = np.divmod(A, G8)
            izb, ixb = np.divmod(B, G8)
            dz = izb[None, :] - iza[:, None] + LS - 1
            dx = ixb[None, :] - ixa[:, None] + LS - 1
            inside = (dz >= 0) & (dz < DS) & (dx >= 0) & (dx < DS)
            both = np.where(inside, np.take_along_axis(M, np.where(inside, dz * DS + dx, 0), axis=1), 0.0)
            p = pa[:, None] + pb[B][None, :] - both
            dist = np.hypot(ALL_XZ[B, 0][None, :] - ALL_XZ[A, 0][:, None],
                            ALL_XZ[B, 1][None, :] - ALL_XZ[A, 1][:, None]) / cfg.speed
            first = g.walk[A] + cfg.dig + cfg.cost(a)
            cost = first[:, None] + (1.0 - pa)[:, None] * (dist + cfg.dig + cfg.cost(b))
            t_end = cfg.look + first[:, None] + dist + cfg.dig + cfg.cost(b)
            ok = (t_end <= cfg.budget) & (A[:, None] != B[None, :])
            p, cost = p.ravel(), cost.ravel()
            v = lams[:, None] * p[None, :] - cost[None, :]
            v[:, ~ok.ravel()] = -np.inf
            j = np.argmax(v, axis=1)
            vj = v[np.arange(len(lams)), j]
            for li in np.flatnonzero(vj > best_v):
                ka, kb = divmod(int(j[li]), len(B))
                best_v[li], best_p[li], best_c[li] = vj[li], p[j[li]], cost[j[li]]
                plans[li] = Plan("pair", int(A[ka]), (a,), int(B[kb]), b)
        return Choice(best_v, best_p, best_c, plans)


# ----------------------------------------------------------------------------------------- evaluation


@dataclass
class Worlds:
    px: np.ndarray
    pz: np.ndarray
    isx: np.ndarray  # 8-grid index of the start chunk origin
    isz: np.ndarray
    sample: np.ndarray
    group: np.ndarray  # group id per world (index into the grouping's plan list)


def cell_of(lay_map: np.ndarray, ez, ex, default: float) -> np.ndarray:
    ok = (ez >= 0) & (ez < LS) & (ex >= 0) & (ex < LS)
    out = np.full(ez.shape, default, np.float64)
    out[ok] = lay_map[ez[ok], ex[ok]]
    return out


def evaluate(plan: Plan, w: Worlds, lay: Layouts, cfg: Cfg, sim_codes: np.ndarray | None = None):
    """Per world: expected finds, expected seconds after the look-around, and per step (find chance, seconds after
    spawn). With sim_codes (per world 5-bit layout codes, (n, LC)), the outcome is simulated with those layouts."""
    n = len(w.px)
    if plan.kind == "reset" or n == 0:
        return np.zeros(n), np.zeros(n), [], None
    x1, z1 = spot_xz(plan.spot1)
    iz1, ix1 = divmod(plan.spot1, G8)
    ez, ex = iz1 - w.isz + LH, ix1 - w.isx + LH
    walk = np.hypot(w.px - x1, w.pz - z1) / cfg.speed
    t = cfg.look + walk + cfg.dig
    go = t + cfg.cost(plan.actions[0]) <= cfg.budget  # otherwise the runner resets at once
    steps, cost = [], np.where(go, walk + cfg.dig, 0.0)
    inside1 = (ez >= 0) & (ez < LS) & (ex >= 0) & (ex < LS)
    code1 = None
    if sim_codes is not None:
        code1 = np.zeros(n, np.uint8)
        code1[inside1] = sim_codes[np.flatnonzero(inside1), ez[inside1] * LS + ex[inside1]]
    if plan.kind == "seq":
        sets = seq_sets(plan.actions)
        prev = np.ones(n)
        for k, a in enumerate(plan.actions):
            t = t + cfg.cost(a)
            do = go & (t <= cfg.budget)
            if code1 is None:
                cur = cell_of(lay.none[sets[k + 1]], ez, ex, 1.0)
            else:
                cur = ((code1 & sets[k + 1]) == 0).astype(np.float64)
            cost += np.where(do, cfg.cost(a) * prev, 0.0)
            steps.append((np.where(do, prev - cur, 0.0), t.copy()))
            prev = np.where(do, cur, prev)
        found = 1.0 - prev
        return found, cost, steps, walk
    a, b = plan.actions[0], plan.action2
    x2, z2 = spot_xz(plan.spot2)
    iz2, ix2 = divmod(plan.spot2, G8)
    fz, fx = iz2 - w.isz + LH, ix2 - w.isx + LH
    if code1 is None:
        pa = cell_of(lay.marg[a], ez, ex, 0.0)
        pb = cell_of(lay.marg[b], fz, fx, 0.0)
        both = np.zeros(n)
        ok = inside1 & (fz >= 0) & (fz < LS) & (fx >= 0) & (fx < LS)
        both[ok] = lay.joint[a, b][ez[ok] * LS + ex[ok], fz[ok] * LS + fx[ok]]
        p2 = pb - both
    else:
        pa = ((code1 >> a) & 1).astype(np.float64)
        inside2 = (fz >= 0) & (fz < LS) & (fx >= 0) & (fx < LS)
        code2 = np.zeros(n, np.uint8)
        code2[inside2] = sim_codes[np.flatnonzero(inside2), fz[inside2] * LS + fx[inside2]]
        p2 = (1.0 - pa) * ((code2 >> b) & 1)
    t = t + cfg.cost(a)
    cost += np.where(go, cfg.cost(a), 0.0)
    steps.append((np.where(go, pa, 0.0), t.copy()))
    hop = np.hypot(x2 - x1, z2 - z1) / cfg.speed + cfg.dig + cfg.cost(b)
    t = t + hop
    do = go & (t <= cfg.budget)
    cost += np.where(do, (1.0 - pa) * hop, 0.0)
    steps.append((np.where(do, p2, 0.0), t.copy()))
    found = np.where(go, pa, 0.0) + np.where(do, p2, 0.0)
    return found, cost, steps, walk


# ------------------------------------------------------------------------------------------- driver


FAMILIES = [
    ("down", "one xray down", False),
    ("down2", "xray down; if nothing, a second xray down at another spot", False),
    ("spot", "any sequence of xrays at one spot (down and/or looks, in the best order)", True),
    ("all", "best of: a sequence at one spot, or one xray at A then one at B", True),
]


def family_parts(key: str) -> tuple[list[tuple], list[tuple]]:
    """(sequences at one spot, (a, b) action pairs for two spots) allowed in a family."""
    if key == "down":
        return [(0,)], []
    if key == "down2":
        return [(0,)], [(0, 0)]
    if key == "spot":
        return SEQS, []
    return SEQS, [(a, b) for a in range(5) for b in range(5)]


def choose(groups: list[Group], choices: list[Choice], lams: np.ndarray, cfg: Cfg, allow_reset: bool):
    """Pick the break-even value whose plans give the fewest seconds per find on the training worlds."""
    shares = np.array([g.share for g in groups])
    P = np.array([c.p for c in choices])  # (G, L)
    C = np.array([c.cost for c in choices])
    V = np.array([c.value for c in choices])
    go = (V > 0) | (not allow_reset)
    ratio = (cfg.reset + cfg.look + (shares[:, None] * np.where(go, C, 0.0)).sum(0)) / \
        np.maximum((shares[:, None] * np.where(go, P, 0.0)).sum(0), 1e-12)
    li = int(np.argmin(ratio))
    plans = [c.plans[li] if go[g, li] else Plan("reset") for g, c in enumerate(choices)]
    return plans, float(ratio[li]), float(lams[li])


def time_hist(steps_all, weights_total: float, marks=(30, 45, 60, 90, 120)) -> dict:
    """Share of finds made within each number of seconds after spawning."""
    ps = np.concatenate([s[0] for s in steps_all]) if steps_all else np.zeros(0)
    ts = np.concatenate([s[1] for s in steps_all]) if steps_all else np.zeros(0)
    tot = ps.sum()
    out = {f"<= {m}s": float(ps[ts <= m].sum() / tot) if tot > 0 else float("nan") for m in marks}
    if tot > 0:
        order = np.argsort(ts)
        cum = np.cumsum(ps[order]) / tot
        out["median"] = float(ts[order][np.searchsorted(cum, 0.5)])
    return out


_SEQ_CACHE: dict = {}  # (group, number of sequences) -> Choice; cleared per variant


def run_grouping(name, groups, labels, lay, cfg, lams, family, pair_search, jds, a_cands, args, test, sim, log,
                 allow_reset=True):
    seqs, pairs = family_parts(family)
    choices = []
    for g in groups:
        key = (id(g), len(seqs))
        if key not in _SEQ_CACHE:
            _SEQ_CACHE[key] = best_seq(g, cfg, lams, seqs)
        c0 = _SEQ_CACHE[key]
        c = Choice(c0.value.copy(), c0.p.copy(), c0.cost.copy(), list(c0.plans))
        if pairs:
            c2 = pair_search.best(g, cfg, lams, pairs, jds, a_cands[g.name], args.b_cands)
            take = c2.value > c.value
            for li in np.flatnonzero(take):
                c.value[li], c.p[li], c.cost[li], c.plans[li] = c2.value[li], c2.p[li], c2.cost[li], c2.plans[li]
        choices.append(c)
    plans, train_ratio, lam = choose(groups, choices, lams, cfg, allow_reset)

    # test worlds: expected value per world, plus simulation with real layouts where available
    n = len(test.px)
    found, cost = np.zeros(n), np.zeros(n)
    sim_found, sim_cost = np.zeros(n), np.zeros(n)
    steps_all, per_group = [], []
    for gi, plan in enumerate(plans):
        idx = np.flatnonzero(labels == gi)
        w = Worlds(test.px[idx], test.pz[idx], test.isx[idx], test.isz[idx], test.sample[idx], test.group[idx])
        f, c, st, _ = evaluate(plan, w, lay, cfg)
        found[idx], cost[idx] = f, c
        steps_all += st
        per_group.append({"found": float(f.sum()), "cost": float(c.sum()), "worlds": len(idx)})
        if sim is not None:
            sidx = np.flatnonzero(sim.mask[idx])
            if len(sidx):
                ws = Worlds(w.px[sidx], w.pz[sidx], w.isx[sidx], w.isz[sidx], w.sample[sidx], w.group[sidx])
                f2, c2, _, _ = evaluate(plan, ws, lay, cfg, sim.codes[sim.row[idx[sidx]]])
                sim_found[idx[sidx]], sim_cost[idx[sidx]] = f2, c2
    base = cfg.reset + cfg.look
    total = n * base + cost.sum()
    res = {"grouping": name, "family": family, "lam": lam, "train_s_per_find": train_ratio,
           "s_per_find": float(total / max(found.sum(), 1e-12)),
           "plans": plans, "per_group": per_group, "found": found, "cost": cost,
           "time": time_hist(steps_all, found.sum())}
    go = np.zeros(n, bool)
    for gi, plan in enumerate(plans):
        if plan.kind != "reset":
            go[labels == gi] = True
    res["played"] = float(go.mean())
    res["chance_when_go"] = float(found[go].sum() / max(go.sum(), 1))
    res["seconds_when_go"] = float(cost[go].sum() / max(go.sum(), 1))
    if sim is not None and sim.mask.any():
        m = sim.mask
        ef, sf = found[m], sim_found[m]
        res["sim"] = {"worlds": int(m.sum()), "expected": float(ef.mean()), "simulated": float(sf.mean()),
                      "se": float(sf.std() / np.sqrt(m.sum())),
                      "expected_cost": float(cost[m].mean()), "simulated_cost": float(sim_cost[m].mean())}
    return res


@dataclass
class Sim:
    mask: np.ndarray  # test worlds that get a layout to simulate with
    row: np.ndarray  # layout row per test world
    codes: np.ndarray
    own: bool  # the world's own layout (True) or a random one


def main() -> None:
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("--data", required=True)
    p.add_argument("--tag", required=True)
    p.add_argument("--out", help="output directory (default DATA/reports/TAG)")
    p.add_argument("--layouts-tag", help="layouts file tag (default: the biome scale of --tag)")
    p.add_argument("--variants", default="flat,3d", help="xray down model(s): flat, 3d (default both)")
    p.add_argument("--cell", type=int, default=64)
    p.add_argument("--min-group", type=int, default=2000)
    p.add_argument("--test-share", type=float, default=0.3)
    p.add_argument("--budget", type=float, default=float("inf"),
                   help="finds later than this many seconds after spawning do not count (default: no limit)")
    p.add_argument("--a-cands", type=int, default=40, help="first spots tried per action for two-spot plans")
    p.add_argument("--b-cands", type=int, default=800, help="second spots tried per action for two-spot plans")
    p.add_argument("--reset", type=float, default=10.0)
    p.add_argument("--look", type=float, default=5.0)
    p.add_argument("--dig", type=float, default=5.0)
    p.add_argument("--xray", type=float, default=6.0)
    p.add_argument("--look-dir", type=float, default=30.0)
    p.add_argument("--speed", type=float, default=5.0)
    a = p.parse_args()
    t0 = time.time()
    log = lambda s: print(f"[{time.time() - t0:6.1f}s] {s}", flush=True)  # noqa: E731

    data = Path(a.data)
    run = S.load_run(data, a.tag)
    ltag = a.layouts_tag or run.info["biome_scale"]
    lrun = json.loads((data / f"layouts_run_{ltag}.json").read_text())
    own_layouts = lrun["run_seed"] == run.info["run_seed"] and not run.info.get("balanced_only") and \
        lrun["first_sample"] == 0 and lrun["biome_scale"] == run.info["biome_scale"] and \
        bool(lrun["balanced_only"]) == bool(run.info.get("balanced_only"))
    cfg = Cfg(a.reset, a.look, a.dig, a.xray, a.look_dir, a.speed, a.budget)
    out = Path(a.out) if a.out else data / "reports" / a.tag
    out.mkdir(parents=True, exist_ok=True)

    df = S.load_seeds(run, columns=["sample", "player_x", "player_z", "sh_x", "sh_z"])
    rng = np.random.default_rng(12345)
    train = rng.random(len(df)) >= a.test_share  # same split as report.py
    px, pz = df.player_x.to_numpy().astype(np.float64), df.player_z.to_numpy().astype(np.float64)
    cx = np.floor_divide(df.sh_x.to_numpy().astype(np.int64), 16)
    cz = np.floor_divide(df.sh_z.to_numpy().astype(np.int64), 16)
    chunks = S.chunk_index(df.sh_x, df.sh_z)
    sample = df["sample"].to_numpy().astype(np.int64)
    log(f"{len(df):,} worlds, layouts from '{ltag}'" + (" (own layouts for the cross-check)" if own_layouts else ""))

    # groups: everyone, and spawn cells with enough training worlds
    cell = (np.floor_divide(px.astype(np.int64), a.cell) + 500) * 1000 + np.floor_divide(pz.astype(np.int64), a.cell) + 500
    counts = pd.Series(cell[train]).value_counts()
    big = np.sort(counts[counts >= a.min_group].index.to_numpy())
    sub_rng = np.random.default_rng(7)

    def make_group(name, mask):
        idx = np.flatnonzero(mask & train)
        h = S.chunk_histogram(chunks[idx]).reshape(S.CHUNK_SIDE, S.CHUNK_SIDE)
        pick = idx if len(idx) <= 3000 else sub_rng.choice(idx, 3000, replace=False)
        walk = np.sqrt((ALL_XZ[None, :, 0] - px[pick, None]) ** 2 + (ALL_XZ[None, :, 1] - pz[pick, None]) ** 2)
        return Group(name, len(idx) / train.sum(), h, walk.mean(0) / a.speed)

    everyone = [make_group("everyone", np.ones(len(df), bool))]
    cells = [make_group(str(c), cell == c) for c in big]
    rest = ~np.isin(cell, big)
    if (rest & train).any():
        cells.append(make_group("other", rest))
    log(f"{len(cells)} spawn-cell groups")

    te = np.flatnonzero(~train)
    test = Worlds(px[te], pz[te], 2 * cx[te] + 2 * S.CHUNK_HALF, 2 * cz[te] + 2 * S.CHUNK_HALF, sample[te],
                  np.zeros(len(te), np.int64))
    cell_labels = np.searchsorted(big, cell[te])
    cell_labels = np.where((cell_labels < len(big)) & (big[np.minimum(cell_labels, len(big) - 1)] == cell[te]),
                           cell_labels, len(cells) - 1)

    results = {"tag": a.tag, "layouts": ltag, "budget": a.budget, "variants": {}}
    md = [f"# Multi-step plans: {run.info['biome_scale']} biomes" +
          (" (Find Balanced Seed ON)" if run.info.get("balanced_only") else ""),
          "",
          f"Generated {time.strftime('%Y-%m-%d %H:%M')} from `{data}` (tag `{a.tag}`, layouts `{ltag}`, "
          f"{lrun['worlds']:,} real layouts). Plans are picked on {train.mean():.0%} of the {len(df):,} worlds and "
          f"scored on the other {1 - train.mean():.0%}.",
          "",
          f"Time model: reset {cfg.reset:g}s, look around {cfg.look:g}s, dig in (3 deep + ladder + sand) {cfg.dig:g}s "
          f"per spot, xray down {cfg.xray:g}s, one directional look {cfg.look_dir:g}s, travel {cfg.speed:g} blocks/s"
          + (f", finds later than {cfg.budget:g}s after spawning do not count" if np.isfinite(cfg.budget) else "")
          + ". Spots are on an 8-block grid. 'look' = xray facing one direction: a piece up to 100 blocks ahead in a "
            "90 degree wedge.",
          ""]

    lams = np.geomspace(40, 3000, 72)
    for vkey in [v.strip() for v in a.variants.split(",") if v.strip()]:
        lay = load_layouts(data, ltag, BIT[VARIANTS[vkey]], log)
        _SEQ_CACHE.clear()
        for g in everyone + cells:
            group_maps(g, lay)
        log(f"variant {vkey}: group maps done")
        ps = PairSearch(lay)
        jds = {(x, y): ps.jd(x, y) for x in range(5) for y in range(5)}

        sim = None
        if own_layouts:
            pos = np.searchsorted(lay.sample, test.sample)
            ok = (pos < lay.n) & (lay.sample[np.minimum(pos, lay.n - 1)] == test.sample)
            same = np.all(lay.chunk[np.minimum(pos, lay.n - 1)] == np.column_stack([cx[te], cz[te]]), axis=1)
            if not same[ok].all():
                raise SystemExit("layouts file does not match this dataset (different start chunks)")
            sim = Sim(ok, np.where(ok, pos, 0), lay.codes, True)
        else:
            m = np.zeros(len(te), bool)
            m[sub_rng.choice(len(te), min(len(te), 200000), replace=False)] = True
            sim = Sim(m, sub_rng.integers(0, lay.n, len(te)), lay.codes, False)

        # first spots for two-spot plans: best single-action spots at the one-spot optimum
        single = [run_grouping("everyone", everyone, np.zeros(len(te), np.int64), lay, cfg, lams, "spot", ps, jds,
                               {}, a, test, None, log, allow_reset=False)]
        lam1 = single[0]["lam"]

        def cands(g: Group) -> dict:
            out_c = {}
            for act in range(5):
                pa = 1.0 - g.none[1 << act]
                v = lam1 * pa - g.walk
                out_c[act] = np.unique(np.concatenate([np.argsort(-v)[:a.a_cands], np.argsort(-pa)[:a.a_cands // 2]]))
            return out_c

        a_cands = {g.name: cands(g) for g in everyone + cells}
        vres, rows = {}, []
        base_key = None
        cells_name = f"plan per spawn cell ({a.cell} blocks), reset bad cells"
        for fam, label, _ in FAMILIES:
            for gname, groups, labels, allow in (("same plan for everyone", everyone, np.zeros(len(te), np.int64), False),
                                                 (cells_name, cells, cell_labels, True)):
                r = run_grouping(gname, groups, labels, lay, cfg, lams, fam, ps, jds, a_cands, a, test, sim, log,
                                 allow_reset=allow)
                vres[(fam, gname)] = r
                if base_key is None:
                    base_key = (fam, gname)
                log(f"{vkey} {fam:5s} {gname[:24]:24s}: {r['s_per_find']:.0f} s/find")
        base = vres[base_key]["s_per_find"]
        for (fam, gname), r in vres.items():
            rows.append({"plans": dict((f[0], f[1]) for f in FAMILIES)[fam], "grouping": gname,
                         "worlds played out": f"{r['played']:.0%}",
                         "chance when you go": f"{100 * r['chance_when_go']:.1f}%",
                         "seconds when you go": f"{r['seconds_when_go']:.0f}",
                         "seconds per find": f"{r['s_per_find']:.0f}",
                         "speed-up": f"{base / r['s_per_find']:.2f}x",
                         "finds <= 60s / 90s after spawn": f"{r['time'].get('<= 60s', 0):.0%} / "
                                                           f"{r['time'].get('<= 90s', 0):.0%}"})

        md += [f"## Xray down = {'30 blocks flat' if vkey == 'flat' else '30 blocks 3D from eye y 62'}", "",
               *mdt(pd.DataFrame(rows))]
        # the plans themselves
        best_same = min((r for (f, gname), r in vres.items() if gname.startswith("same")), key=lambda r: r["s_per_find"])
        md += ["Best plan for everyone: " + best_same["plans"][0].describe() + ".", ""]
        q = vres[("down2", "same plan for everyone")]
        pl = q["plans"][0]
        if pl.kind == "pair":
            st = pair_detail(pl, everyone[0], lay)
            md += [f"Down xrays only, same plan for everyone: {pl.describe()}. First xray "
                   f"{100 * st['p1']:.1f}%, second one {100 * st['p2_given_fail']:.1f}% of the worlds where the first "
                   f"failed ({100 * st['p2_alone']:.1f}% on its own, so the two overlap by "
                   f"{100 * (st['p2_alone'] - st['p2_given_fail'] * (1 - st['p1'])):.1f} points); "
                   f"{q['s_per_find']:.0f} vs {vres[('down', 'same plan for everyone')]['s_per_find']:.0f} s/find.", ""]
        else:
            md += ["A second xray down elsewhere never pays off under this time model (same plan for everyone).", ""]
        for title, key in (("down xrays only", ("down2", cells_name)),
                           ("everything", min((k for k in vres if k[1] == cells_name),
                                              key=lambda k: vres[k]["s_per_find"]))):
            bc = vres[key]
            tbl = plan_table(bc, cells, a.cell)
            csv_name = f"multistep_plans_{vkey}_{key[0]}.csv"
            tbl.to_csv(out / csv_name, index=False)
            md += [f"### Plan per spawn cell, {title} ({dict((f[0], f[1]) for f in FAMILIES)[key[0]]}): "
                   f"{bc['s_per_find']:.0f} s/find", "",
                   f"Largest cells (all {len(tbl)} in `{csv_name}`); 'chance' and 'seconds' are on test worlds of that "
                   f"cell, seconds after the look-around.", "",
                   *mdt(tbl.head(20)), ""]
            if "sim" in bc:
                s = bc["sim"]
                md += [f"Check: {s['worlds']:,} test worlds simulated with "
                       f"{'their own real layouts' if own_layouts else 'random real layouts'}: "
                       f"{100 * s['simulated']:.2f}% found vs {100 * s['expected']:.2f}% expected "
                       f"(+- {100 * s['se']:.2f}), seconds {s['simulated_cost']:.1f} vs {s['expected_cost']:.1f}.", ""]
        results["variants"][vkey] = {
            f"{fam}|{gname}": {k: v for k, v in r.items() if k in ("s_per_find", "train_s_per_find", "played",
                                                                   "chance_when_go", "seconds_when_go", "time", "sim",
                                                                   "lam")}
            | {"plan_everyone": r["plans"][0].describe() if gname.startswith("same") else None}
            for (fam, gname), r in vres.items()}
        del lay, jds

    md += ["## Notes", "",
           "* 'Chance when you go' counts a find at any step of the plan. Seconds exclude the reset and the look-around, "
           "which every attempt pays.",
           "* A plan per spawn cell sees only your map coordinates; biomes are ignored here because they did not help "
           "in report.md.",
           "* Two-spot plans allow one xray per spot; a sequence of looks happens only at one spot.",
           "* Travel is a straight line; water and terrain are ignored. Pieces intersecting water are kept.", ""]
    (out / "multistep.md").write_text("\n".join(md))
    (out / "multistep.json").write_text(json.dumps(results, indent=2, default=float))
    if not a.out:
        import report  # noqa: E402  (summary table lives there)
        report.write_summary(out.parent)
    log(f"wrote {out / 'multistep.md'}")


def pair_detail(plan: Plan, g: Group, lay: Layouts) -> dict:
    p1 = 1.0 - g.none[1 << plan.actions[0]][plan.spot1]
    p2 = 1.0 - g.none[1 << plan.action2][plan.spot2]
    H = PairSearch.chunk_weights(g, np.array([plan.spot1]))
    iz1, ix1 = divmod(plan.spot1, G8)
    iz2, ix2 = divmod(plan.spot2, G8)
    dz, dx = iz2 - iz1 + LS - 1, ix2 - ix1 + LS - 1
    both = 0.0
    if 0 <= dz < DS and 0 <= dx < DS:
        uz, ux = np.divmod(np.arange(LC), LS)
        fz, fx = uz + dz - (LS - 1), ux + dx - (LS - 1)
        ok = (fz >= 0) & (fz < LS) & (fx >= 0) & (fx < LS)
        j = np.zeros(LC, np.float32)
        j[ok] = lay.joint[plan.actions[0], plan.action2][np.arange(LC)[ok], fz[ok] * LS + fx[ok]]
        both = float(H[0] @ j)
    return {"p1": float(p1), "p2_alone": float(p2), "p2_given_fail": float((p2 - both) / max(1 - p1, 1e-12))}


def plan_table(r: dict, groups: list[Group], size: int) -> pd.DataFrame:
    rows = []
    for g, plan, pg in zip(groups, r["plans"], r["per_group"]):
        if g.name == "other":
            where = ("other cells", "")
        else:
            c = int(g.name)
            gx, gz = c // 1000 - 500, c % 1000 - 500
            where = (f"{gx * size}..{gx * size + size - 1}", f"{gz * size}..{gz * size + size - 1}")
        n = max(pg["worlds"], 1)
        rows.append({"you spawn at x": where[0], "z": where[1], "worlds": f"{g.share:.1%}", "plan": plan.describe(),
                     "chance": f"{100 * pg['found'] / n:.1f}%" if plan.kind != "reset" else "-",
                     "seconds": f"{pg['cost'] / n:.0f}" if plan.kind != "reset" else "0", "_s": g.share})
    return pd.DataFrame(rows).sort_values("_s", ascending=False).drop(columns="_s")


def mdt(df: pd.DataFrame) -> list[str]:
    cols = [str(c) for c in df.columns]
    out = ["| " + " | ".join(cols) + " |", "|" + "|".join("---" for _ in cols) + "|"]
    out += ["| " + " | ".join(str(v) for v in row) + " |" for row in df.itertuples(index=False)]
    return out + [""]


if __name__ == "__main__":
    main()
