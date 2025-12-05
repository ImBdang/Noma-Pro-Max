#include "processor.h"

/* ================================== STATIC DECLARATIONS =================================== */

/* =========================================================================================== */

void processor_init(void){
    hardware_init();
    at_engine_init();  
    ota_init();
}

void processor(void){
    static uint8_t step = 0;
    static bool flag = true;
    at_engine_process();        /*<! Parse and get line command */
    gsm_process();              /*<! Finite State Machine GSM */
    if (gsm_is_ready()){
        ota_process();
    }
}