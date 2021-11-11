#include <fstream>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "wasm3.h"
#include "wasm3_cpp.h"
#include "m3_api_esp_wasi.h"
#include "common.h"

/********************************************************************************/
/***** You can define additional functions to be linked to the module here *****/

static void delay_ms(int ms)
{
    usleep(ms * 1000);
}

static void wasm_ext_init(wasm3::module &mod)
{
    /* link additional functions defined in this file */
    mod.link_optional("*", "delay_ms", delay_ms);
}

/********************************************************************************/


static std::string s_wasm_file_name;
static size_t s_wasm_env_stack_size = 8 * 1024;

class wasi_module: public wasm3::module
{
public:
    void link_wasi() {
        m3_LinkEspWASI(m_module.get());
    }
};

static void wasm_task(void* arg)
{
    std::cout << "Loading wasm file " << s_wasm_file_name.c_str() << std::endl;

    try {
        wasm3::environment env;
        wasm3::runtime runtime = env.new_runtime(s_wasm_env_stack_size);
        std::ifstream wasm_file(s_wasm_file_name.c_str(), std::ios::binary | std::ios::in);
        if (!wasm_file.is_open()) {
            throw std::runtime_error("Failed to open wasm file");
        }

        wasm3::module mod = env.parse_module(wasm_file);
        runtime.load(mod);
        ((wasi_module*) &mod)->link_wasi();  /* hack, this should be upstreamed to wasm3_cpp.h */
        wasm_ext_init(mod);

        wasm3::function start_fn = runtime.find_function("_start");
        start_fn.call();
    }
    catch(std::runtime_error &e) {
        std::cerr << "WASM3 error: " << e.what() << std::endl;
    }

    vTaskDelete(NULL);
}

extern "C" void wasm_run(const char* wasm_file_name, size_t wasm_task_stack_size, size_t wasm_env_stack_size)
{
    s_wasm_file_name = wasm_file_name;
    s_wasm_env_stack_size = wasm_env_stack_size;
    xTaskCreate(wasm_task, "wasm_task", wasm_task_stack_size, NULL, 2, NULL);
}
