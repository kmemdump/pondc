# pond

A simple 2D graphics engine and pond-ecosystem simulation, written in C on top of
[raylib](https://github.com/raysan5/raylib).

The simulation is a direct translation of the original Processing implementation
(`pond-processing`): an ecosystem of algae, fish, and snakes that move, sense,
eat, reproduce, and die over a clamped clock.

## Layout

- `src/main.c` — window, render loop, frame loop.
- `src/pond/` — simulation modules:
  - `types.h` — shared globals (clock, biomass, population size, constants) and
    `PVector`/`PColor` stand-ins built on raylib `Vector2`/`Color`.
  - `log.h/.c` — lightweight logging helpers.
  - `pangenome.h/.c` — species/genome/DNA definitions and factory methods.
  - `motor.h/.c` — movement physics (base / snake / fish / algae motors).
  - `senses.h/.c` — attraction / repulsion sensing between organisms.
  - `appearance.h/.c` — drawing shapes per species (ellipse, fish tail fin, snake).
  - `organism.h/.c` — organism state, updates, senses, eating, fission.
  - `painter.h/.c` — draw flags / colors.
  - `population.h/.c` — organism list, update loop, interactions, biomass.
  - `pond.h/.c` — top-level pond state, update orchestration, globals.

## Build & Run

```
make build   # compile to build/pond
make run     # run ./build/pond
make clean   # remove build artifacts
```

Note: `pkg-config` is not installed; the Makefile uses explicit flags
(`-Isrc -I/usr/include -lraylib -lm`). raylib is a system install at `/usr` (6.0.0).

`./build/pond` requires a display (GLFW/OpenGL); run under `timeout` if you just
want to smoke-test startup.
