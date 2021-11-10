#include <stdio.h>
#include <string.h>
#include "common.h"

static void handle_settings_line(wasm_example_settings_t* settings, const char* first, const char* second);
static void create_default_settings_file(const char* filename, const wasm_example_settings_t* settings);

esp_err_t settings_load(const char* filename, wasm_example_settings_t* out_settings)
{
    *out_settings = {};
    out_settings->wasm_task_stack_size = 32 * 1024;
    out_settings->wasm_env_stack_size = 8 * 1024;

    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        create_default_settings_file(filename, out_settings);
        return ESP_OK;
    }

    char line[100];
    while (fgets(line, sizeof(line), f) != NULL) {
        if (line[0] == '#') {
            continue;
        }
        char* eq_pos = strchr(line, '=');
        if (eq_pos == NULL) {
            continue;
        }
        *eq_pos = 0;
        const char* first = line;
        const char* second = eq_pos + 1;
        handle_settings_line(out_settings, first, second);
    }
    fclose(f);
    return ESP_OK;
}

static void handle_settings_line(wasm_example_settings_t* settings, const char* first, const char* second)
{
    if (strcmp(first, "wasm_task_stack_size") == 0) {
        settings->wasm_task_stack_size = (size_t) strtol(second, NULL, 0);
    } else if (strcmp(first, "wasm_env_stack_size") == 0) {
        settings->wasm_env_stack_size = (size_t) strtol(second, NULL, 0);
    }
}

static void create_default_settings_file(const char* filename, const wasm_example_settings_t* settings)
{
    FILE* f = fopen(filename, "w");
    fprintf(f, "# stack size for wasm task\nwasm_task_stack_size=%d\n", settings->wasm_task_stack_size);
    fprintf(f, "# wasm interpreter stack size\nwasm_env_stack_size=%d\n", settings->wasm_env_stack_size);
    fclose(f);
}
