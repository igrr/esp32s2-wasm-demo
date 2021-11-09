# WASI (emscripten) template for gitpod.io

To get started, fork this Github project.

Sign into gitpod.io, and create a new workspace based on the forked project.

The workspace contains Emscripten SDK installed into `$EMSDK_PATH`. If you want to use Emscripten SDK in the workspace terminal, run `. $EMSDK_PATH/emsdk_env.sh`.

Inside the workspace, two _tasks_ are defined:

* Build (emmake) — runs `emmake make all`
* Clean (emmake) — runs `emmake make clean`

You can run the build task using Ctrl-Shift-B shortcut (Cmd-Shift-B on macOS).

The build will produce `hello.wasm` file. Right-click on this file and choose "Download".

Save the WASM file into the mass storage device of your development board.

Eject the mass storage device. The board will run the latest WASM file automatically. You should see "Hello world" printed in the serial output.

