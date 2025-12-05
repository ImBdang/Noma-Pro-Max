#include "http_engine.h"

/* ====================================== GLOBAL VARIABLES ================================== */
uint32_t reading_chunk;                                 /*<! THIS VAR UPDATE IN HTTPREAD FUNC */
uint8_t http_read_buff[HTTP_READ_BUFFER];               /*<! HTTP READ BFFER TO STORE */
uint32_t http_data_len;                                 /*<! LEN OF THE DATA THAT NEED TO READ */

/* ========================================================================================== */

/* ================================== STATIC DECLARATIONS =================================== */
static bool is_busy = false;                            /*<! Only for when read mode */
static bool http_ready = false;                         
/* ========================================================================================== */

static bool http_init_entry(void){
    at_command_t cmd = {
        .cmd = "AT+HTTPINIT",
        .expect = "",
        .timeout_ms = 12000,
        .start_tick = get_systick_ms(),
        .cb = gsm_basic_callback
        };
    return send_at_cmd(cmd);
}
static bool http_init_wait(void){
    event_t event;
    static uint8_t timeout_count = 0;
    static uint8_t max_timeout = 3;
    if (!pop_event(&response_event_queue, &event))
        return false;

    switch (event.response)
    {
        case EVT_OK:
            DEBUG_PRINT("HTTPINIT SUCCESS\r\n");
            timeout_count = 0; 
            return true;


        case EVT_TIMEOUT:
            DEBUG_PRINT("HTTPINIT TIMEOUT\r\n");
            timeout_count++;
            if (timeout_count >= max_timeout){
                timeout_count = 0;
            }
            return false;
        

        case EVT_ERR:
            DEBUG_PRINT("HTTPINIT ERROR\r\n");
            timeout_count = 0;
            return false;
    }    
}

bool http_init(void){
    static uint8_t step = 0;
    bool tmp = false;
    switch (step)
    {
    case 0:
        if (http_term())
            step++;
        return false;
    case 1:
        tmp = http_init_entry();
        if (tmp){
            step++;
        }
        return false;
    
    case 2:
        if (http_init_wait()){
            step = 0;
            http_ready = true;
            return true;
        }
        step = 0;
        return false;
    }
    return false;
}


static bool http_term_entry(void){
    at_command_t cmd = {
        .cmd = "AT+HTTPTERM",
        .expect = "",
        .timeout_ms = 12000,
        .start_tick = get_systick_ms(),
        .cb = gsm_basic_callback
        };
    return send_at_cmd(cmd);
}
static bool http_term_wait(void){
    event_t event;
    static uint8_t timeout_count = 0;
    static uint8_t max_timeout = 3;
    if (!pop_event(&response_event_queue, &event))
        return false;

    switch (event.response)
    {
        case EVT_OK:
            DEBUG_PRINT("HTTPTERM SUCCESS\r\n");
            timeout_count = 0; 
            return true;


        case EVT_TIMEOUT:
            DEBUG_PRINT("HTTPTERM TIMEOUT\r\n");
            timeout_count++;
            if (timeout_count >= max_timeout){
                timeout_count = 0;
            }
            return false;
        

        case EVT_ERR:
            DEBUG_PRINT("HTTPTERM ERROR\r\n");
            timeout_count = 0;
            return true;
    }    
}

bool http_term(void){
    static uint8_t step = 0;
    bool tmp = false;
    switch (step)
    {
    case 0:
        tmp = http_term_entry();
        if (tmp){
            step++;
        }
        return false;
    
    case 1:
        if (http_term_wait()){
            step = 0;
            http_ready = false;
            return true;
        }
        return false;
    }
    return false;
}

static bool http_seturl_entry(const char* url){
    at_command_t cmd = {
        .expect = "",
        .timeout_ms = 5000,
        .cb = gsm_basic_callback
    };
    snprintf(cmd.cmd, sizeof(cmd.cmd),"AT+HTTPPARA=\"URL\",\"%s\"", url);
    return send_at_cmd(cmd); 
}
static bool http_seturl_wait(){
    event_t event;
    static uint8_t timeout_count = 0;
    static uint8_t max_timeout = 3;
    if (!pop_event(&response_event_queue, &event))
        return false;
    switch (event.response)
    {
        case EVT_OK:
            DEBUG_PRINT("SET URL DONE\r\n");
            return true;

        case EVT_TIMEOUT:
            timeout_count++;
            if (timeout_count >= max_timeout){
                timeout_count = 0;
            }
            return false;
        

        case EVT_ERR:
            timeout_count = 0;
            return false;
    }
    return false;
}

bool http_seturl(const char* url){
    static uint8_t step = 0;
    bool tmp = false;
    switch (step)
    {
    case 0:
        tmp = http_seturl_entry(url);
        if (tmp){
            step++;
        }
        return false;
    
    case 1:
        if (http_seturl_wait()){
            step = 0;
            return true;
        }
        return false;
    }
    return false;
}


static bool http_action_entry(uint8_t action){
    char act[4];       
    char buf[32];           
    fast_itoa(action, act);
    sprintf(buf, "AT+HTTPACTION=%s", act);
    at_command_t cmd = {
        .timeout_ms = 12000,
        .expect = "",
        .start_tick = get_systick_ms(),
        .cb = gsm_basic_callback
    };
    strcpy(cmd.cmd, buf); 
    return send_at_cmd(cmd);
}
static bool http_action_wait(uint8_t action){
    event_t event;
    static uint8_t timeout_count = 0;
    static uint8_t max_timeout = 3;
    if (!pop_event(&response_event_queue, &event))
        return false;

    switch (event.response)
    {
        case EVT_OK:
            DEBUG_PRINT("HTTPACTION SUCCESS\r\n");
            timeout_count = 0; 
            return true;


        case EVT_TIMEOUT:
            DEBUG_PRINT("HTTPACTION TIMEOUT\r\n");
            timeout_count++;
            if (timeout_count >= max_timeout){
                timeout_count = 0;
            }
            return false;
        

        case EVT_ERR:
            DEBUG_PRINT("HTTPACTION ERROR\r\n");
            timeout_count = 0;
            return false;
    }   
}

bool http_action(uint8_t action){
    static uint8_t step = 0;
    bool tmp = false;
    switch (step)
    {
    case 0:
        tmp = http_action_entry(action);
        if (tmp){
            step++;
        }
        return false;
    
    case 1:
        if (http_action_wait(action)){
            step = 0;
            return true;
        }
        return false;
    }
    return false;
}


static bool http_read_entry(uint32_t offset, uint32_t chunk){     
    char buf[32];
    char chunk_str[11];
    char offset_str[11];
    fast_itoa(chunk, chunk_str);
    fast_itoa(offset, offset_str);
    sprintf(buf, "AT+HTTPREAD=%s,%s", offset_str, chunk_str);
    at_command_t cmd = {
        .timeout_ms = 12000,
        .expect = "",
        .start_tick = get_systick_ms(),
        .cb = gsm_basic_callback
    };
    strcpy(cmd.cmd, buf); 
    return send_at_cmd(cmd);
}
static bool http_read_wait(void){
    event_t event;
    static uint8_t timeout_count = 0;
    static uint8_t max_timeout = 3;
    if (!pop_event(&response_event_queue, &event))
        return false;

    switch (event.response)
    {
        case EVT_OK:
            DEBUG_PRINT("HTTPREAD OK\r\n");
            timeout_count = 0; 
            return true;


        case EVT_TIMEOUT:
            DEBUG_PRINT("HTTPREAD TIMEOUT\r\n");
            timeout_count++;
            if (timeout_count >= max_timeout){
                timeout_count = 0;
            }
            return false;
        

        case EVT_ERR:
            DEBUG_PRINT("HTTPREAD ERROR\r\n");
            timeout_count = 0;
            return false;
    }   
}

bool http_read(uint32_t offset, uint32_t chunk){
    if (is_busy)
        return false;
    if (http_data_len == 0)
        return true;
    reading_chunk = chunk;
    static uint8_t step = 0;
    bool tmp = false;
    switch (step)
    {
    case 0:
        tmp = http_read_entry(offset, chunk);
        if (tmp){
            step++;
        }
        return false;
    
    case 1:
        if (http_read_wait()){
            step = 0;
            is_busy = false;
            return true;
        }
        return false;
    }
    return false;
}

bool http_is_ready(void){
    return http_ready;
}
