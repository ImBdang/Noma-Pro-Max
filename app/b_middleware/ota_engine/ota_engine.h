#ifndef __OTA_ENGINE_H__
#define __OTA_ENGINE_H__

/* ====================================== INCLUDE HEADER ====================================== */
#include "stdint.h"
#include "stdbool.h"
#include "http_engine.h"
#include "flash_driver.h"
/* ============================================================================================ */

/* ====================================== DEFINTIONS ========================================== */
#define CHUNK_SIZE              512
#define OTA_SECTOR_ADDR         0x08020000 

#define OTA_SIZE_ADDR           ((uint32_t *)0x0801FFF8)
#define OTA_FLAG_ADDR           ((uint32_t *)0x0801FFFC)

#define OTA_FLAG_NONE           0xFFFFFFFF
#define OTA_FLAG_READY          0xA5A5A5A5

extern bool ota_detect;
extern volatile uint32_t ota_size;
extern volatile uint32_t ota_flag;
/* ============================================================================================ */


/* ====================================== TYPEDEF DEFINITION ================================== */
void ota_process(void);
void set_ota_flag(uint32_t flag);
void set_firmware_size(uint32_t size);
void set_ota_detect(bool value);
void ota_init(void);
/* ============================================================================================ */

#endif /* __OTA_ENGINE_H__ */