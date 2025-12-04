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
#define OTA_FLAG_ADDR           0x0801FFF8
#define OTA_SIZE_ADDR           0x0801FFFC

extern bool ota_detect;
/* ============================================================================================ */


/* ====================================== TYPEDEF DEFINITION ================================== */
void ota_process(void);
void set_ota_detect(bool value);
/* ============================================================================================ */

#endif /* __OTA_ENGINE_H__ */