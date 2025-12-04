#include "clock_api.h"

void clock_init_64MHz_HSI(void){
  RCC_HSICmd(ENABLE);
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
  RCC_PLLConfig(
    RCC_PLLSource_HSI,
    8,                            // M: 16/8 = 2MHz
    192,                          // N: 2×192 = 384MHz 
    6,                            // P: 384/6 = 64MHz 
    8                             // Q: 384/8 = 48MHz 
  );
  RCC_PLLCmd(ENABLE);
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){}; 
  FLASH_SetLatency(FLASH_ACR_LATENCY_2WS);
  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  RCC_PCLK1Config(RCC_HCLK_Div2);
  RCC_PCLK2Config(RCC_HCLK_Div1);
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  SystemCoreClockUpdate();
}