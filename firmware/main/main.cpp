#include <string>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <cstring>
#include <time.h>
#include <sys/stat.h>
#include <sys/dirent.h>
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "common.h"

static const char* TAG = "main";
#define BASE_PATH "/data"

static std::string get_latest_wasm_file(void);
static void create_readme_file(void);
static void alloc_failed_hook(size_t size, uint32_t caps, const char * function_name);
static wasm_example_settings_t s_settings;
void msc_on_eject(void);
static void run_latest_wasm(void);
static void set_running_flag(void);
static void clear_running_flag(void);
static bool is_running_flag_set(void);
static TaskHandle_t s_main_task_handle;


extern "C" void app_main(void)
{
    s_main_task_handle = xTaskGetCurrentTaskHandle();
    heap_caps_register_failed_alloc_callback(&alloc_failed_hook);
    status_init();
    status_red();

    ESP_LOGI(TAG, "Initializing USB...");
    msc_allow_mount(false);
    usb_init();

    ESP_LOGI(TAG, "Initializing filesystem...");
    ESP_ERROR_CHECK( storage_init_wl() );
    ESP_ERROR_CHECK( storage_mount_fat(BASE_PATH) );
    status_green();

    create_readme_file();
    ESP_LOGI(TAG, "Loading settings...");
    ESP_ERROR_CHECK( settings_load(BASE_PATH "/settings.txt", &s_settings) );

    while (true) {
        if (is_running_flag_set()) {
            ESP_LOGI(TAG, "WASM didn't finish last time, skipping...");
            clear_running_flag();
        } else {
            ESP_LOGI(TAG, "Running WASM...");
            set_running_flag();
            run_latest_wasm();
            clear_running_flag();
        }

        ESP_LOGI(TAG, "Unmounting filesystem...");
        ESP_ERROR_CHECK( storage_unmount_fat() );

        status_blue();
        ESP_LOGI(TAG, "Waiting for USB...");
        msc_allow_mount(true);
        uint32_t notify_val = 0;
        xTaskNotifyWait(0, 1, &notify_val, portMAX_DELAY);

        status_green();
        ESP_LOGI(TAG, "Mounting filesystem...");
        ESP_ERROR_CHECK( storage_mount_fat(BASE_PATH) );
    }
}

void msc_on_eject(void)
{
    ESP_LOGI(TAG, "USB eject callback called");
    xTaskNotifyGive(s_main_task_handle);
}

static void create_readme_file(void)
{
    const char* readme_txt_name = BASE_PATH "/README.MD";
    FILE* readme_txt = fopen(readme_txt_name, "r");
    if (readme_txt == NULL) {
        ESP_LOGW(TAG, "README.MD doesn't exist yet, creating");
        readme_txt = fopen(readme_txt_name, "w");
        fprintf(readme_txt, "ESP32-S2 WASM3 demo\n");
        fprintf(readme_txt, "-------------------\n\n");
        fprintf(readme_txt, "You can save .wasm files to this mass storage device.\n");
        fprintf(readme_txt, "Eject this drive after saving the file.\n");
        fprintf(readme_txt, "The latest wasm file will be executed.\n");
        fclose(readme_txt);
    }
}

static void run_latest_wasm(void)
{
    std::string wasm_file = get_latest_wasm_file();
    if (!wasm_file.size()) {
        ESP_LOGW(TAG, "Nothing to execute");
        return;
    }
    ESP_LOGI(TAG, "Running %s", wasm_file.c_str());
    wasm_run(wasm_file.c_str(), s_settings.wasm_task_stack_size, s_settings.wasm_env_stack_size);
}


std::string get_latest_wasm_file(void)
{
    ESP_LOGI(TAG, "Files list:");
    time_t latest_mtime = 0;
    std::string latest_mtime_file;
    DIR* dir = opendir(BASE_PATH);
    while (true) {
        struct dirent* de = readdir(dir);
        if (!de) {
            break;
        }

        struct stat st = {};
        char full_name[512];
        snprintf(full_name, sizeof(full_name), BASE_PATH "/%s", de->d_name);
        stat(full_name, &st);
        time_t mtime = st.st_mtime;
        struct tm mtm;
        localtime_r(&mtime, &mtm);
        bool is_wasm = false;
        FILE* f = fopen(full_name, "rb");
        if (f) {
            char hdr[4];
            const char hdr_expected[] = {0x00, 0x61, 0x73, 0x6d};
            if (fread(hdr, 1, 4, f) == 4 && memcmp(hdr, hdr_expected, 4) == 0) {
                is_wasm = true;
            }
            fclose(f);
        }
        char* str_time = asctime(&mtm);
        str_time[strlen(str_time) - 1] = 0;
        ESP_LOGI(TAG, "File: %s mtime: %s%s", full_name, str_time, is_wasm?" [WASM]":"");
        if (is_wasm && mtime > latest_mtime) {
            latest_mtime = mtime;
            latest_mtime_file = full_name;
        }
    }
    closedir(dir);
    return latest_mtime_file;
}

static void alloc_failed_hook(size_t size, uint32_t caps, const char * function_name)
{
    ESP_LOGE(TAG, "Failed to allocate %d bytes (%s). Available %d bytes.", size, function_name, heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
}

static void set_running_flag(void)
{
    FILE* running = fopen(BASE_PATH "/.running", "w");
    fclose(running);
}

static void clear_running_flag(void)
{
    unlink(BASE_PATH "/.running");
}

static bool is_running_flag_set(void)
{
    FILE* running = fopen(BASE_PATH "/.running", "r");
    fclose(running);
    return running != NULL;
}
