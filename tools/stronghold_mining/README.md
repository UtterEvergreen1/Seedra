# Stronghold mining

Where is the stronghold in a random classic world, given what you can see in the first seconds after spawning?
This folder answers that as a data-mining pipeline for Xbox One (Elytra TU), for each biome scale:

1. **`EXE_StrongholdMining`** (C++): samples worlds the way the game creates them and writes one CSV row per world,
   plus a few NumPy arrays.
2. **`analysis/report.py`** (Python): mines those files and writes one report per run, plus a side-by-side summary.

## Build

Needs a C++23 compiler with `<format>`: GCC 13+, Clang 17+ or Visual Studio 2022. GCC 11/12 do not work.

```sh
git submodule update --init --recursive
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target EXE_StrongholdMining
```

Always build in Release; Debug is 10-50x slower.

## Run the study

```sh
EXE=build/EXE_StrongholdMining        # build/Release/EXE_StrongholdMining.exe with Visual Studio

$EXE selftest --scale small           # cross-checks every shortcut against Seedra, must print "0 failed"

OUT=stronghold_data                   # git-ignored at the repo root
for s in small medium large; do
  $EXE dataset --scale $s --seeds 10000000 --out $OUT               # "Find Balanced Seed" OFF
  $EXE oracle  --scale $s --seeds 20000 --out $OUT                  # ceiling, needs the dataset's kernels
  $EXE dataset --scale $s --balanced-only --seeds 200000 --kernel-seeds 0 --out $OUT   # balanced ON
  $EXE oracle  --scale $s --balanced-only --seeds 5000 --kernel-tag $s --out $OUT
done
```

(PowerShell: `foreach ($s in "small","medium","large") { ... }` with the same commands.)

On 8 threads (a sandbox, not a fast desktop):

| run | speed | 10M worlds / example size |
|---|---|---|
| `dataset` | ~1,000 worlds/s for the first 250k (kernels), then ~3,500-5,000/s | ~45 min, 3.1 GB CSV per scale |
| `dataset --balanced-only` | ~70 worlds/s (only 1 in ~475 seeds is balanced) | 200k worlds in ~50 min |
| `oracle` | ~100 worlds/s | 20k worlds in ~4 min |

Time scales linearly with cores. The same `--run-seed` and sample indices always give the same worlds. Use
`--first-sample` (or another `--run-seed`) for a different batch. Each report reads one run.

Then the reports (Python 3.10+):

```sh
cd tools/stronghold_mining/analysis
python -m venv .venv && . .venv/bin/activate      # Windows: .venv\Scripts\activate
pip install -r requirements.txt
for t in small medium large small_balanced medium_balanced large_balanced; do
  python report.py --data ../../../stronghold_data --tag $t
done
```

Output goes to `stronghold_data/reports/<tag>/report.md`, with its images and CSV tables in the same folder, and
to `stronghold_data/reports/summary.md` (all runs side by side). The first report of a run converts the CSV to
Parquet once. A 1M-world report takes about 35 s; sections 3c-5 use a random 2M-world subset (`--max-worlds`, about
1.5 GB RAM per million).

`python report.py -h` lists the options: the main method, how far you look around, the time model (reset, look,
dig, xray, travel speed), rule depth and minimum group sizes.

### Multi-step plans

What to do after the first xray fails (look in another direction, walk to a second spot, or reset) needs the joint
outcome of several xrays within one layout, which the kernels do not have. `layouts` mode stores it, and
`multistep.py` turns it into plans:

```sh
for s in small medium large; do
  $EXE layouts --scale $s --seeds 250000 --out $OUT      # ~5 min each, 650 MB; must print "0 mismatches"
done
cd tools/stronghold_mining/analysis
for t in small medium large small_balanced medium_balanced large_balanced; do
  python multistep.py --data ../../../stronghold_data --tag $t     # 10M worlds: ~5 min, ~7 GB RAM
done
```

It writes `reports/<tag>/multistep.md`, one CSV of plans per spawn cell per variant, and adds two columns to
`summary.md`. Each plan is spelled out ("go to (16, 136), dig in, xray down; if nothing: look north; ...; then
reset", or "reset at once"). The plan families:

- one xray down;
- xray down, and if nothing, a second xray down at another spot;
- any sequence of xrays at one spot: down and/or the 4 looks, in the best order, stopping at the first hit;
- the best of the above, or one xray at spot A and then one at spot B (any two methods).

Each family is scored with the same plan for everyone and with a plan per 64-block spawn cell (including "reset at
once"). Both xray-down models (flat, 3D) are reported. `--budget S` drops finds later than S seconds after spawning;
the time model options are the same as report.py's.

How: for a group of worlds, P(A fails, B hits) = sum over start chunks s of P(s | group) * P_layout(B - 16 s hit,
A - 16 s miss). The layout term is one Gram matrix over the 250k layouts (only cells that are ever visible). Plans
minimise seconds per find on the training worlds (Dinkelbach over a grid of break-even values) and are scored on the
test worlds. Test worlds whose own layout is in the layouts file are also simulated with it; the report prints that
check. Spots are on an 8-block grid.

## How it works

**Sampling like the game.** A random world seed is `nextLong()` of a `java.util.Random` seeded from the performance
counter. With Find Balanced Seed the game keeps calling `nextLong()` until `BalancedSeed::getIsMatch` passes. That
test always uses SMALL biomes, while the world itself uses the chosen scale. `--balanced-only` does exactly this
(with a cheap mushroom pre-filter that `selftest` proves never rejects a balanced seed). Plain runs record a
`balanced` flag instead.

**Per world** the tool records:

- The final stronghold position. The placement is copied from `Placement::Stronghold::getWorldPositions` with the RNG
  made injectable, and re-checked against Seedra for every world.
- The full layout.
- The spawn (`Generator::getSpawnBlock`), and the player position: spawn +-16 per axis, as in game.
- A 9 x 9 grid of biomes every 32 blocks around the player.

**Kernels.** A stronghold's layout depends only on (world seed, chunk), never on biomes. So for any group of worlds:

    P(xray at column c finds it | group) = sum over start chunks s of P(start = s | group) * K(c - 16 s)

`K(dx, dz)` is the chance that something lies within R blocks of the column at offset (dx, dz) from the start chunk.
The tool measures it once with exact distance transforms, for several radii and kinds:

- horizontal distance to any piece;
- 3D distance from an eye at y = 62;
- distance to the portal room;
- a 90 degree wedge per look direction.

Every heat map in the reports is therefore a start-chunk histogram convolved with a kernel (FFT, milliseconds).
Section 0 of each report compares this with a heat map accumulated from every world's real layout.

**Ceiling (`oracle`).** For each world the tool keeps the real biome map and re-runs the placement with thousands
of random RNG streams. That gives where the stronghold could be on exactly this map. The best spot is picked on half
the streams and scored on the other half, so the estimate is not optimistic. No biome-based rule can beat it, so it
tells you how much is left to find.

**Mining** (`report.py`):

- Worlds are split 70/30. Every spot and every rule is chosen on the 70% and scored on the 30%, so noise cannot
  pass as a pattern.
- *Group-bys*: one conditional heat map per spawn cell, biome group and water direction. This is the classic "slice
  and compare" step.
- *Policy tree*: a decision tree with a custom split criterion, the gain in expected finds when each side gets its own
  best spot (optionally with a look direction). A standard classification tree optimises purity instead, which is not
  the goal here.
  - Minimum leaf size and minimum gain keep the rules short and memorisable.
  - Features are only things a runner can see: map coordinates, the biome group you stand in, and water or
    stronghold-land at grid points plus their share per compass direction, within 64 blocks ("close") or 128
    ("far").
- *Time*: seconds per stronghold found = E[seconds per attempt] / E[finds per attempt]. This is a ratio, so spots
  and reset-immediately decisions are optimised with Dinkelbach's method. That way a closer spot can beat a better
  one.

## Files

`seeds_<tag>.csv`, one row per world (`<tag>` = scale, or `<scale>_balanced`):

| column | meaning |
|---|---|
| `sample`, `seed` | sample index (with `--run-seed` it identifies the world) and world seed |
| `balanced`, `candidates` | passes Find Balanced Seed (1/0, -1 = not checked); `nextLong()` calls needed |
| `spawn_x/z`, `player_x/z` | world spawn point; where the player appears (spawn +-16) |
| `spawn_biome`, `player_biome` | biome ids (1:4) at those points |
| `sh_x/z` | stronghold position = start chunk * 16 + 4 |
| `target_x/z`, `angle`, `mult`, `dist` | placement before the biome snap: ring target, angle (radians), multiplier 3-6, distance (chunks) |
| `attempts`, `found` | placement attempts (1 = first try); valid picks in the last snap (0 = no valid biome) |
| `spawn_valid_cells`, `snap_valid_cells` | valid cells in the spawn search square / in the stronghold snap square |
| `portal_x/z/y` | portal room centre and floor height |
| `sh_top_y`, `sh_bottom_y`, `pieces`, `footprint` | stronghold height range, piece count, x/z columns covered |
| `b_<dx>_<dz>` | biome id at player + (dx, dz), e.g. `b_m32_64` = 32 west, 64 south; -1 = outside the world |

Other outputs:

| file | contents |
|---|---|
| `kernel_<kind>_<tag>.npy` | uint32 counts `[radius, dz + 200, dx + 200]`, radii and world count in `run_<tag>.json`; kinds `any_h`, `any_3d`, `portal_h`, `dir_n/e/s/w` |
| `heatmap_<name>_<tag>.npy` | uint32 counts per column `[z + 432, x + 432]` from real layouts: `any`, `any_balanced`, `portal`, `start` |
| `run_<tag>.json` | settings, timing, balanced count, placement retries, checks |
| `oracle_<tag>.csv` | per world and target: best spot, cross-fitted chance, whether that spot hit the real stronghold |
| `layouts_<scale>.npy` | uint8 `[world, dz, dx]`, offsets -200..200 step 8 from the start chunk origin; bits dig, down_flat, down_3d, portal30, look_n/e/s/w (settings in `layouts_run_<scale>.json`) |
| `layouts_index_<scale>.npy` | int32 `[world, (sample, chunk x, chunk z)]`, same samples as the dataset run |

Biome ids are Seedra's `biome_t` values; `analysis/shlib.py` has the names and the groups used in the reports.

## Assumptions and limits

- **Xray geometry** is modelled three ways: flat 30 blocks, 3D 30 blocks from eye height 62, and a 90 degree wedge
  of 100 blocks per direction. The real view is somewhere in between.
- Pieces that would intersect water or lava are kept; the game removes them.
- Biomes are read at 1:4 resolution. The runner is modelled as telling water from land, and land a stronghold can
  use from land it cannot (swamps and mutated biomes are not valid).
- The spawn uses `Generator::getSpawnBlock`, which can rarely be wrong in wooded badlands plateau.
- Travel is a straight line at a fixed speed.
- A time limit on Find Balanced Seed, if the game has one, is not modelled.
