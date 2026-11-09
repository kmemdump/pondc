# pond

![Demo](demo.gif)

A simple 2D graphics engine and pond-ecosystem simulation, written in C on top of
[raylib](https://github.com/raysan5/raylib).

The simulation is a direct translation of the original Processing implementation
(`pond-processing`): an ecosystem of algae, fish, and snakes that move, sense,
eat, reproduce, and die over a clamped clock.

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

## TODO

- Merge senses with motor functionality for more consistent motion
