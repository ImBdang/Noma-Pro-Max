#include "stm32f4xx.h"
#include "processor.h"

int main(void){
    processor_init();
    while (1){
        processor();
        delay_ms(500);
    }
    return 0;
}