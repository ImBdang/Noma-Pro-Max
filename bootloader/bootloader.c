#include "stm32f4xx_rcc.h"
#include "stm32f4xx_flash.h"
#include "stm32f4xx_syscfg.h"
#include "misc.h"

/* =======================================================
 * FLASH LAYOUT FOR STM32F401CCU6
 * =======================================================
 * Sector 0 : 0x08000000 - 0x08003FFF  (16 KB) → Bootloader
 * Sector 1 : 0x08004000 - 0x08007FFF  (16 KB) → App
 * Sector 2 : 0x08008000 - 0x0800BFFF  (16 KB)
 * Sector 3 : 0x0800C000 - 0x0800FFFF  (16 KB)
 * Sector 4 : 0x08010000 - 0x0801FFFF  (64 KB)
 * Sector 5 : 0x08020000 - 0x0803FFFF  (128 KB) → OTA image
 * =======================================================
 */

#define APP_START_ADDR      0x08004000U     /* App entry → Sector 1 */
#define OTA_SECTOR_ADDR     0x08020000U     /* Firmware downloaded here */

#define OTA_SIZE_ADDR       0x0801FFF8U     /* last 8 bytes of Sector 4 */
#define OTA_FLAG_ADDR       0x0801FFFCU
#define OTA_FLAG_VALUE      0xDEADBEEFU

#define APP_MAX_SIZE        (112 * 1024U)   /* Sector 1–4 = 16+16+16+64 = 112KB */

#define RAM_START   0x20000000U
#define RAM_END     0x20010000U     /* 64 KB */

typedef void (*app_entry_t)(void);

/* =======================================================
 * Check if application is valid
 * ======================================================= */
static uint8_t app_is_valid(void)
{
    uint32_t msp = *(uint32_t*)APP_START_ADDR;
    uint32_t reset = *(uint32_t*)(APP_START_ADDR + 4);

    /* MSP must be inside RAM */
    if (msp < RAM_START || msp > RAM_END)
        return 0;

    /* Reset handler must be inside Flash app area */
    if (reset < APP_START_ADDR || reset > 0x0801FFFF)
        return 0;

    return 1;
}

/* =======================================================
 * Jump to application
 * ======================================================= */
static void jump_to_app(void)
{
    uint32_t msp = *(uint32_t*)APP_START_ADDR;
    uint32_t reset = *(uint32_t*)(APP_START_ADDR + 4);
    app_entry_t app = (app_entry_t) reset;

    __disable_irq();

    /* Set vector table to app */
    SCB->VTOR = APP_START_ADDR;

    __set_MSP(msp);

    app();
}

/* =======================================================
 * Erase APP sectors (1 → 4)
 * ======================================================= */
static void erase_app_area(void)
{
    FLASH_EraseSector(FLASH_Sector_1, VoltageRange_3);
    FLASH_EraseSector(FLASH_Sector_2, VoltageRange_3);
    FLASH_EraseSector(FLASH_Sector_3, VoltageRange_3);
    FLASH_EraseSector(FLASH_Sector_4, VoltageRange_3);
}

/* =======================================================
 * Copy OTA to APP flash region
 * ======================================================= */
static void apply_ota(uint32_t fw_size)
{
    if (fw_size == 0 || fw_size == 0xFFFFFFFF)
        return;

    if (fw_size > APP_MAX_SIZE)
        return;

    FLASH_Unlock();

    erase_app_area();

    uint32_t src = OTA_SECTOR_ADDR;
    uint32_t dst = APP_START_ADDR;
    uint32_t end = APP_START_ADDR + fw_size;

    while (dst < end)
    {
        uint32_t data = *(uint32_t*)src;

        FLASH_Status st = FLASH_ProgramWord(dst, data);
        if (st != FLASH_COMPLETE)
            break;

        src += 4;
        dst += 4;
    }

    /* Clear OTA flag */
    FLASH_ProgramWord(OTA_FLAG_ADDR, 0xFFFFFFFF);
    FLASH_ProgramWord(OTA_SIZE_ADDR, 0xFFFFFFFF);

    FLASH_Lock();
}

/* =======================================================
 * Minimal HSI clock init (SPL-style, not HAL)
 * ======================================================= */
static void clock_init(void)
{
    RCC_DeInit();
    RCC_HSICmd(ENABLE);
    while (!RCC_GetFlagStatus(RCC_FLAG_HSIRDY));
}

/* =======================================================
 * BOOTLOADER ENTRY POINT
 * ======================================================= */
int main(void)
{
    clock_init();

    uint32_t flag = *(uint32_t*)OTA_FLAG_ADDR;
    uint32_t size = *(uint32_t*)OTA_SIZE_ADDR;

    /* If OTA is available */
    if (flag == OTA_FLAG_VALUE)
        apply_ota(size);

    /* Jump to APP if valid */
    if (app_is_valid())
        jump_to_app();

    /* Stay here if no app */
    while (1)
    {
        /* Optional: blink LED or debug */
    }
}
