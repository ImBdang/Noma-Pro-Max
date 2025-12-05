#include "at_engine.h"

/* ====================================== GLOBAL VARIABLES ================================== */
lwrb_t  usart_rb;                               /*<! LWRB */
uint8_t line_buff[LINE_BUFFER_SIZE];            /*<! Buffer get data from LWRB to process */
uint8_t* http_read_ptr = NULL;
/* ========================================================================================== */

/* ================================== STATIC DECLARATIONS =================================== */
static uint8_t lwrb_buffer[LWRB_BUFFER_SIZE];   /*<! Buffer for lwrb that store usart rx */
static bool is_busy = false;                    /*<! Flag to know any AT command is sending, mostly to classify */
static at_command_t executing_cmd;              /*<! Executing AT command */

static void handle_response_line(const char *line);
static void handle_urc_line(const char *urc);
static void line_parse(void);
/* ========================================================================================== */



/**
 * @brief   Send AT command
 * 
 * @param   cmd: Struct that description command as liek expect response, command, ...
 * 
 * @note    Callback func will handler the response, dont need to process response in this func
 *          We only need to make sure that setup the params as like start time then send through USART
 * 
 * @retval  true if send success
 * @retval  false is send not success (mostly is_busy is true, other command is still wating for response)
 */
bool send_at_cmd(at_command_t cmd){
    if (is_busy)
        return false;
    executing_cmd = cmd;
    executing_cmd.start_tick = get_systick_ms();
    is_busy = true;

    usart_sendstring(SIM_USART_PORT, executing_cmd.cmd);
    usart_sendstring(SIM_USART_PORT, "\r\n");
    DEBUG_PRINT(">> AT CMD: ");
    DEBUG_PRINT(executing_cmd.cmd);
    DEBUG_PRINT("\r\n");
    return true;
}

/**
 * @brief   Parse the line that GSM modem resposne
 */
void line_parse(void){
    static uint16_t line_len = 0;       /*<! Len of the line resposne */
    uint8_t c;
    static uint16_t checkp = 0;
    while (lwrb_read(&usart_rb, &c, 1)){

        /*<! ACTIVE TO COLLECT HTTPDATA, ENABLE WHEN HTTPREAD */
        if (http_read_state == HTTP_READ_BUSY) {
            checkp++;
            *http_read_ptr++ = c;
            reading_chunk--;
            if (reading_chunk == 0) {
                http_read_state = HTTP_READ_DONE;
            }
            continue; 
        }

        if (line_len < sizeof(line_buff) - 1) {

            if (c != 0xFF){
                line_buff[line_len++] = c;
            }
        }

        if (c == '\n') {
            line_buff[line_len] = '\0';     
            if (is_busy){
                DEBUG_PRINT("<< RESPONSE: ");
                DEBUG_PRINT(line_buff);
                DEBUG_PRINT("\r");
                handle_response_line(line_buff);
            }
            else {
                DEBUG_PRINT("<< URC: ");
                DEBUG_PRINT(line_buff);
                DEBUG_PRINT("\r");
                handle_urc_line(line_buff);
            }
            line_len = 0;              
        }

    }
}

/**
 * @brief   Handle a single response line from the modem.
 *
 * This function processes one line returned by the modem and classifies it into
 * one of the following response categories:
 *
 *  - PRIMARY:       The line matches the expected pattern of the executing command.
 *  - OK_RESP:       The terminal "OK" response.
 *  - ERROR_RESP:    The terminal "ERROR" response.
 *  - INTERMEDIATE:  Any line that is not primary or final.
 *
 * The processing sequence:
 *  1) Ignore empty lines, or lines starting with '\r' / '\n'
 *  2) Check for PRIMARY match using `executing_cmd.expect`
 *  3) Detect terminal "OK"
 *  4) Detect terminal "ERROR"
 *  5) Otherwise, treat it as an INTERMEDIATE line
 *
 * @param[in] line   Null-terminated response string from the modem.
 *
 * @note This function does NOT buffer or queue lines. It immediately forwards
 *       the parsed classification to the user callback defined in `executing_cmd.cb`.
 *
 * @note Setting `is_busy = false` occurs when a final result ("OK" or "ERROR") is detected.
 */
void handle_response_line(const char *line){
    /*<! If the line is empty */
    size_t len = strlen(line);
    if (len == 0 || line[0] == '\r' || line[0] == '\n')
        return;
        
    /*<! PRIMARY response */
    if (executing_cmd.expect[0] != '\0' &&
        strncmp(line, executing_cmd.expect, strlen(executing_cmd.expect)) == 0)
    {
        if (executing_cmd.cb)
            executing_cmd.cb(PRIMARY_RESP, line, strlen(line));
        return;
    }

    /*<! OK response */
    if (strstr(line, "OK") != NULL)
    {
        is_busy = false;
        if (executing_cmd.cb)
            executing_cmd.cb(OK_RESP ,line, 2);
        return;
    }

    /*<! ERROR response*/    
    if (strstr(line, "ERROR") != NULL)
    {
        is_busy = false;
        if (executing_cmd.cb)
            executing_cmd.cb(ERROR_RESP, line, 5);

        return;
    }

    /*<! INTERMEDIATE response */
    if (executing_cmd.cb) {
        executing_cmd.cb(INTERMEDIATE, line, strlen(line));
    }
}

/**
 * @brief   Handle urc line
 */
void handle_urc_line(const char *urc){
    /*<! NEW SMS URC */
    if (strncmp(urc, "+CMTI:", 6) == 0) {
        event_t event = {
            .urc = URC_SMS_NEW
        };
        push_event(&urc_event_queue, event);
        urc_process(urc);
        return;
    }

    if (strncmp(urc, "+CREG:", 6) == 0) {
        urc_process(urc);
        return;
    }

    if (strncmp(urc, "+HTTPACTION:", 12) == 0) {
        event_t event = {
            .urc = URC_HTTPACTION
        };
        push_event(&urc_event_queue, event);
        urc_process(urc);
        return;
    }

    if (strncmp(urc, "+HTTPREAD:", 10) == 0) {
        uint32_t size = httpread_dispatch(urc);
        if (size == 0){
            http_read_state = HTTP_READ_IDLE;   
            return;
        }
        event_t event = {
            .urc = URC_HTTPREAD
        };
        push_event(&urc_event_queue, event);
        urc_process(urc);
        return;
    }


    if (strcmp(urc, "RING") == 0) {
        urc_process(urc);
        return;
    }
}

/**
 * @brief   Daily task, check and parse line
 *          Check AT command
 */
void at_engine_process(void){
    line_parse();
    if (is_busy) {
        uint32_t now = get_systick_ms();
        if (now - executing_cmd.start_tick >= executing_cmd.timeout_ms) {
            is_busy = false;
            if (executing_cmd.cb){
                executing_cmd.cb(TIMEOUT_RESP ,"TIMEOUT", 7);
                is_busy = false;
            }
        }
    }
}

/**
 * @brief   Init engine
 */
void at_engine_init(void){
    lwrb_init(&usart_rb, lwrb_buffer, sizeof(lwrb_buffer));
}