# ESP32-S2 WebAssembly Demo

This project will demonstrate WebAssembly code running on ESP32-S2. There are two parts in this project:

* ESP-IDF application for the ESP32-S2, inside [firmware](firmware/) directory.

  It contains [WASM3](https://github.com/wasm3/wasm3) WebAssembly interpreter, mass storage device emulation and some glue code to make everything work.

* C application inside [wasm](wasm/) directory, which is compiled to WebAssembly using [Emscripten](https://emscripten.org/docs/introducing_emscripten/about_emscripten.html) compiler.

## Demo overview

The demo has the following steps:

1. Set up ESP32-S2 hardware.
2. Build the firmware for the ESP32-S2 and flash it to the board.
3. A USB drive (mass storage device) emulated by ESP32-S2 will appear in the system.
4. Build the WebAssembly application.
5. Copy the WebAssembly application to USB drive.
6. Eject the USB drive.
7. The firmware will load and execute WebAssembly module, then go back to step 3.

## Getting started with Gitpod workspace

To get started, create a project from this template by clicking "Use this template" on Github.

Sign into [gitpod.io](https://gitpod.io/) and create a new workspace based on the project you have created.

The workspace contains the project itself, ESP-IDF and Emscripten SDK.

Inside the workspace, three _tasks_ are defined:

* Build (firmware) — builds firmware (application for ESP32-S2)
* Build (wasm) — builds WebAssmebly module
* Clean (wasm) — cleans WebAssembly module output

## Step by step

### Step 1: set up hardware

1. Connect a USB cable to an ESP32-S2 development board: GPIO19: white, GPIO20: green, GND: black. Note: you need an ESP32-S2 development board with PSRAM!
2. Plug USB cable into the PC, while holding "BOOT" button.
3. Check Device Manager (Windows), lsusb (Linux), System Information (macOS) and verify that an ESP32-S2 device is in the list.

### Step 2: build and flash the firmware

1. Press F1, select "Tasks: Run task", choose "Build (firmware)" task. This will build the application for ESP32-S2.
2. DFU binary will be located in [firmware/build/dfu.bin](firmware/build/dfu.bin). Right-click this file and choose "Download". Save it somewhere on your computer.
3. Install dfu-util if you don't have it yet. On Linux and macOS it can be installed using the package managers. On Windows it is installed together with IDF.
4. Reboot the development board into download mode: press and hold Boot button, click Reset button, release Boot.
5. Flash the board with dfu-util: `dfu-util -D dfu.bin`.
6. Reset the board by pressing Reset button.

### Step 3: ESP32-S2 USB drive will appear in the system

This USB mass storage device is emulated by the application flashed in the previous step. The volume name should be `NO NAME`.

At the same time, LED on the ESP32-S2 board will turn blue.

### Step 4: build WebAssembly module

Press F1, select "Tasks: Run task", choose "Build (wasm)" task. This will build the webassembly module.

### Step 5: copy WebAssembly module to ESP32-S2 USB drive

Locate [wasm/hello.wasm](wasm/hello.wasm), right-click, choose "Download", select location in the root directory of the USB drive.

### Step 6: eject the USB drive

Eject the USB device from the computer (using Explorer, Finder, etc.)

### Step 7: the firmware will execute the WebAssembly module

The firmware will select the latest wasm file and try to run it.

The LED will turn green when the execution starts.

Output from WebAssembly module will go to UART console of the ESP32-S2. Use some serial monitor to see the output, for example `python3 -m serial.tools.miniterm /dev/ttyUSB0 115200`. The example program [wasm/hello.c](wasm/hello.c) will print "Hello world" to the console.

When the module finishes executing, the USB mass storage device will appear in the system again, going back to step 3.

Note, if the WebAssembly interpreter crashes and the chip resets, it will go into USB disk mode and let you upload a new program.

## Next steps

Webassmebly module is located in [wasm/hello.c](wasm/hello.c). It can call functions exported from C by the firmware. The exported functions are defined in [firmware/main/wasm.cpp](firmware/main/wasm.cpp). See `delay_ms` function definition and `mod.link_optional` calls for an example.

There are also _a few_ WASI functions defined in [m3_api_esp_wasi.c](firmware/components/wasm3/wasm3/platforms/embedded/esp32-idf-wasi/main/m3_api_esp_wasi.c).

The development board features an LED. Can you make the LED blink or change colors from WebAssembly?

There is a `void status_rgb(int r, int g, int b)` function that you can use, arguments `r`, `g`, `b` can be in [0, 255] range.

You can also look at the list of WASI functions implemented in [m3_api_esp_wasi.c](firmware/components/wasm3/wasm3/platforms/embedded/esp32-idf-wasi/main/m3_api_esp_wasi.c) and try to implement some more. For example, can you make your WebAssembly module write to a file? On the ESP32-S2, the filesystem is mounted to the `/data` directory.

