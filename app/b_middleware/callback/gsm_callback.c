#include "gsm_callback.h"

/* ================================== EXTERN VARIABLES ====================================== */
extern event_queue_t response_event_queue;
extern sim_state_t sim_state;
extern network_state_t network_state;
/* ========================================================================================== */

void gsm_basic_callback(response_status_t status, const char* line, uint32_t len){
    switch (status){
        case OK_RESP:{
            event_t event = {
                .response = EVT_OK
            };
            DEBUG_PRINT("<!>: ");
            DEBUG_PRINT("EVENT OK\r\n");
            push_event(&response_event_queue, event);
            break;
        }

        case ERROR_RESP:{
            event_t event = {
                .response = EVT_ERR
            };
            DEBUG_PRINT("<!>: ");
            DEBUG_PRINT("EVENT ERROR\r\n");
            push_event(&response_event_queue, event);
            break;
        }

        case TIMEOUT_RESP:{
            event_t event = {
                .response = EVT_TIMEOUT
            };
            DEBUG_PRINT("<!>: ");
            DEBUG_PRINT("EVENT TIMEOUT\r\n");
            push_event(&response_event_queue, event);
            break;
        }
    }    
}

void gsm_check_sim_callback(response_status_t status, const char* line, uint32_t len){
    char value[16];

    switch (status)
    {
        case PRIMARY_RESP:
            primary_dispatch(line, value);

            if (strstr(value, "READY"))
                sim_state = SIM_STATE_READY;

            else if (strstr(value, "SIM PIN"))
                sim_state = SIM_STATE_PIN;

            else if (strstr(value, "SIM PUK"))
                sim_state = SIM_STATE_PUK;

            else
                sim_state = SIM_STATE_NOT_RDY;

            break;

        case OK_RESP:
            switch (sim_state)
            {
                case SIM_STATE_READY:{
                    event_t event = {
                        .response = EVT_SIM_RDY
                    };
                    DEBUG_PRINT("<!>: ");
                    DEBUG_PRINT("EVENT OK CPIN READY\r\n");
                    push_event(&response_event_queue, event);
                    break;
                }

                case SIM_STATE_PIN:{
                    event_t event = {
                        .response = EVT_SIM_PIN
                    };
                    DEBUG_PRINT("<!>: ");
                    DEBUG_PRINT("EVENT OK BUT CPIN PIN\r\n");
                    push_event(&response_event_queue, event);
                    break;
                }

                case SIM_STATE_PUK:{
                    event_t event = {
                        .response = EVT_SIM_PUK
                    };
                    DEBUG_PRINT("<!>: ");
                    DEBUG_PRINT("EVENT OK BUT CPIN PUK\r\n");
                    push_event(&response_event_queue, event);
                    break;
                }
            }
            break;

        case ERROR_RESP:{
            event_t event = {
                .response = EVT_ERR
            };
            DEBUG_PRINT("<!>: ");
            DEBUG_PRINT("EVENT ERROR\r\n");
            sim_state = SIM_STATE_NOT_RDY;    
            push_event(&response_event_queue, event);
            break;
        }
            

        case TIMEOUT_RESP:{
            event_t event = {
                .response = EVT_TIMEOUT
            };
            DEBUG_PRINT("<!>: ");
            DEBUG_PRINT("EVENT TIMEOUT\r\n");
            sim_state = SIM_STATE_NOT_RDY;    
            push_event(&response_event_queue, event);
            break;
        }
    }
}

void gsm_reg_network_callback(response_status_t status, const char* line, uint32_t len){
    char value[16];
    switch (status)
    {
        case PRIMARY_RESP:
            primary_dispatch(line, value);
            if (strstr(value, "0,0"))
                network_state = NET_NOT_REGISTERED;

            else if (strstr(value, "0,1"))
                network_state = NET_REGISTERED_HOME;

            else if (strstr(value, "0,2"))
                network_state = NET_SEARCHING;

            else if (strstr(value, "0,3"))
                network_state = NET_REG_DENIED;

            else if (strstr(value, "0,4"))
                network_state = NET_UNKNOWN;

            else if (strstr(value, "0,5"))
                network_state = NET_REGISTERED_ROAMING;

            else if (strstr(value, "0,6"))
                network_state = NET_REGISTERED_SMS;

            else if (strstr(value, "0,7"))
                network_state = NET_REGISTERED_SMS_ROAM;

            break;

        case OK_RESP:
        {
            event_t event;

            switch (network_state)
            {
                case NET_NOT_REGISTERED:
                    event.response = EVT_SIM_RDY;
                    DEBUG_PRINT("<!>: EVENT OK NET_NOT_REGISTERED\r\n");
                    push_event(&response_event_queue, event);
                    break;

                case NET_REGISTERED_HOME:
                    event.response = EVT_CREG_REGISTERED_HOME;
                    DEBUG_PRINT("<!>: EVENT OK NET_REGISTERED_HOME\r\n");
                    push_event(&response_event_queue, event);
                    break;

                case NET_SEARCHING:
                    event.response = EVT_CREG_SEARCHING;
                    DEBUG_PRINT("<!>: EVENT OK NET_SEARCHING\r\n");
                    push_event(&response_event_queue, event);
                    break;

                case NET_REG_DENIED:
                    event.response = EVT_CREG_REG_DENIED;
                    DEBUG_PRINT("<!>: EVENT OK NET_REG_DENIED\r\n");
                    push_event(&response_event_queue, event);
                    break;

                case NET_UNKNOWN:
                    event.response = EVT_CREG_UNKNOWN;
                    DEBUG_PRINT("<!>: EVENT OK NET_UNKNOWN\r\n");
                    push_event(&response_event_queue, event);
                    break;

                case NET_REGISTERED_ROAMING:
                    event.response = EVT_CREG_REGISTERED_ROAMING;
                    DEBUG_PRINT("<!>: EVENT OK NET_REGISTERED_ROAMING\r\n");
                    push_event(&response_event_queue, event);
                    break;

                case NET_REGISTERED_SMS:
                    event.response = EVT_CREG_REGISTERED_SMS;
                    DEBUG_PRINT("<!>: EVENT OK NET_REGISTERED_SMS\r\n");
                    push_event(&response_event_queue, event);
                    break;

                case NET_REGISTERED_SMS_ROAM:
                    event.response = EVT_CREG_REGISTERED_SMS_ROAM;
                    DEBUG_PRINT("<!>: EVENT OK NET_REGISTERED_SMS_ROAM\r\n");
                    push_event(&response_event_queue, event);
                    break;
            }
            break;
        }
    
        case ERROR_RESP:
        {
            event_t event = {
                .response = EVT_ERR
            };
            push_event(&response_event_queue, event);
            network_state = NET_NOT_REGISTERED;   
            break;
        }  
        
        case TIMEOUT_RESP:
        {
            event_t event = {
                .response = EVT_TIMEOUT
            };
            push_event(&response_event_queue, event);
            network_state = NET_NOT_REGISTERED;    
            break;
        }
    }
}