"""Shared helpers for the stronghold analysis: loading, biome categories, heat maps, features, policy tree.

Core idea (see ../README.md): the stronghold's *layout* only depends on (world seed, chunk), never on biomes.
So for any group of worlds, P(a dig/xray at column c finds the stronghold) is
    sum over start chunks s of  P(start = s | group) * K(c - 16 * s)
where the "kernel" K is measured once by the C++ tool. Every heat map here is a start-chunk histogram
convolved with a kernel; report.py checks this against heat maps accumulated directly from real layouts.
"""
from __future__ import annotations

import json
from dataclasses import dataclass, field
from pathlib import Path

import numpy as np
import pandas as pd
from scipy import fft

WORLD_HALF = 432  # classic world: blocks [-432, 431]
WORLD_SIDE = 2 * WORLD_HALF
CHUNK_HALF = WORLD_HALF // 16  # chunks [-27, 26]
CHUNK_SIDE = 2 * CHUNK_HALF

# --------------------------------------------------------------------------------------------- biomes

BIOME_NAMES = {
    0: "ocean", 1: "plains", 2: "desert", 3: "extreme_hills", 4: "forest", 5: "taiga", 6: "swampland",
    7: "river", 8: "hell", 9: "the_end", 10: "frozen_ocean", 11: "frozen_river", 12: "ice_plains",
    13: "ice_mountains", 14: "mushroom_island", 15: "mushroom_island_shore", 16: "beach", 17: "desert_hills",
    18: "forest_hills", 19: "taiga_hills", 20: "extreme_hills_edge", 21: "jungle", 22: "jungle_hills",
    23: "jungle_edge", 24: "deep_ocean", 25: "stone_beach", 26: "cold_beach", 27: "birch_forest",
    28: "birch_forest_hills", 29: "roofed_forest", 30: "cold_taiga", 31: "cold_taiga_hills", 32: "mega_taiga",
    33: "mega_taiga_hills", 34: "extreme_hills_plus_trees", 35: "savanna", 36: "savanna_plateau", 37: "mesa",
    38: "mesa_plateau_stone", 39: "mesa_plateau", 129: "sunflower_plains", 130: "desert_lakes",
    131: "gravelly_mountains", 132: "flower_forest", 133: "taiga_mutated", 134: "swamp_hills", 140: "ice_spikes",
    149: "modified_jungle", 151: "modified_jungle_edge", 155: "tall_birch_forest", 156: "tall_birch_hills",
    157: "dark_forest_hills", 158: "snowy_taiga_mountains", 160: "giant_spruce_taiga",
    161: "giant_spruce_taiga_hills", 162: "modified_gravelly_mountains", 163: "shattered_savanna",
    164: "shattered_savanna_plateau", 165: "eroded_badlands", 166: "modified_wooded_badlands_plateau",
    167: "modified_badlands_plateau", -1: "outside_world",
}

# What a runner can tell apart: hill variants merged into the base biome, ocean variants merged,
# mutated ("M") variants kept separate.
CATEGORY_MEMBERS = {
    "ocean": [0, 10, 24], "river": [7, 11], "beach": [16, 26], "stone_beach": [25],
    "plains": [1], "sunflower_plains": [129], "desert": [2, 17], "desert_lakes": [130],
    "mountains": [3, 20, 34], "gravelly_mountains": [131, 162],
    "forest": [4, 18], "flower_forest": [132], "birch_forest": [27, 28], "tall_birch": [155, 156],
    "dark_forest": [29], "dark_forest_m": [157],
    "taiga": [5, 19], "taiga_m": [133], "mega_taiga": [32, 33], "mega_spruce": [160, 161],
    "snowy_taiga": [30, 31], "snowy_taiga_m": [158], "snowy_tundra": [12, 13], "ice_spikes": [140],
    "jungle": [21, 22], "jungle_edge": [23], "modified_jungle": [149], "modified_jungle_edge": [151],
    "swamp": [6], "swamp_m": [134], "savanna": [35, 36], "shattered_savanna": [163, 164],
    "badlands": [37, 38, 39], "badlands_m": [165, 166, 167], "mushroom": [14, 15],
    "outside_world": [-1],
}
CATEGORIES = list(CATEGORY_MEMBERS)

# Coarse groups, used for the biome you are standing in.
GROUP_MEMBERS = {
    "water": ["ocean", "river"], "beach": ["beach", "stone_beach"],
    "plains": ["plains", "sunflower_plains"],
    "forest": ["forest", "flower_forest", "birch_forest", "tall_birch", "dark_forest", "dark_forest_m"],
    "taiga": ["taiga", "taiga_m", "mega_taiga", "mega_spruce"],
    "snowy": ["snowy_taiga", "snowy_taiga_m", "snowy_tundra", "ice_spikes"],
    "mountains": ["mountains", "gravelly_mountains"],
    "jungle": ["jungle", "jungle_edge", "modified_jungle", "modified_jungle_edge"],
    "swamp": ["swamp", "swamp_m"],
    "dry": ["desert", "desert_lakes", "savanna", "shattered_savanna", "badlands", "badlands_m"],
    "mushroom": ["mushroom"], "outside": ["outside_world"],
}
GROUPS = list(GROUP_MEMBERS)

# Placement::Stronghold::stronghold_biomes (mutated biomes never qualify) and Generator::SPAWN_BIOMES.
STRONGHOLD_VALID = [1, 2, 3, 4, 5, 8, 9, 12, 13, 14, 17, 18, 19, 20, 21, 22, 23, 25, 27, 28, 29, 30, 31, 32, 33,
                    34, 35, 36, 37, 38, 39]
WATER = CATEGORY_MEMBERS["ocean"] + CATEGORY_MEMBERS["river"]


def _table(mapping: dict[int, int], default: int = -1) -> np.ndarray:
    """Integer lookup table indexed by biome id + 1 (so id -1 = outside world maps to index 0)."""
    t = np.full(258, default, dtype=np.int16)
    for b, v in mapping.items():
        t[b + 1] = v
    return t


CATEGORY_INDEX = _table({b: i for i, (c, ids) in enumerate(CATEGORY_MEMBERS.items()) for b in ids})
GROUP_INDEX = _table({b: GROUPS.index(g) for g, cats in GROUP_MEMBERS.items() for c in cats
                      for b in CATEGORY_MEMBERS[c]})
IS_WATER = _table({b: 1 for b in WATER}, 0).astype(bool)
IS_VALID = _table({b: 1 for b in STRONGHOLD_VALID}, 0).astype(bool)


def biome_table(ids, table: np.ndarray) -> np.ndarray:
    return table[np.asarray(ids, dtype=np.int64) + 1]


# -------------------------------------------------------------------------------------------- loading

@dataclass
class Run:
    """One dataset run of the C++ tool (files named *_<tag>.*, e.g. tag = "small" or "small_balanced")."""
    directory: Path
    tag: str
    info: dict = field(default_factory=dict)

    @property
    def grid_offsets(self) -> list[int]:
        step, half = self.info["grid_step"], self.info["grid_half"]
        return [(i - half) * step for i in range(2 * half + 1)]

    def file(self, name: str) -> Path:
        return self.directory / name


def load_run(directory: str | Path, tag: str) -> Run:
    directory = Path(directory)
    info_path = directory / f"run_{tag}.json"
    if not info_path.exists():
        raise FileNotFoundError(f"{info_path} not found - run `EXE_StrongholdMining dataset` first")
    return Run(directory, tag, json.loads(info_path.read_text()))


def grid_column(dx: int, dz: int, prefix: str = "b") -> str:
    fmt = lambda v: f"m{-v}" if v < 0 else str(v)  # noqa: E731
    return f"{prefix}_{fmt(dx)}_{fmt(dz)}"


def seeds_parquet(run: Run, refresh: bool = False) -> Path:
    """Convert seeds_<tag>.csv to seeds_<tag>.parquet once (streamed, so 10+ GB CSVs are fine) and return it."""
    import pyarrow as pa
    import pyarrow.csv as pcsv
    import pyarrow.parquet as pq

    csv = run.file(f"seeds_{run.tag}.csv")
    parquet = run.file(f"seeds_{run.tag}.parquet")
    if parquet.exists() and not refresh and parquet.stat().st_mtime >= csv.stat().st_mtime:
        return parquet
    with open(csv) as f:
        header = f.readline().strip().split(",")
    types = {c: pa.int16() for c in header}
    types.update({"sample": pa.int64(), "seed": pa.int64(), "candidates": pa.int32(),
                  "angle": pa.float32(), "dist": pa.float32()})
    reader = pcsv.open_csv(csv, read_options=pcsv.ReadOptions(block_size=64 << 20),
                           convert_options=pcsv.ConvertOptions(column_types=types))
    tmp = parquet.with_suffix(".parquet.tmp")
    with pq.ParquetWriter(tmp, reader.schema, compression="zstd") as writer:
        for batch in reader:
            writer.write_batch(batch)
    tmp.replace(parquet)
    return parquet


def load_seeds(run: Run, columns: list[str] | None = None, refresh: bool = False) -> pd.DataFrame:
    """seeds_<tag>.csv as a DataFrame (read through the Parquet cache; pass `columns` to save memory)."""
    return pd.read_parquet(seeds_parquet(run, refresh), columns=columns)


def load_oracle(run: Run) -> pd.DataFrame | None:
    path = run.file(f"oracle_{run.tag}.csv")
    return pd.read_csv(path) if path.exists() else None


def load_kernel(run: Run, kind: str, radius: int) -> np.ndarray:
    """P(distance from the column at offset (dx, dz) to the structure <= radius), shape (401, 401), [dz, dx]."""
    radii = run.info["kernel_radii"]
    if radius not in radii:
        raise ValueError(f"radius {radius} not tabulated (have {radii})")
    counts = np.load(run.file(f"kernel_{kind}_{run.tag}.npy"), mmap_mode="r")
    return counts[radii.index(radius)].astype(np.float64) / run.info["kernel_worlds"]


def load_direct_heatmap(run: Run, name: str = "any") -> np.ndarray:
    """Heat map accumulated from the real layouts of every world (probability per column), [z, x]."""
    counts = np.load(run.file(f"heatmap_{name}_{run.tag}.npy")).astype(np.float64)
    n = run.info["balanced_worlds"] if name.endswith("balanced") else run.info["worlds"]
    return counts / max(n or 0, 1)


# ------------------------------------------------------------------------------------------ heat maps

def chunk_index(sh_x, sh_z) -> np.ndarray:
    """Flat index of the stronghold start chunk on the CHUNK_SIDE x CHUNK_SIDE chunk grid ([cz, cx])."""
    cx = np.floor_divide(np.asarray(sh_x, dtype=np.int64), 16) + CHUNK_HALF
    cz = np.floor_divide(np.asarray(sh_z, dtype=np.int64), 16) + CHUNK_HALF
    return cz * CHUNK_SIDE + cx


def chunk_histogram(chunks: np.ndarray) -> np.ndarray:
    """Share of worlds per start chunk."""
    h = np.bincount(chunks, minlength=CHUNK_SIDE * CHUNK_SIDE).astype(np.float64)
    return h / max(h.sum(), 1.0)


class HeatMapper:
    """Convolves start-chunk histograms with one kernel (FFT; the kernel's transform is cached)."""

    def __init__(self, kernel: np.ndarray):
        self.kernel = kernel
        self.half = kernel.shape[0] // 2
        self.shape = tuple(fft.next_fast_len(WORLD_SIDE + 2 * self.half, real=True) for _ in range(2))
        self._kf = fft.rfft2(kernel, s=self.shape, workers=-1)

    def __call__(self, hist: np.ndarray) -> np.ndarray:
        """hist: chunk histogram. Returns P per column, shape (864, 864), indexed [z + 432, x + 432]."""
        lattice = np.zeros((WORLD_SIDE, WORLD_SIDE))
        lattice[::16, ::16] = hist.reshape(CHUNK_SIDE, CHUNK_SIDE)  # chunk c origin = index 16 * (c + 27)
        full = fft.irfft2(fft.rfft2(lattice, s=self.shape, workers=-1) * self._kf, s=self.shape, workers=-1)
        h = self.half
        return np.clip(full[h:h + WORLD_SIDE, h:h + WORLD_SIDE], 0.0, None)


def best_point(heat: np.ndarray) -> tuple[int, int, float]:
    """(x, z, value) of the maximum of a world heat map."""
    iz, ix = np.unravel_index(int(np.argmax(heat)), heat.shape)
    return int(ix) - WORLD_HALF, int(iz) - WORLD_HALF, float(heat[iz, ix])


def kernel_value(kernel: np.ndarray, dx, dz) -> np.ndarray:
    """K(dx, dz), zero outside the tabulated window."""
    h = kernel.shape[0] // 2
    ix = np.asarray(dx, dtype=np.int64) + h
    iz = np.asarray(dz, dtype=np.int64) + h
    ix, iz = np.broadcast_arrays(ix, iz)
    ok = (ix >= 0) & (ix < kernel.shape[1]) & (iz >= 0) & (iz < kernel.shape[0])
    out = np.zeros(ix.shape)
    out[ok] = kernel[iz[ok], ix[ok]]
    return out


def success_at(kernel: np.ndarray, x, z, sh_x, sh_z) -> np.ndarray:
    """Per world: P(a dig/xray at (x, z) finds that world's stronghold), averaged over layouts. Exact in
    expectation (the layout is independent of everything a runner can see) and less noisy than one layout."""
    cx = np.floor_divide(np.asarray(sh_x, dtype=np.int64), 16) * 16
    cz = np.floor_divide(np.asarray(sh_z, dtype=np.int64), 16) * 16
    return kernel_value(kernel, np.asarray(x, dtype=np.int64) - cx, np.asarray(z, dtype=np.int64) - cz)


# ------------------------------------------------------------------------------------------- features

def offset_label(dx: int, dz: int) -> str:
    """'32N 64E' style offset from the player (north = -z, east = +x)."""
    parts = []
    if dz:
        parts.append(f"{abs(dz)}{'N' if dz < 0 else 'S'}")
    if dx:
        parts.append(f"{abs(dx)}{'W' if dx < 0 else 'E'}")
    return " ".join(parts) if parts else "here"


def sector_of(dx: int, dz: int) -> str:
    if abs(dz) >= abs(dx):
        return "north" if dz < 0 else "south"
    return "east" if dx > 0 else "west"


@dataclass
class FeatureSet:
    X: np.ndarray  # (n, F) float32
    names: list[str]
    labels: list[str]  # human readable, used in the rules
    thresholds: list[np.ndarray]  # candidate split points: value <= t goes left
    binary: list[bool]


def observable_features(df: pd.DataFrame, run: Run, radius: int, position_thresholds=None) -> FeatureSet:
    """What a runner sees in the first seconds: own coordinates (on the map), the biome group they stand in, and
    for grid points within `radius` blocks: water (ocean/river)? land a stronghold may use?  Plus the share of
    water / stronghold land per compass sector."""
    cols, names, labels, thresholds, binary = [], [], [], [], []

    def add(values, name, label, thr, is_binary):
        cols.append(np.asarray(values, dtype=np.float32))
        names.append(name)
        labels.append(label)
        thresholds.append(np.asarray(thr, dtype=np.float32))
        binary.append(is_binary)

    for axis in ("x", "z"):
        v = df[f"player_{axis}"].to_numpy()
        if position_thresholds is None:
            thr = np.unique(np.round(np.quantile(v, np.linspace(0.02, 0.98, 49))))
        else:
            thr = position_thresholds[axis]
        add(v, f"player_{axis}", f"your {axis}", thr, False)

    own = biome_table(df["player_biome"].to_numpy(), GROUP_INDEX)
    for gi, g in enumerate(GROUPS):
        add(own == gi, f"own_{g}", f"you stand in {g}", [0.5], True)

    sectors: dict[str, list] = {}
    for dz in run.grid_offsets:
        for dx in run.grid_offsets:
            if dx * dx + dz * dz > radius * radius or (dx == 0 and dz == 0):
                continue
            ids = df[grid_column(dx, dz)].to_numpy()
            water = biome_table(ids, IS_WATER)
            valid = biome_table(ids, IS_VALID)
            where = offset_label(dx, dz)
            add(water, f"water_{grid_column(dx, dz, 'at')}", f"water {where}", [0.5], True)
            add(valid, f"valid_{grid_column(dx, dz, 'at')}", f"stronghold land {where}", [0.5], True)
            sectors.setdefault(sector_of(dx, dz), []).append((water, valid))

    for s, pts in sorted(sectors.items()):
        k = len(pts)
        steps = np.arange(1, k) / k - 1e-6
        add(np.mean([p[0] for p in pts], axis=0), f"water_share_{s}", f"water share to the {s}", steps, False)
        add(np.mean([p[1] for p in pts], axis=0), f"valid_share_{s}", f"stronghold-land share to the {s}", steps,
            False)

    return FeatureSet(np.column_stack(cols), names, labels, thresholds, binary)


# --------------------------------------------------------------------------------------- spot choices

def candidate_spots(step: int = 8, lo: int = -328, hi: int = 360) -> np.ndarray:
    """(x, z) candidate dig/xray spots on a grid."""
    xs = np.arange(lo, hi + 1, step)
    gx, gz = np.meshgrid(xs, xs)
    return np.column_stack([gx.ravel(), gz.ravel()])


def spot_matrix(kernel: np.ndarray, chunks: np.ndarray, spots: np.ndarray) -> np.ndarray:
    """M[c, j] = P(spot j finds a stronghold whose start chunk is chunks[c])."""
    cx = (chunks % CHUNK_SIDE - CHUNK_HALF) * 16
    cz = (chunks // CHUNK_SIDE - CHUNK_HALF) * 16
    return kernel_value(kernel, spots[None, :, 0] - cx[:, None], spots[None, :, 1] - cz[:, None]).astype(np.float32)


@dataclass
class TimeModel:
    """Seconds per attempt. Every attempt: reset + look. If you go: walk to the spot, dig 3 deep, xray."""
    reset: float = 10.0
    look: float = 5.0
    dig: float = 5.0
    xray: float = 6.0
    speed: float = 5.0  # blocks per second while travelling

    def describe(self) -> str:
        return (f"reset {self.reset:g}s, look around {self.look:g}s, dig 3 deep + ladder + sand {self.dig:g}s, "
                f"xray {self.xray:g}s, travel {self.speed:g} blocks/s")


def optimise_time(p: list[np.ndarray], walk: list[np.ndarray], share: np.ndarray, tm: TimeModel,
                  allow_reset: bool = True) -> tuple[float, list[int], list[bool]]:
    """Minimise expected seconds per found stronghold over "go to spot j" / "reset right away" per group.

    p[g][j]: success chance of spot j for group g; walk[g][j]: travel seconds; share[g]: share of worlds.
    Expected time = E[seconds per attempt] / E[successes per attempt], a ratio, so Dinkelbach's method: with
    lam = current time per find, each group picks argmax_j lam * p - (walk + dig + xray) and goes if > 0."""
    lam = 600.0
    spots, go = [0] * len(p), [True] * len(p)
    for _ in range(100):
        cost = success = 0.0
        for g in range(len(p)):
            score = lam * p[g] - (walk[g] + tm.dig + tm.xray)
            j = int(np.argmax(score))
            spots[g], go[g] = j, (score[j] > 0) or not allow_reset
            cost += share[g] * (tm.reset + tm.look + (walk[g][j] + tm.dig + tm.xray if go[g] else 0.0))
            success += share[g] * (p[g][j] if go[g] else 0.0)
        new = cost / success if success > 0 else float("inf")
        if abs(new - lam) < 1e-9:
            break
        lam = new
    return lam, spots, go


def mean_walk(px: np.ndarray, pz: np.ndarray, spots: np.ndarray, speed: float, limit: int = 3000,
              rng: np.random.Generator | None = None) -> np.ndarray:
    """Mean travel seconds from the players (subsampled to `limit`) to every spot."""
    if len(px) > limit:
        rng = rng or np.random.default_rng(0)
        idx = rng.choice(len(px), limit, replace=False)
        px, pz = px[idx], pz[idx]
    px = px.astype(np.float32)[:, None]
    pz = pz.astype(np.float32)[:, None]
    out = np.zeros(len(spots), dtype=np.float64)
    for j0 in range(0, len(spots), 2048):
        s = spots[j0:j0 + 2048].astype(np.float32)
        out[j0:j0 + 2048] = np.sqrt((s[None, :, 0] - px) ** 2 + (s[None, :, 1] - pz) ** 2).mean(axis=0)
    return out / speed


# ----------------------------------------------------------------------------------------- policy tree

@dataclass
class TreeNode:
    rows: np.ndarray
    depth: int
    conditions: list[str]
    value: float = 0.0  # expected finds at the node's best spot (training worlds)
    spot: int = -1
    feature: int = -1
    threshold: float = 0.0
    left: "TreeNode | None" = None
    right: "TreeNode | None" = None
    leaf_id: int = -1


class PolicyTree:
    """Decision tree whose split criterion is the goal itself: how many more strongholds would we find if each
    side of the split used its own best spot? Each leaf is a rule with one spot, evaluated on held-out worlds.
    With several kernels (e.g. the four look directions) a choice is a (kernel, spot) pair."""

    def __init__(self, kernels: list[np.ndarray], max_depth: int = 4, min_leaf: int = 3000, min_gain: float = 0.003,
                 spots: np.ndarray | None = None):
        self.kernels = kernels
        self.max_depth = max_depth
        self.min_leaf = min_leaf
        self.min_gain = min_gain  # required gain in success chance for the worlds of the node being split
        self.spots = candidate_spots(16) if spots is None else spots
        self.leaves: list[TreeNode] = []
        self.root: TreeNode | None = None

    def fit(self, fs: FeatureSet, chunks: np.ndarray, rows: np.ndarray | None = None) -> "PolicyTree":
        """Grow the tree on `rows` (default: all) of fs.X / chunks; no copy of the feature matrix is made."""
        self.fs = fs
        rows = np.arange(len(chunks)) if rows is None else np.asarray(rows)
        self.used = np.unique(chunks[rows])
        self.local = np.searchsorted(self.used, chunks)  # only read at `rows`, where it is exact
        self.M = np.hstack([spot_matrix(k, self.used, self.spots) for k in self.kernels])
        self.root = self._grow(TreeNode(rows, 0, []))
        self.leaves = []
        self._collect(self.root)
        return self

    def _grow(self, node: TreeNode) -> TreeNode:
        C = len(self.used)
        rows = node.rows
        hist = np.bincount(self.local[rows], minlength=C).astype(np.float32)
        v = hist @ self.M
        node.spot = int(np.argmax(v))
        node.value = float(v[node.spot])
        if node.depth >= self.max_depth or len(rows) < 2 * self.min_leaf:
            return node

        best_gain, best = -np.inf, None
        loc = self.local[rows]
        for f, thr in enumerate(self.fs.thresholds):
            x = self.fs.X[rows, f]
            b = np.searchsorted(thr, x, side="left")  # rows with x <= thr[k] have b <= k
            per_bin = np.bincount(b * C + loc, minlength=(len(thr) + 1) * C).reshape(len(thr) + 1, C)
            L = np.cumsum(per_bin, axis=0)[:-1].astype(np.float32)
            R = hist[None, :] - L
            ok = (L.sum(axis=1) >= self.min_leaf) & (R.sum(axis=1) >= self.min_leaf)
            if not ok.any():
                continue
            L, R = L[ok], R[ok]
            gain = (L @ self.M).max(axis=1) + (R @ self.M).max(axis=1) - node.value
            k = int(np.argmax(gain))
            if gain[k] > best_gain:
                best_gain, best = float(gain[k]), (f, float(thr[np.flatnonzero(ok)[k]]))
        if best is None or best_gain < self.min_gain * len(rows):
            return node

        f, t = best
        node.feature, node.threshold = f, t
        go_left = self.fs.X[rows, f] <= t
        lab = self.fs.labels[f]
        if self.fs.binary[f]:
            lc, rc = f"not {lab}", lab
        else:
            lc, rc = f"{lab} <= {t:g}", f"{lab} > {t:g}"
        node.left = self._grow(TreeNode(rows[go_left], node.depth + 1, node.conditions + [lc]))
        node.right = self._grow(TreeNode(rows[~go_left], node.depth + 1, node.conditions + [rc]))
        return node

    def _collect(self, node: TreeNode) -> None:
        if node.left is None:
            node.leaf_id = len(self.leaves)
            self.leaves.append(node)
            return
        self._collect(node.left)
        self._collect(node.right)

    def apply(self, X: np.ndarray) -> np.ndarray:
        """Leaf id for every row of X."""
        out = np.empty(len(X), dtype=np.int64)
        stack = [(self.root, np.arange(len(X)))]
        while stack:
            node, idx = stack.pop()
            if node.left is None:
                out[idx] = node.leaf_id
                continue
            go_left = X[idx, node.feature] <= node.threshold
            stack.append((node.left, idx[go_left]))
            stack.append((node.right, idx[~go_left]))
        return out


# -------------------------------------------------------------------------------------------- plotting

HEAT_STOPS = [(0.00, "#000000"), (0.03, "#5a0000"), (0.15, "#ff0000"), (0.30, "#ff8c00"), (0.45, "#ffff00"),
              (0.60, "#00dc00"), (0.78, "#003cff"), (1.00, "#960078")]


def plot_heatmap(heat: np.ndarray, path: Path, title: str, marks=None, extent: int = 360,
                 vmax: float | None = None) -> None:
    """Save a world heat map (north up) cropped to +-extent blocks, with optional (x, z, label) marks."""
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
    from matplotlib.colors import LinearSegmentedColormap

    lo, hi = WORLD_HALF - extent, WORLD_HALF + extent
    fig, ax = plt.subplots(figsize=(6.4, 5.4), dpi=110)
    cmap = LinearSegmentedColormap.from_list("stronghold", HEAT_STOPS)
    im = ax.imshow(heat[lo:hi, lo:hi] * 100, cmap=cmap, origin="upper", extent=(-extent, extent, extent, -extent),
                   interpolation="nearest", vmin=0, vmax=None if vmax is None else vmax * 100)
    fig.colorbar(im, ax=ax, label="chance (%)")
    for x, z, text in marks or []:
        ax.plot(x, z, marker="x", color="white", markersize=9, mew=2)
        ax.annotate(text, (x, z), color="white", xytext=(6, -8), textcoords="offset points", fontsize=8)
    ax.set_xlabel("x  (east ->)")
    ax.set_ylabel("z  (south ->)")
    ax.set_title(title, fontsize=10)
    fig.tight_layout()
    fig.savefig(path)
    plt.close(fig)
