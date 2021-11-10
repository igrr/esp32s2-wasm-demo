// Copyright 2020-2021 Espressif Systems (Shanghai) CO LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


// The mass storage class creates a mountable USB device into which files can be dropped.
// The access to the underlying block device is provided by functions in storage.c.
// The module contains the following callbacks from the tinyusb USB stack.
// - tud_msc_inquiry_cb - returns string identifiers about the device.
// - tud_msc_test_unit_ready_cb - return the availability of the device. It is available in the beginning and while it
//   is mounted. It becomes unavailable after ejecting the device.
// - tud_msc_capacity_cb - returns the device capacity.
// - tud_msc_start_stop_cb - handles disc ejection.
// - tud_msc_scsi_cb - desired actions to SCSI disc commands can be handler there.
// - tud_msc_read10_cb - invoked in order to read from the disc.
// - tud_msc_write10_cb - invoked in order to write the disc.

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/param.h>
#include "esp_err.h"
#include "esp_log.h"
#include "common.h"
#include "tusb.h"

static const char *TAG = "msc";
static bool s_allow_mount = true;

#define CONFIG_BRIDGE_MSC_VOLUME_LABEL "WASM3"

void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
    (void) lun;

    const char vid[8] = "ESP";
    const char pid[16] = "Flash Storage";
    const char rev[4] = "0.1";

    ESP_LOGI(TAG, "tud_msc_inquiry_cb() invoked");

    memcpy(vendor_id, vid, strlen(vid));
    memcpy(product_id, pid, strlen(pid));
    memcpy(product_rev, rev, strlen(rev));
}

bool tud_msc_test_unit_ready_cb(uint8_t lun)
{
    (void) lun;

    ESP_LOGD(TAG, "tud_msc_test_unit_ready_cb() invoked");

    if (!s_allow_mount) {
        tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3a, 0x00);
        return false;
    }

    return true;
}

void tud_msc_capacity_cb(uint8_t lun, uint32_t *block_count, uint16_t *block_size)
{
    (void) lun;

    ESP_LOGI(TAG, "tud_msc_capacity_cb() invoked");
    size_t size = storage_get_size();
    size_t sec_size = storage_get_sector_size();
    *block_count = size / sec_size;
    *block_size  = sec_size;
}

bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject)
{
    (void) lun;
    (void) power_condition;

    ESP_LOGI(TAG, "tud_msc_start_stop_cb() invoked, power_condition=%d, start=%d, load_eject=%d", power_condition,
             start, load_eject);

    if (load_eject) {
        if (start) {
            ESP_LOGI(TAG, "MSC START");
        } else {
            ESP_LOGI(TAG, "MSC EJECT");
            s_allow_mount = false;
            msc_on_eject();
        }
    }

    return true;
}

int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize)
{
    ESP_LOGD(TAG, "tud_msc_read10_cb() invoked, lun=%d, lba=%d, offset=%d, bufsize=%d", lun, lba, offset, bufsize);

    size_t addr = lba * storage_get_sector_size() + offset;
    esp_err_t err = storage_read_sector(addr, bufsize, buffer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "storage_read_sector failed: 0x%x", err);
        return 0;
    }
    return bufsize;
}

int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize)
{
    ESP_LOGD(TAG, "tud_msc_write10_cb() invoked, lun=%d, lba=%d, offset=%d", lun, lba, offset);

    size_t addr = lba * storage_get_sector_size() + offset;
    esp_err_t err = storage_write_sector(addr, bufsize, buffer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "storage_write_sector failed: 0x%x", err);
        return 0;
    }
    return bufsize;

}

int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16], void *buffer, uint16_t bufsize)
{
    int32_t ret;

    ESP_LOGD(TAG, "tud_msc_scsi_cb() invoked. bufsize=%d", bufsize);
    ESP_LOG_BUFFER_HEXDUMP("scsi_cmd", scsi_cmd, 16, ESP_LOG_DEBUG);

    switch (scsi_cmd[0]) {
    case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
        ESP_LOGI(TAG, "tud_msc_scsi_cb() invoked: SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL");
        ret = 0;
        break;

    default:
        ESP_LOGW(TAG, "tud_msc_scsi_cb() invoked: %d", scsi_cmd[0]);

        tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);

        ret = -1;
        break;
    }

    return ret;
}

void msc_allow_mount(bool allow)
{
    s_allow_mount = allow;
}
