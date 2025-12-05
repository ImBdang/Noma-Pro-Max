#include "ota_engine.h"

/* ================================== GLOBAL VARIABLES ====================================== */
bool ota_detect = false;
volatile uint32_t ota_size;
volatile uint32_t ota_flag;
/* ========================================================================================== */


/* ================================== STATIC DECLARATIONS =================================== */
static char firmware_url[200] = "https://raw.githubusercontent.com/ImBdang/ota/main/main.bin";
//static char firmware_url[128] = "http://httpbin.org/bytes/512";
//static char firmware_url[128] = "http://opinion.people.com.cn/GB/n1/2018/0815/c1003-30228758.html";

static uint32_t offset = 0;

static bool ota_download_firmware(void);
/* ========================================================================================== */

static bool ota_download_firmware(void){
    static uint8_t step = 0;
    bool tmp = false;
    switch (step)
    {
    case 0:
        flash_erase_sector_addr(OTA_SECTOR_ADDR);
        set_ota_flag(OTA_FLAG_READY);
        step++;
        return false;
    
    case 1:
        if (http_init())
            step++;
        return false;
    case 2:
        offset = 0;
        http_data_len = 0;
        if (http_seturl(firmware_url)){
            step++;
        }
        return false;
    
    case 3:
        if (http_action(0)){
            step++;
        }
        return false;
    
    case 4:
        if (http_data_len > 0){
            if (http_read_state == HTTP_READ_BUSY)
                return false;
            
            static uint32_t actual_chunk;

            if (http_read_state == HTTP_READ_DONE) {
                uint32_t addr = OTA_SECTOR_ADDR + offset;
                http_read_ptr = http_read_buff;
                http_read_state = HTTP_READ_IDLE;
                breakp();
                flash_chunk(http_read_buff, actual_chunk, addr);
            }

            if (http_read_state == HTTP_READ_IDLE) {
                actual_chunk = (http_data_len > CHUNK_SIZE) ? CHUNK_SIZE : http_data_len;
                reading_chunk = actual_chunk;
                if (http_read(offset, actual_chunk)){
                    http_data_len -= actual_chunk;
                    offset += actual_chunk;
                }
            }
            if (http_data_len == 0)
                step++;
        }
        return false;

    case 5:
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

void ota_init(void)
{
    ota_size = *OTA_SIZE_ADDR;
    ota_flag = *OTA_FLAG_ADDR;
    if (ota_flag != OTA_FLAG_READY && ota_flag != OTA_FLAG_NONE)
    {
        ota_flag = OTA_FLAG_NONE;
    }
    if (ota_size == 0 || ota_size > (128 * 1024))
    {
        ota_size = 0;
    }
}


void set_firmware_size(uint32_t size)
{
    if (size == 0 || size > (128 * 1024))   
        return;
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
    FLASH_ProgramWord((uint32_t)OTA_SIZE_ADDR, size);
    FLASH_Lock();
}


void set_ota_flag(uint32_t flag)
{
    if (flag != OTA_FLAG_NONE && flag != OTA_FLAG_READY)
        return;
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
    FLASH_ProgramWord((uint32_t)OTA_FLAG_ADDR, flag);
    FLASH_Lock();
}
