set(APP_SOURCES "wasm3_extras.cpp")

idf_component_register(SRCS ${APP_SOURCES}
                       LDFRAGMENTS linker.lf)

idf_build_get_property(build_dir BUILD_DIR)
add_subdirectory(wasm3/source)
add_subdirectory(wasm3/platforms/cpp/wasm3_cpp)

target_sources(${COMPONENT_TARGET} PRIVATE wasm3/platforms/embedded/esp32-idf-wasi/main/m3_api_esp_wasi.c)
target_include_directories(${COMPONENT_TARGET} PUBLIC wasm3/platforms/embedded/esp32-idf-wasi/main)

target_link_libraries(${COMPONENT_TARGET} PUBLIC m3 wasm3_cpp)

target_compile_options(m3 PUBLIC -DM3_IN_IRAM -DESP32 -O3 -freorder-blocks)
