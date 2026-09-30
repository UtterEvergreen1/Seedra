#!/usr/bin/env python3
"""Stronghold report for one dataset run of EXE_StrongholdMining.

    python report.py --data DATA_DIR --tag small            (tag = scale, or e.g. small_balanced)

Writes DATA_DIR/reports/<tag>/report.md with images and CSV tables, plus summary.json, and refreshes
DATA_DIR/reports/summary.md, which compares every report in that folder side by side.
"""
from __future__ import annotations

import argparse
import json
import sys
import time
from dataclasses import dataclass, field
from pathlib import Path

import numpy as np
import pandas as pd

sys.path.insert(0, str(Path(__file__).resolve().parent))  # run from any directory
import shlib as S  # noqa: E402

DIRECTIONS = {"dir_n": "north", "dir_e": "east", "dir_s": "south", "dir_w": "west"}


@dataclass(frozen=True)
class Method:
    key: str
    kinds: tuple  # one kernel, or several when you also choose (e.g. the look direction)
    radius: int
    label: str
    what: str

    def choice(self, k: int) -> str:
        return f", look {DIRECTIONS[self.kinds[k]]}" if len(self.kinds) > 1 else ""


METHODS = [
    Method("dig", ("any_h",), 0, "Blind dig",
           "dig straight down at the spot; success = that column touches a stronghold piece"),
    Method("xray", ("any_h",), 30, "Xray down (30 flat)",
           "one xray looking down; success = a piece within 30 blocks horizontally, at any depth"),
    Method("xray3d", ("any_3d",), 30, "Xray down (30 3D)",
           "same, but the 30 blocks are 3D distance from your eye (y = EYE) to the top of the piece"),
    Method("look", tuple(DIRECTIONS), 100, "Xray one direction (100)",
           "one deliberate xray facing north, east, south or west (you pick); success = a piece up to 100 blocks "
           "ahead inside a 90 degree wedge"),
    Method("xray100", ("any_h",), 100, "Xray all 4 directions (100)",
           "four directional xrays; success = a piece within 100 blocks horizontally"),
    Method("portal30", ("portal_h",), 30, "Portal room within 30",
           "the portal room itself is within 30 blocks horizontally of the spot"),
]
METHOD = {m.key: m for m in METHODS}


def pct(p, digits: int = 1) -> str:
    return "n/a" if p is None or not np.isfinite(p) else f"{100 * p:.{digits}f}%"


def md_table(df: pd.DataFrame) -> list[str]:
    cols = [str(c) for c in df.columns]
    out = ["| " + " | ".join(cols) + " |", "|" + "|".join("---" for _ in cols) + "|"]
    out += ["| " + " | ".join(str(v) for v in row) + " |" for row in df.itertuples(index=False)]
    return out + [""]


@dataclass
class Choice:
    """A spot (and look direction, for 'look'), with its chance on training and test worlds."""
    k: int
    x: int
    z: int
    p_train: float
    p_test: float = float("nan")


@dataclass
class Ctx:
    args: argparse.Namespace
    run: S.Run
    out: Path
    df: pd.DataFrame
    chunks: np.ndarray
    train: np.ndarray
    kernels: dict  # kernel name -> kernel for the radius of each method: (kind, radius) -> array
    mappers: dict  # (kind, radius) -> HeatMapper
    tm: S.TimeModel
    kernel_info: dict = field(default_factory=dict)
    md: list = field(default_factory=list)
    summary: dict = field(default_factory=dict)

    def add(self, *lines: str) -> None:
        self.md.extend(lines)

    def kernel(self, m: Method, k: int = 0) -> np.ndarray:
        return self.kernels[(m.kinds[k], m.radius)]

    def heats(self, m: Method, mask: np.ndarray) -> list[np.ndarray]:
        h = S.chunk_histogram(self.chunks[mask])
        return [self.mappers[(kind, m.radius)](h) for kind in m.kinds]

    def best(self, m: Method, mask: np.ndarray) -> Choice:
        options = [S.best_point(h) for h in self.heats(m, mask)]
        k = int(np.argmax([o[2] for o in options]))
        return Choice(k, *options[k])

    def success(self, m: Method, choice: Choice, mask: np.ndarray) -> np.ndarray:
        d = self.df
        return S.success_at(self.kernel(m, choice.k), choice.x, choice.z, d.sh_x.to_numpy()[mask],
                            d.sh_z.to_numpy()[mask])

    def fit_and_test(self, m: Method, mask: np.ndarray) -> Choice:
        """Best choice from the training worlds in `mask`, chance measured on its test worlds."""
        ch = self.best(m, mask & self.train)
        te = mask & ~self.train
        ch.p_test = float(self.success(m, ch, te).mean()) if te.any() else float("nan")
        return ch

    def action_seconds(self, m: Method) -> float:
        """Seconds spent at the spot: dig in (3 deep + ladder + sand) and xray, or dig all the way down."""
        a, tm = self.args, self.tm
        return {"dig": a.dig_down, "xray": tm.dig + tm.xray, "xray3d": tm.dig + tm.xray,
                "look": tm.dig + a.look_dir, "xray100": tm.dig + 4 * a.look_dir, "portal30": tm.dig + tm.xray}[m.key]


# ------------------------------------------------------------------------------------------------ setup

def section_setup(c: Ctx) -> None:
    info, a = c.run.info, c.args
    n = len(c.df)
    bal = info.get("balanced_worlds")
    c.add(f"# Stronghold report: {info['console']} {info['version']}, classic world, {info['biome_scale']} biomes"
          + (" (Find Balanced Seed ON)" if info.get("balanced_only") else ""),
          "",
          f"Generated {time.strftime('%Y-%m-%d %H:%M')} from `{c.run.directory}` (tag `{c.run.tag}`).",
          "",
          f"* Worlds: **{n:,}** (run seed {info['run_seed']}, samples {info['first_sample']:,} .. "
          f"{info['first_sample'] + n - 1:,}); {c.train.mean():.0%} pick the spots, the other {1 - c.train.mean():.0%} "
          f"test them.",
          "* Balanced worlds: " + ("every world (seeds found the way Find Balanced Seed does)" if info.get("balanced_only")
                                   else f"{bal:,} ({bal / n:.2%})" if bal is not None else "not checked"),
          f"* Kernels (what each method can see around a stronghold) measured on "
          f"{c.kernel_info['kernel_worlds']:,} worlds of run `{c.kernel_info['tag']}`.",
          f"* What you are assumed to see: your coordinates on the map (you appear up to {info['player_spawn_radius']} "
          f"blocks from the world spawn), the biome group you stand in, and whether points every {info['grid_step']} "
          f"blocks around you are water (ocean/river) or land a stronghold can use. 'Close' = within {a.radius} "
          f"blocks (about the starting map), 'far' = within {a.radius_far}.",
          f"* Time model: {c.tm.describe()}, one directional xray {a.look_dir:g}s, blind dig down {a.dig_down:g}s.",
          "",
          "Methods (chances are averaged over stronghold layouts, so they are exact in expectation):",
          "")
    for m in METHODS:
        c.add(f"* **{m.label}**: {m.what.replace('EYE', str(info['eye_y']))}.")
    c.add("")


def section_validation(c: Ctx) -> None:
    direct = S.load_direct_heatmap(c.run, "any")
    conv = c.heats(METHOD["dig"], np.ones(len(c.df), bool))[0]
    diff = np.abs(conv - direct)
    peak = direct.max()
    sd = np.sqrt(peak * (1 - peak) / len(c.df))
    info = c.run.info
    c.summary["validation"] = {"max_abs_diff": float(diff.max()), "peak_sd": float(sd)}
    c.add("## 0. Checks",
          "",
          f"* Heat map from start chunks x kernel vs. heat map from every world's real layout: largest difference "
          f"{100 * diff.max():.2f} points; random noise at the peak is {100 * sd:.2f} points "
          f"({diff.max() / sd:.1f} sd over {diff.size:,} columns, which is what noise alone gives).",
          f"* Pieces outside the kernel window: {info['pieces_outside_kernel_window']}, clipped by the world "
          f"border: {info['pieces_clipped_by_world']}. Placement was re-checked against Seedra for every world.",
          f"* Placement retries: {info['placement_retries']:,} worlds ({info['placement_retries'] / len(c.df):.2%}),"
          f" no valid biome at all: {info['placement_fallbacks']:,}.",
          "")


# ---------------------------------------------------------------------------------------------- baseline

def section_baseline(c: Ctx) -> None:
    everyone = np.ones(len(c.df), bool)
    balanced = c.df.balanced.to_numpy() == 1
    rows, base = [], {}
    for m in METHODS:
        heats = c.heats(m, everyone)
        ch = c.best(m, everyone)
        walk = np.hypot(c.df.player_x - ch.x, c.df.player_z - ch.z).mean()
        row = {"method": m.label, "best spot (x, z)": f"({ch.x}, {ch.z}){m.choice(ch.k)}", "chance": pct(ch.p_train),
               "avg distance from you": f"{walk:.0f}"}
        if 1000 <= balanced.sum() < len(c.df):
            b = c.best(m, balanced)
            row["balanced worlds only"] = f"{pct(b.p_train)} at ({b.x}, {b.z}){m.choice(b.k)}"
        rows.append(row)
        base[m.key] = {"x": ch.x, "z": ch.z, "k": ch.k, "p": ch.p_train}
        if m.key in ("dig", "xray", "xray3d"):
            S.plot_heatmap(heats[0], c.out / f"baseline_{m.key}.png", f"{m.label}: chance per spot, no information",
                           marks=[(ch.x, ch.z, pct(ch.p_train))])
    c.summary["baseline"] = base
    c.add("## 1. Baseline: no information, always the same spot",
          "",
          *md_table(pd.DataFrame(rows)),
          "![blind dig](baseline_dig.png) ![xray](baseline_xray.png)",
          "")
    if 0 < balanced.sum() < 1000 and not c.run.info.get("balanced_only"):
        c.add(f"Only {balanced.sum()} balanced worlds in this run; `dataset --balanced-only` generates that case.", "")


# ------------------------------------------------------------------------------------------------ oracle

def section_oracle(c: Ctx) -> None:
    o = S.load_oracle(c.run)
    title = "## 2. Ceiling: if you knew every biome"
    if o is None:
        c.add(title, "", f"No `oracle_{c.run.tag}.csv` yet: run `EXE_StrongholdMining oracle` with the same options.",
              "")
        return
    joined = o.merge(c.df[["sample", "sh_x", "sh_z"]], on="sample", how="inner", suffixes=("", "_ds"))
    if len(joined) == 0 or ((joined.sh_x != joined.sh_x_ds) | (joined.sh_z != joined.sh_z_ds)).any():
        c.add(title, "", "The oracle file does not match this dataset (different run seed?), skipped.", "")
        return
    rows, res = [], {}
    for m in METHODS:
        name = f"{m.kinds[0]}_r{m.radius}"
        if len(m.kinds) != 1 or f"{name}_p_cross" not in o:
            continue
        b = c.summary["baseline"][m.key]
        same = S.success_at(c.kernel(m), b["x"], b["z"], joined.sh_x, joined.sh_z).mean()
        p, hit = o[f"{name}_p_cross"], o[f"{name}_hit"]
        rows.append({"method": m.label, "same spot (these worlds)": pct(same), "all biomes known": pct(p.mean()),
                     "real hits": f"{pct(hit.mean())} +- {pct(hit.std() / np.sqrt(len(hit)))}",
                     "median world": pct(p.median()), "best 10% of worlds": f">= {pct(p.quantile(0.9))}",
                     "worlds >= 2x": f"{(p >= 2 * same).mean():.0%}"})
        res[m.key] = {"p": float(p.mean()), "same_spot": float(same)}
    c.summary["oracle"] = res
    c.add(title,
          "",
          f"For {len(o):,} worlds the tool kept the real biome map and re-ran the stronghold placement with "
          f"{int(o.streams.iloc[0]):,} random RNG streams: where the stronghold could be on exactly this map. The best "
          "spot is picked on half of the streams and scored on the other half (so it is not optimistic); 'real hits' "
          "checks it against the world's actual stronghold. No rule based on biomes can beat this column.",
          "",
          *md_table(pd.DataFrame(rows)))


# -------------------------------------------------------------------------------------- single features

CELL_OFFSET = 500  # spawn cell id = (cx + 500) * 1000 + (cz + 500)


def spawn_cells(c: Ctx, size: int) -> np.ndarray:
    cx = np.floor_divide(c.df.player_x.to_numpy().astype(np.int64), size) + CELL_OFFSET
    cz = np.floor_divide(c.df.player_z.to_numpy().astype(np.int64), size) + CELL_OFFSET
    return cx * 1000 + cz


def cell_xz(cell: int) -> tuple[int, int]:
    cx, cz = divmod(int(cell), 1000)
    return cx - CELL_OFFSET, cz - CELL_OFFSET


def section_single(c: Ctx, sub: np.ndarray) -> None:
    a = c.args
    m = METHOD[a.method]
    base = c.fit_and_test(m, np.ones(len(c.df), bool))
    c.summary["baseline_test"] = base.p_test
    c.add(f"## 3. One thing at a time ({m.label})",
          "",
          f"Each group gets its own best spot (picked on training worlds, scored on test worlds). Same spot for "
          f"everyone scores {pct(base.p_test)} on the test worlds.",
          "")

    def row(mask: np.ndarray) -> dict:
        ch = c.fit_and_test(m, mask)
        return {"worlds": f"{mask.mean():.1%}", "go to (x, z)": f"({ch.x}, {ch.z}){m.choice(ch.k)}",
                "chance (test)": pct(ch.p_test), "lift": f"{ch.p_test / base.p_test:.2f}x", "_p": ch.p_test,
                "_x": ch.x, "_z": ch.z, "_k": ch.k}

    # 3a. spawn cells
    cells = spawn_cells(c, a.cell)
    rows = []
    for g in np.unique(cells):
        mask = cells == g
        if (mask & c.train).sum() < a.min_group or (mask & ~c.train).sum() < a.min_group // 3:
            continue
        cx, cz = cell_xz(g)
        rows.append({"you spawn at x": f"{cx * a.cell}..{cx * a.cell + a.cell - 1}",
                     "z": f"{cz * a.cell}..{cz * a.cell + a.cell - 1}", "_cx": cx, "_cz": cz, "_share": mask.mean(),
                     **row(mask)})
    t = pd.DataFrame(rows)
    if len(t):
        t = t.sort_values("_p", ascending=False)
        t.drop(columns=["_share"]).to_csv(c.out / "spawn_cells.csv", index=False)
        plot_cells(t, a.cell, c.out / "spawn_cells.png", f"{m.label}: best chance by where you spawn")
        covered = t._share.sum()
        weighted = float((t._share * t._p).sum() / covered)
        c.summary["spawn_lookup_test"] = weighted
        c.add(f"### 3a. Where you spawn ({a.cell} x {a.cell} block cells of your map coordinates)",
              "",
              f"Cells with enough worlds cover {covered:.0%} of spawns; with each cell's own spot they score "
              f"{pct(weighted)} (vs {pct(base.p_test)}). Best cells:",
              "",
              *md_table(t.head(12)[[col for col in t.columns if not col.startswith("_")]]),
              "![spawn cells](spawn_cells.png)", "", "All cells: `spawn_cells.csv`.", "")

    # 3b. the biome you stand in
    group = S.biome_table(c.df.player_biome.to_numpy(), S.GROUP_INDEX)
    rows = [{"you stand in": name, **row(group == gi)} for gi, name in enumerate(S.GROUPS)
            if ((group == gi) & c.train).sum() >= a.min_group]
    if rows:
        t = pd.DataFrame(rows)
        c.add("### 3b. The biome you stand in", "", *md_table(t[[col for col in t.columns if not col.startswith("_")]]))

    # 3c. water around you
    fs = S.observable_features(c.df.iloc[sub], c.run, a.radius)
    rows = []
    for s in ("north", "east", "south", "west"):
        share = fs.X[:, fs.names.index(f"water_share_{s}")]
        for label, cond in (("mostly water", share >= 0.5), ("mostly land", share < 0.5)):
            mask = np.zeros(len(c.df), bool)
            mask[sub[cond]] = True
            if (mask & c.train).sum() >= a.min_group:
                rows.append({"direction": s, f"within {a.radius} blocks": label, **row(mask)})
    if rows:
        t = pd.DataFrame(rows)
        c.add(f"### 3c. Water around you (within {a.radius} blocks, per direction)", "",
              *md_table(t[[col for col in t.columns if not col.startswith("_")]]))


def plot_cells(t: pd.DataFrame, size: int, path: Path, title: str) -> None:
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt

    lo = -(S.WORLD_HALF // size) - 1
    n = S.WORLD_SIDE // size + 2
    img = np.full((n, n), np.nan)
    cells = list(zip(t["_cx"], t["_cz"], t["_p"]))
    for cx, cz, p in cells:
        img[cz - lo, cx - lo] = p * 100
    fig, ax = plt.subplots(figsize=(6.4, 5.6), dpi=110)
    im = ax.imshow(img, extent=(lo * size, (lo + n) * size, (lo + n) * size, lo * size), cmap="viridis", origin="upper")
    fig.colorbar(im, ax=ax, label="chance with this cell's own spot (%)")
    for cx, cz, p in cells:
        ax.text((cx + 0.5) * size, (cz + 0.5) * size, f"{100 * p:.0f}", ha="center", va="center", fontsize=6,
                color="white")
    ax.set_xlabel("your x at spawn (east ->)")
    ax.set_ylabel("your z at spawn (south ->)")
    ax.set_title(title, fontsize=10)
    fig.tight_layout()
    fig.savefig(path)
    plt.close(fig)


# ------------------------------------------------------------------------------------------------ rules

@dataclass
class Rules:
    method: Method
    radius: int
    tree: S.PolicyTree
    leaf: np.ndarray  # leaf id of every world in the sub-sample
    choices: list  # Choice per leaf
    p_test: float
    same_spot_test: float


def fit_rules(c: Ctx, m: Method, radius: int, sub: np.ndarray) -> Rules:
    a = c.args
    d = c.df.iloc[sub]
    fs = S.observable_features(d, c.run, radius)
    tr = c.train[sub]
    chunks = c.chunks[sub]
    min_leaf = max(a.min_leaf, int(a.min_leaf_share * tr.sum()))
    tree = S.PolicyTree([c.kernel(m, k) for k in range(len(m.kinds))], max_depth=a.depth, min_leaf=min_leaf,
                        min_gain=a.min_gain)
    tree.fit(fs, chunks, rows=np.flatnonzero(tr))
    leaf = tree.apply(fs.X)
    in_sub = np.zeros(len(c.df), bool)
    choices, found = [], np.zeros(len(sub))
    for lf in tree.leaves:
        mask = np.zeros(len(c.df), bool)
        mask[sub[leaf == lf.leaf_id]] = True
        ch = c.fit_and_test(m, mask)
        choices.append(ch)
        te = (leaf == lf.leaf_id) & ~tr
        found[te] = S.success_at(c.kernel(m, ch.k), ch.x, ch.z, d.sh_x.to_numpy()[te], d.sh_z.to_numpy()[te])
    in_sub[sub] = True
    same = c.fit_and_test(m, in_sub).p_test
    return Rules(m, radius, tree, leaf, choices, float(found[~tr].mean()), same)


def section_rules(c: Ctx, sub: np.ndarray) -> dict:
    a = c.args
    plan = [(a.method, a.radius), (a.method, a.radius_far), ("look", a.radius), ("dig", a.radius)]
    results = {}
    for key, radius in dict.fromkeys(plan):
        results[(key, radius)] = fit_rules(c, METHOD[key], radius, sub)
        print(f"         rules {key} r{radius}: {len(results[(key, radius)].tree.leaves)} leaves")
    c.summary["rules_test"] = {f"{k}_r{r}": v.p_test for (k, r), v in results.items()}
    c.summary["rules_same_spot_test"] = {f"{k}_r{r}": v.same_spot_test for (k, r), v in results.items()}

    c.add("## 4. Rules (decision tree over what you can see)",
          "",
          "A decision tree whose split rule is the goal itself: at every split it asks 'how many more strongholds do "
          "we find if each side gets its own spot?'. Rules are learned on the training worlds; chances are measured "
          "on the test worlds.",
          "",
          *md_table(pd.DataFrame([{"method": r.method.label, "you look": f"within {r.radius}",
                                   "rules": len(r.tree.leaves), "same spot (test)": pct(r.same_spot_test),
                                   "rules (test)": pct(r.p_test), "gain": f"{r.p_test / r.same_spot_test:.2f}x"}
                                  for r in results.values()])))
    for key, radius in dict.fromkeys([(a.method, a.radius), ("look", a.radius)]):
        r = results[(key, radius)]
        n = len(r.leaf)
        t = pd.DataFrame([{"if": " AND ".join(lf.conditions) or "(every world)",
                           "worlds": f"{(r.leaf == lf.leaf_id).sum() / n:.1%}",
                           "go to (x, z)": f"({ch.x}, {ch.z}){r.method.choice(ch.k)}",
                           "chance (test)": pct(ch.p_test), "lift": f"{ch.p_test / r.same_spot_test:.2f}x",
                           "_p": ch.p_test} for lf, ch in zip(r.tree.leaves, r.choices)])
        t = t.sort_values("_p", ascending=False).drop(columns="_p")
        t.to_csv(c.out / f"rules_{key}.csv", index=False)
        c.add(f"### Rules for {r.method.label}, looking within {radius} blocks", "", *md_table(t),
              f"(also in `rules_{key}.csv`)", "")
    r = results[(a.method, a.radius)]
    scores = np.array([ch.p_test for ch in r.choices], dtype=float)
    if np.isfinite(scores).any():
        best_leaf = int(np.nanargmax(scores))
        mask = np.zeros(len(c.df), bool)
        mask[sub[r.leaf == best_leaf]] = True
        ch = r.choices[best_leaf]
        S.plot_heatmap(c.heats(r.method, mask)[ch.k], c.out / "best_rule.png",
                       "best rule: " + (" AND ".join(r.tree.leaves[best_leaf].conditions) or "every world")[:80],
                       marks=[(ch.x, ch.z, pct(ch.p_test))])
        c.add("![best rule](best_rule.png)", "")
    return results


# ------------------------------------------------------------------------------------------------ timing

def time_table(c: Ctx, m: Method, sub: np.ndarray, rules: Rules) -> tuple[list[dict], dict]:
    a, tm = c.args, c.tm
    act = c.action_seconds(m)
    d = c.df.iloc[sub]
    tr, te = c.train[sub], ~c.train[sub]
    px, pz = d.player_x.to_numpy(), d.player_z.to_numpy()
    shx, shz = d.sh_x.to_numpy(), d.sh_z.to_numpy()
    chunks = c.chunks[sub]
    spots = S.candidate_spots(8)
    J = len(spots)
    rng = np.random.default_rng(1)
    cells = spawn_cells(c, a.cell)[sub]
    counts = pd.Series(cells[tr]).value_counts()
    cells = np.where(np.isin(cells, counts[counts >= a.min_group].index.to_numpy()), cells, -1)
    groupings = {"same spot for every world": np.zeros(len(sub), dtype=np.int64),
                 f"spot by spawn cell ({a.cell} blocks)": cells,
                 f"rules (within {rules.radius})": rules.leaf}
    model = S.TimeModel(tm.reset, tm.look, 0.0, act, tm.speed)  # optimise_time charges dig + xray = act
    rows, res = [], {}
    for name, labels in groupings.items():
        groups = np.unique(labels[tr])
        ps, walks, shares = [], [], []
        for g in groups:
            mask = (labels == g) & tr
            h = S.chunk_histogram(chunks[mask])
            ps.append(np.concatenate([c.mappers[(kind, m.radius)](h)[spots[:, 1] + S.WORLD_HALF,
                                                                      spots[:, 0] + S.WORLD_HALF] for kind in m.kinds]))
            walks.append(np.tile(S.mean_walk(px[mask], pz[mask], spots, tm.speed, rng=rng), len(m.kinds)))
            shares.append(mask.sum())
        shares = np.asarray(shares, float) / np.sum(shares)
        gi = np.clip(np.searchsorted(groups, labels), 0, len(groups) - 1)
        gi[groups[gi] != labels] = int(np.argmax(shares))  # unseen group -> biggest group's choice
        for allow_reset in (False, True):
            _, js, go = S.optimise_time(ps, walks, shares, model, allow_reset)
            js, go = np.asarray(js)[gi[te]], np.asarray(go)[gi[te]]
            k, pos = js // J, js % J
            sx, sz = spots[pos, 0], spots[pos, 1]
            walk = np.hypot(px[te] - sx, pz[te] - sz) / tm.speed
            cost = tm.reset + tm.look + go * (walk + act)
            found = np.zeros(te.sum())
            for kk in range(len(m.kinds)):
                sel = go & (k == kk)
                found[sel] = S.success_at(c.kernel(m, kk), sx[sel], sz[sel], shx[te][sel], shz[te][sel])
            seconds = cost.sum() / found.sum()
            label = name + (", reset bad worlds at once" if allow_reset else "")
            rows.append({"strategy": label, "worlds played out": f"{go.mean():.0%}",
                         "chance when you go": pct(found.sum() / max(go.sum(), 1)),
                         "avg travel": f"{(walk * go).sum() / max(go.sum(), 1):.0f}s",
                         "attempts per find": f"{len(cost) / found.sum():.1f}",
                         "seconds per find": seconds})
            res[label] = float(seconds)
    base = res["same spot for every world"]
    for r in rows:
        r["speed-up"] = f"{base / r['seconds per find']:.2f}x"
        r["seconds per find"] = f"{r['seconds per find']:.0f}"
    return rows, res


def section_time(c: Ctx, sub: np.ndarray, rules: dict) -> None:
    a = c.args
    c.add("## 5. Seconds per stronghold found",
          "",
          "Each attempt costs a reset and a look around. If you go, you also travel to the spot and dig in and "
          "xray (or dig down, for blind digging), once per world as in your runs. Spots are chosen to minimise "
          "seconds per find (a closer spot can beat a better one) on training worlds, and scored on test worlds. "
          "'Reset bad worlds at once' also decides, per group, whether a world is worth playing out.",
          f"Time model: {c.tm.describe()}, one directional xray {a.look_dir:g}s, blind dig down {a.dig_down:g}s "
          "(change with `--reset --look --dig --xray --look-dir --dig-down --speed`).",
          "")
    c.summary["time"] = {}
    for key in dict.fromkeys([a.method, "look"]):
        m = METHOD[key]
        rows, res = time_table(c, m, sub, rules[(key, a.radius)])
        c.summary["time"][key] = res
        c.add(f"### {m.label} ({c.action_seconds(m):g}s at the spot)", "", *md_table(pd.DataFrame(rows)))


# ------------------------------------------------------------------------------------------------- notes

def section_notes(c: Ctx) -> None:
    c.add("## Notes and limits",
          "",
          "* The xray models are assumptions: 'flat' ignores depth, '3D' measures from a fixed eye height, the "
          "directional look is a 90 degree wedge. Use `--method` for the one that matches what you see.",
          "* Stronghold pieces that would intersect water or lava are not removed (the game skips those pieces).",
          "* Biomes are read at 1:4 resolution; water/land at points every 32 blocks around you.",
          "* Travel time is a straight line at a fixed speed; terrain, water and what you do on the way are ignored.",
          "")


# --------------------------------------------------------------------------------------------- summary

def write_summary(reports: Path) -> None:
    rows = []
    for f in sorted(reports.glob("*/summary.json")):
        s = json.loads(f.read_text())
        b, o, rt = s.get("baseline", {}), s.get("oracle", {}), s.get("rules_test", {})
        tm = s.get("time", {})
        key = s["method"]
        best_time = min((v for per in tm.values() for v in per.values()), default=float("nan"))
        rows.append({
            "run": s["tag"], "worlds": f"{s['worlds']:,}",
            "dig": pct(b.get("dig", {}).get("p")),
            "dig, all biomes known": pct(o["dig"]["p"]) if "dig" in o else "-",
            f"{key}": pct(b.get(key, {}).get("p")),
            f"{key}, rules": pct(rt.get(f"{key}_r{s['radius']}")),
            f"{key}, all biomes known": pct(o[key]["p"]) if key in o else "-",
            "look 1 dir": pct(b.get("look", {}).get("p")),
            "look, rules": pct(rt.get(f"look_r{s['radius']}")),
            "s/find, same spot": f"{tm.get(key, {}).get('same spot for every world', float('nan')):.0f}",
            "s/find, best strategy": f"{best_time:.0f}"})
    lines = ["# Stronghold reports side by side", "",
             "Chances are for one attempt at the chosen spot. 'rules' = decision tree over what you see near spawn "
             "(scored on test worlds). 'all biomes known' = the ceiling for any biome-based method. s/find = expected "
             "seconds per stronghold found under each report's time model.", ""]
    lines += md_table(pd.DataFrame(rows)) if rows else ["(no reports yet)"]
    (reports / "summary.md").write_text("\n".join(lines))


# ----------------------------------------------------------------------------------------------- main

def main() -> None:
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("--data", required=True, help="directory with the dataset run (seeds_<tag>.csv, run_<tag>.json)")
    p.add_argument("--tag", required=True, help="small, medium, large, or e.g. small_balanced")
    p.add_argument("--out", help="report directory (default DATA/reports/TAG)")
    p.add_argument("--method", default="xray", choices=["dig", "xray", "xray3d", "look"],
                   help="main method for sections 3-5 (default xray)")
    p.add_argument("--radius", type=int, default=64, help="'close' look-around radius in blocks (default 64)")
    p.add_argument("--radius-far", type=int, default=128, help="'far' look-around radius (default 128)")
    p.add_argument("--cell", type=int, default=64, help="spawn cell size in blocks (default 64)")
    p.add_argument("--min-group", type=int, default=2000, help="min training worlds per group (default 2000)")
    p.add_argument("--depth", type=int, default=4, help="max rule depth (default 4)")
    p.add_argument("--min-leaf", type=int, default=3000, help="min training worlds per rule (default 3000)")
    p.add_argument("--min-leaf-share", type=float, default=0.01, help="... or this share of them (default 0.01)")
    p.add_argument("--min-gain", type=float, default=0.002, help="min chance gain for a split (default 0.002)")
    p.add_argument("--max-worlds", type=int, default=2_000_000,
                   help="random subset used for sections 3c-5 (memory ~1.5 GB per million; default 2,000,000)")
    p.add_argument("--test-share", type=float, default=0.3)
    p.add_argument("--kernel-tag", help="take the kernels from this run (default: own run, or the plain scale run)")
    p.add_argument("--reset", type=float, default=10.0, help="seconds from reset to control (default 10)")
    p.add_argument("--look", type=float, default=5.0, help="seconds looking around at spawn (default 5)")
    p.add_argument("--dig", type=float, default=5.0, help="seconds to dig 3 deep, ladder, sand (default 5)")
    p.add_argument("--xray", type=float, default=6.0, help="seconds for one xray looking down (default 6)")
    p.add_argument("--look-dir", type=float, default=30.0, help="seconds for one directional xray (default 30)")
    p.add_argument("--dig-down", type=float, default=30.0, help="seconds to dig down to stronghold depth (default 30)")
    p.add_argument("--speed", type=float, default=5.0, help="travel speed in blocks/s (default 5)")
    a = p.parse_args()

    t0 = time.time()
    run = S.load_run(a.data, a.tag)
    # Layouts never depend on biomes, so a run without kernels (e.g. --balanced-only --kernel-seeds 0) can borrow
    # the kernels of another run.
    kernel_run = run
    if a.kernel_tag or not run.info.get("kernel_worlds"):
        kernel_run = S.load_run(a.data, a.kernel_tag or run.info["biome_scale"])
        print(f"kernels from run '{kernel_run.tag}' ({kernel_run.info['kernel_worlds']:,} worlds)")
    out = Path(a.out) if a.out else Path(a.data) / "reports" / a.tag
    out.mkdir(parents=True, exist_ok=True)
    grid = [S.grid_column(dx, dz) for dz in run.grid_offsets for dx in run.grid_offsets]
    df = S.load_seeds(run, columns=["sample", "balanced", "player_x", "player_z", "player_biome", "sh_x", "sh_z"] + grid)
    rng = np.random.default_rng(12345)
    train = rng.random(len(df)) >= a.test_share
    needed = {(kind, m.radius) for m in METHODS for kind in m.kinds}
    kernels = {kr: S.load_kernel(kernel_run, *kr) for kr in needed}
    mappers = {kr: S.HeatMapper(k) for kr, k in kernels.items()}
    tm = S.TimeModel(a.reset, a.look, a.dig, a.xray, a.speed)
    c = Ctx(a, run, out, df, S.chunk_index(df.sh_x, df.sh_z), train, kernels, mappers, tm,
            {"kernel_worlds": kernel_run.info["kernel_worlds"], "tag": kernel_run.tag})
    c.summary.update({"tag": a.tag, "worlds": len(df), "method": a.method, "radius": a.radius,
                      "scale": run.info["biome_scale"], "balanced_only": bool(run.info.get("balanced_only"))})
    sub = np.sort(rng.choice(len(df), a.max_worlds, replace=False)) if len(df) > a.max_worlds else np.arange(len(df))

    for step in (section_setup, section_validation, section_baseline, section_oracle):
        step(c)
        print(f"[{time.time() - t0:6.1f}s] {step.__name__}")
    section_single(c, sub)
    print(f"[{time.time() - t0:6.1f}s] section_single")
    rules = section_rules(c, sub)
    print(f"[{time.time() - t0:6.1f}s] section_rules")
    section_time(c, sub, rules)
    print(f"[{time.time() - t0:6.1f}s] section_time")
    section_notes(c)

    (out / "report.md").write_text("\n".join(c.md))
    (out / "summary.json").write_text(json.dumps(c.summary, indent=2, default=float))
    write_summary(out.parent)
    print(f"wrote {out / 'report.md'} in {time.time() - t0:.0f}s")


if __name__ == "__main__":
    main()
