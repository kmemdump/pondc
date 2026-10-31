# AGENTS.md

## Current state

- Early-stage scaffold: only an empty `README.md` and a single initial commit (`cae6cae`).
- Initial raylib implementation in place: `src/main.c` (window + render loop + camera + drawing primitives); binary at `build/pond`.
- No tests, CI, or lint/format/typecheck configuration yet.

## Commands

- Build: `make build`
- Run: `make run` (or `./build/pond`)
- Clean: `make clean`

## Build

- Current implementation: `src/main.c` — minimal raylib window + render loop + camera + drawing primitives; compiled binary at `build/pond`.
- Build: `make build`
- Note: the `pkg-config` binary is not installed; use explicit flags (raylib is a system install at `/usr`, version 6.0.0).
- Run: `./build/pond` (requires a display; verified initializing GLFW/OpenGL successfully).
