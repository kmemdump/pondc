# AGENTS.md

## Current state

- 2D graphics engine + pond ecosystem simulation in C on top of raylib (6.0.0, system install at `/usr`).
- Raylib window/render loop in `src/main.c`; simulation lives in `src/pond/`.
- Current milestone focus: porting the Processing pond simulation (`../pond-processing`) into C.

## Commands

- Build (release): `make build` → `build/pond`
- Debug / ASan builds: `make debug` → `build/pond-debug`, `make dasan` → `build/pond-dasan` (or `make all` for all three)
- Run: `make run` (or `./build/pond`)
- Test smoke runs (log to `logs/`): `make debug_test`, `make dasan_test`
- Clean: `make clean`

## Build

- `pkg-config` is not installed; use explicit flags (raylib system install at `/usr`, version 6.0.0).
- Makefile uses `cc` with `-Isrc -I/usr/include` and `-lraylib -lm`; sources are listed explicitly in `SRCS` (see `Makefile`).
- Run requires a display; smoke-test with `timeout 10 ./build/pond`.

## Architecture notes

- Simulation modules (mirroring the Processing design): `pangenome`, `motor`,
  `appearance`, `organism`, `painter`, `population`, `pond`, plus `log` and `types`.
  Sensing/attraction-repulsion is handled on `Organism` (`organism_sense`,
  `organism_sensesAttract`/`Repulse` and attraction/repulsion fields), not a separate module.
- No inheritance: polymorphism is emulated via kind-tagged structs (`MotorKind`,
  `AppearanceKind`).
- `PVector` stand-ins are raylib `Vector2`; `PColor` stand-ins are raylib `Color`.
- Organism physics/velocity/acceleration live on `Motor` (`Organism` holds a `Motor *`);
  code accesses `organism->motor->displacement`/velocity/acceleration directly.
- `Pangenome` stores per-species init counts and provides `genome_create`/`genome_free`/
  `genome_initDna`, plus factories `genome_createMotor`/`genome_createAppearance` and
  `genome_diet_contains`. Per-organism `Genome` (incl. `diet`) is owned/freed with the organism.
- `Painter` keeps an internal static instance (`painter_init`,
  `painter_clearBackground`, `painter_showOrganism`/`painter_debug*`); there is no global painter.
- Lifecycle/free functions exist across modules (e.g. `organism_free`, `genome_free`,
  `pangenome_free`, `population_free`, `pond_free`, `appearance_free`).
- Shared globals (`g_clock`, `g_biomassLock`, `g_width`, `g_height`) are `extern` in
  `src/pond/types.h` and defined in `src/pond/pond.c`. `types.h` also provides `Species`,
  forward decls, and `pond_v*`/random helpers.

## Conventions

- Keep new code consistent with the existing module layout and naming.
- Build before running; iterate with `make clean && make build` (use `make dasan` when chasing memory issues).
