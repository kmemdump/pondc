# AGENTS.md

## Current state

- 2D graphics engine + pond ecosystem simulation in C on top of raylib (6.0.0, system install at `/usr`).
- Initial raylib window/render loop in `src/main.c`; simulation lives in `src/pond/`.
- Current milestone focus: porting the Processing pond simulation (`../pond-processing`) into C.

## Commands

- Build: `make build`
- Run: `make run` (or `./build/pond`)
- Clean: `make clean`

## Build

- `pkg-config` is not installed; use explicit flags (raylib system install at `/usr`, version 6.0.0).
- Compile line: `gcc -Isrc -I/usr/include src/main.c src/pond/*.c -o build/pond -lraylib -lm`
- Run requires a display; smoke-test with `timeout 10 ./build/pond`.

## Architecture notes

- Simulation modules mirror the Processing classes: `pangenome`, `motor`, 
  `appearance`, `organism`, `painter`, `population`, `pond` (+ `log`, `types`).
- No inheritance: polymorphism is emulated via kind-tagged structs (`MotorKind`,
  `AppearanceKind`).
- `PVector` stand-ins are raylib `Vector2`; `PColor` stand-ins are raylib `Color`.
- Organism physics/velocity/acceleration live on `Motor` and are reached through
  accessors (`organism_displacement`, `organism_setDisplacement`, ...) defined in
  `organism.h` — never access `Organism` members directly.
- Sketch-level globals (`g_background`, `g_clock`, `g_width`, `g_height`, ...)
  are externs declared in `src/pond/types.h` and defined in `src/pond/pond.c`.

## Conventions

- Keep new code consistent with the existing module layout and naming.
- Build before running; iterate with `make clean && make build`.
