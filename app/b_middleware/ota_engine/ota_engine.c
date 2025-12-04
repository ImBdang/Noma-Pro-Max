#include "ota_engine.h"

/* ================================== GLOBAL VARIABLES ====================================== */
bool ota_detect = false;
/* ========================================================================================== */


/* ================================== STATIC DECLARATIONS =================================== */
//static char firmware_url[128] = "https://raw.githubusercontent.com/ImBdang/ota/main/main.bin";
static char firmware_url[128] = "http://opinion.people.com.cn/GB/n1/2018/0815/c1003-30228758.html";
static uint32_t offset = 0;

static bool ota_download_firmware(void);
/* ========================================================================================== */

static bool ota_download_firmware(void){
    static uint8_t step = 0;
    bool tmp = false;
    if (!http_is_ready()){
        if (!http_init()){
            DEBUG_PRINT("HTTP INIT IS NOT SUCCESS, TRY TO TERMINATE\r\n");
            http_term();
        }
        return false;
    }  
    switch (step)
    {
    case 0:
        if (http_seturl(firmware_url)){
            step++;
        }
        break;
    
    case 1:
        if (http_action(0)){
            step++;
            flash_erase_sector_addr(OTA_SECTOR_ADDR);
        }
        break;
    
    case 2:
        if (http_data_len > 0){
            uint32_t actual_chunk = (http_data_len > CHUNK_SIZE) ? CHUNK_SIZE : http_data_len;
            if (http_read(offset, actual_chunk)){
                flash_chunk(http_read_buff, actual_chunk, OTA_SECTOR_ADDR + offset);
                http_data_len -= actual_chunk;
                offset += actual_chunk;
                if (http_data_len == 0){
                    step = 0;
                    return true;
                }
            }
        }
        return false;;
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