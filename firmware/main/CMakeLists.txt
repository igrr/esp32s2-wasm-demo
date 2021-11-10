idf_component_register(SRCS "main.cpp" "usb.c" "msc_flash.c" "storage.c" "settings.cpp" "status.c" "wasm.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES wasm3 usb tinyusb wear_levelling fatfs vfs led_strip)

idf_component_get_property(tinyusb tinyusb COMPONENT_LIB)
target_link_libraries(${COMPONENT_LIB} INTERFACE $<TARGET_FILE:${tinyusb}> $<TARGET_FILE:${COMPONENT_LIB}>)
