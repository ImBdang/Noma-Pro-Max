#include "urc_handler.h"

/* ================================== STATIC DECLARATIONS =================================== */
static void urc_httpaction_process(const char* urc);
/* =========================================================================================== */


static void urc_httpaction_process(const char* urc){
    uint8_t method;
    uint32_t len;
    char status[4];
    http_action_dispatch(urc, &method, status, &len);
    if (strcmp(status, "200") == 0){
        if (len != 0){
            http_data_len = len;            /*<! Keep the data len to global variable */
            if (ota_detect == true){
                set_firmware_size(len);
            }
            DEBUG_PRINT("HTTPACTION STATUS CODE IS OK\r\n");
        }
    }
    else{
        DEBUG_PRINT("HTTPACTION STATUS CODE NOT OK\r\n");
    }
}


void urc_process(const char* urc){
    event_t event;
    if (!pop_event(&urc_event_queue, &event))
        return;

    switch (event.urc)
    {
    case URC_HTTPACTION:
        urc_httpaction_process(urc);
        break;
    
    case URC_SMS_NEW:

        break;

    case URC_HTTPREAD:
        http_read_state = HTTP_READ_BUSY;
        http_read_ptr = http_read_buff;
        break;
    }
}