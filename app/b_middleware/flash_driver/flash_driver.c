#include "flash_driver.h"

/* ================================== EXTERN VARIABLES ====================================== */
/* ========================================================================================== */


/* ================================== GLOBAL VARIABLES ====================================== */
/* ========================================================================================== */


/* ================================== STATIC DECLARATIONS =================================== */
static uint32_t flash_get_sector(uint32_t addr);
/* ========================================================================================== */

static uint32_t flash_get_sector(uint32_t addr)
{
    if (addr < 0x08004000) return FLASH_Sector_0;   // 16 KB
    if (addr < 0x08008000) return FLASH_Sector_1;   // 16 KB
    if (addr < 0x0800C000) return FLASH_Sector_2;   // 16 KB
    if (addr < 0x08010000) return FLASH_Sector_3;   // 16 KB
    if (addr < 0x08020000) return FLASH_Sector_4;   // 64 KB
    /* 128 KB */
    return FLASH_Sector_5;
}


void flash_erase_sector_addr(uint32_t addr)
{
    FLASH_Status status;

    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP  | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    uint32_t sector = flash_get_sector(addr);

    status = FLASH_EraseSector(sector, VoltageRange_3); 
    (void)status;

    FLASH_Lock();
}


void flash_chunk(uint8_t *data, uint32_t len, uint32_t addr)
{
    FLASH_Status status;
    while (len % 4U) {
        data[len++] = 0xFF;
    }

    FLASH_Unlock();

    FLASH_ClearFlag(FLASH_FLAG_EOP  | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    for (uint32_t i = 0; i < len; i += 4) {
        uint32_t word =
              (uint32_t)data[i]
            | ((uint32_t)data[i + 1] << 8)
            | ((uint32_t)data[i + 2] << 16)
            | ((uint32_t)data[i + 3] << 24);

        status = FLASH_ProgramWord(addr + i, word);
        if (status != FLASH_COMPLETE) {
            breakp();
            break;
        }
    }

    FLASH_Lock();
}


