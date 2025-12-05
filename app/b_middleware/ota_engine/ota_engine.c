#include "ota_engine.h"

/* ================================== GLOBAL VARIABLES ====================================== */
bool ota_detect = true;
/* ========================================================================================== */


/* ================================== STATIC DECLARATIONS =================================== */
//static char firmware_url[200] = "https://raw.githubusercontent.com/ImBdang/ota/main/main.bin";
//static char firmware_url[128] = "http://httpbin.org/bytes/512";
static char firmware_url[128] = "http://opinion.people.com.cn/GB/n1/2018/0815/c1003-30228758.html";
static uint32_t offset = 0;

static bool ota_download_firmware(void);
/* ========================================================================================== */

static bool ota_download_firmware(void){
    static uint8_t step = 0;
    bool tmp = false;
    switch (step)
    {
    case 0:
        if (http_init())
            step++;
        return false;
    case 1:
        offset = 0;
        http_data_len = 0;
        if (http_seturl(firmware_url)){
            step++;
        }
        return false;
    
    case 2:
        if (http_action(0)){
            step++;
            // flash_erase_sector_addr(OTA_SECTOR_ADDR);
        }
        return false;
    
    case 3:
        if (http_data_len > 0){
            
            if (http_read_state == HTTP_READ_BUSY)
                return false;

            if (http_read_state == HTTP_READ_DONE) {
                uint32_t addr = OTA_SECTOR_ADDR + offset;
                http_read_ptr = http_read_buff;
                step++;
                // flash_chunk(http_read_buff, actual_chunk, addr);
            }

            if (http_read_state == HTTP_READ_IDLE) {
                uint32_t actual_chunk = (http_data_len > CHUNK_SIZE) ? CHUNK_SIZE : http_data_len;
                reading_chunk = actual_chunk;
                if (http_read(offset, actual_chunk)){
                    http_data_len -= actual_chunk;
                    offset += actual_chunk;
                    step++;
                }
            }
        }
        return false;
    
    // case 4:
    //     if (http_data_len > 0){
    //         if (reading_chunk == 0)
    //             step--;
    //         return false;
    //     }
    //     else if (http_data_len == 0)
    //         step++;
    //     return false;

    case 4:
        if (http_term()){      
            step = 0;
            offset = 0;
            return true;
        }
        return false; 
    }
    return false; 
}

void set_ota_detect(bool value){
    ota_detect = value;
}

void ota_process(void){

    if (!ota_detect)
        return;
    static uint8_t step = 0;
    switch (step)
    {
    case 0:
        if (ota_download_firmware()){
            step++;
        }
        break;
    
    case 1:
        break;
    }
}