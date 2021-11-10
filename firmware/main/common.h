#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LED_GPIO 18

esp_err_t storage_init_wl(void);
esp_err_t storage_mount_fat(const char* base_path);
size_t storage_get_size(void);
size_t storage_get_sector_size(void);
esp_err_t storage_unmount_fat(void);
esp_err_t storage_read_sector(size_t addr, size_t size, void* dest);
esp_err_t storage_write_sector(size_t addr, size_t size, const void* src);

void status_init(void);
void status_red(void);
void status_green(void);
void status_blue(void);
void status_rgb(int r, int g, int b);

typedef struct {
    size_t wasm_task_stack_size;
    size_t wasm_env_stack_size;
} wasm_example_settings_t;

esp_err_t settings_load(const char* filename, wasm_example_settings_t* out_settings);

void msc_allow_mount(bool allow);
void msc_on_eject(void);

void usb_init(void);

void wasm_run(const char* wasm_file_name, size_t wasm_task_stack_size, size_t wasm_env_stack_size);

#ifdef __cplusplus
}
#endif
