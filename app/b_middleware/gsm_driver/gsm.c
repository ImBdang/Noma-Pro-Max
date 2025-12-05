#include "gsm.h"

/* ================================== EXTERN VARIABLES ====================================== */
extern event_queue_t response_event_queue;
extern event_queue_t urc_event_queue;
/* ========================================================================================== */


/* ================================== GLOBAL VARIABLES ====================================== */
sim_state_t sim_state;                                  /*<! SIM STATUS */
network_state_t network_state;                          /*<! NETWORK STATUS */
/* ========================================================================================== */


/* ================================== STATIC DECLARATIONS =================================== */
static gsm_state_t gsm_cur_state = GSM_DECISION_STATE;      /*<! Current state of gsm state machine */
static gsm_decision_flag_t decision_flag;                   /*<! Struct store many flag for each state */
static uint8_t sharedStep = 0;                              /*<! Shared step variable for every state, remember reset it to 0 after use */

/*<! Clear decision flags */
static void clear_decision_flags(void);

/*<! Those func for each state */
static void process_gsm_decision_state(void);
static void process_gsm_power_off_state(void);
static void process_gsm_power_on_state(void);
static void process_gsm_sync_at_state(void);
static void process_gsm_config_state(void);
static void process_gsm_check_sim_state(void);
static void process_gsm_reg_network_state(void);
static void process_gsm_attach_psd_state(void);
static void process_gsm_activate_pdp_state(void);
static void process_gsm_error_state(void);
static void process_gsm_ready_state(void);
/* =========================================================================================== */

/**
 * @brief   Check if GSM is ready
 */
bool gsm_is_ready(void){
    if (gsm_cur_state == GSM_READY)
        return true;
    return false;
}

/**
 * @brief   Clear decision flags
 */
static void clear_decision_flags(void)
{
    decision_flag.power_off     = false;
    decision_flag.power_on      = false;
    decision_flag.sync_at       = false;
    decision_flag.config        = false;
    decision_flag.check_sim     = false;
    decision_flag.reg_network   = false;
    decision_flag.attach_psd    = false;
    decision_flag.activate_pdp  = false;
    decision_flag.error         = false;
    decision_flag.ready         = false;
}


/**
 * @brief   Power off modem
 */
bool gsm_power_off(void){
    static uint32_t t0 = 0;

    switch (sharedStep)
    {
        case 0:
            SIM_PWKEY_LOW();
            t0 = get_systick_ms();
            sharedStep = 1;
            DEBUG_PRINT("POWERING OFF, PWKEY LOW 2.5s\r\n");
            return false;

        case 1:
            if (get_systick_ms() - t0 < 2500)
                return false;
            SIM_PWKEY_HIGH();
            t0 = get_systick_ms();
            sharedStep = 2;
            DEBUG_PRINT("POWERING OFF, PWKEY HIGH about 8.9s\r\n");
            return false;

        case 2:
            if (!SIM_STATUS_READ()){
                DEBUG_PRINT("STATUS is LOW, MODEM is now OFFLINE\r\n");
                sharedStep = 0;
                return true;
            }
            if (get_systick_ms() - t0 > 8900) {
                sharedStep = 0;                     
                DEBUG_PRINT("TIMEOUT, retry\r\n");
                return false;
            }
            DEBUG_PRINT("Waiting STATUS OFF\r\n");
            return false;
        
        default:
            break;
    }
    return false;
}


/**
 * @brief   Power on modem
 */
bool gsm_power_on(void)
{
    static uint32_t t0 = 0;

    switch (sharedStep)
    {
        case 0:
            SIM_PWKEY_LOW();
            t0 = get_systick_ms();
            sharedStep = 1;
            DEBUG_PRINT("POWERING ON, PWKEY LOW 50ms\r\n");
            return false;

        case 1:
            if (get_systick_ms() - t0 < 50)
                return false;
            SIM_PWKEY_HIGH();
            t0 = get_systick_ms();
            sharedStep = 2;
            DEBUG_PRINT("POWERING ON, PWKEY HIGH about 8.5s\r\n");
            return false;

        case 2:
            if (SIM_STATUS_READ()){
                DEBUG_PRINT("STATUS is HIGH, MODEM is now ONLINE\r\n");
                sharedStep = 0;
                return true;
            }
            if (get_systick_ms() - t0 > 8500) {
                sharedStep = 0;                   
                DEBUG_PRINT("TIMEOUT, retry\r\n");
                return false;
            }
            DEBUG_PRINT("Waiting STATUS ON\r\n");
            return false;
        
        default:
            break;
    }
    return false;
}

/**
 * @brief   Reset by turn off then turn on
 */
bool gsm_power_reset(void){
    static uint8_t step = 0;
    bool tmp = false;
    switch (step)
    {
    case 0:
        tmp = gsm_power_off();
        if (tmp){
            step++;
        }
        return false;
    
    case 1:
        tmp = gsm_power_on();
        if (tmp){
            step = 0;
            return true;
        }
        step = 0;
        return false;
    }
    return false;
}


/**
 * @brief   Change state by this decision func
 * 
 * @note    If that state flag is false that mean its not done yet so switch to it
 */
static void process_gsm_decision_state(void)
{
    /*<! STATE: ERROR */
    if (decision_flag.error) {
        gsm_cur_state = GSM_ERROR;
        return;
    }

    /*<! STATE: POWER_ON */
    if (!decision_flag.power_on) {
        gsm_cur_state = GSM_POWER_ON_STATE;
        return;
    }

    /*<! STATE: SYNC_AT */
    if (!decision_flag.sync_at) {
        gsm_cur_state = GSM_SYNC_AT_STATE;
        return;
    }

    /*<! STATE: CONFIG */
    if (!decision_flag.config) {
        gsm_cur_state = GSM_CONFIG_STATE;
        return;
    }

    /*<! STATE: CHECK_SIM */
    if (!decision_flag.check_sim) {
        gsm_cur_state = GSM_CHECK_SIM_STATE;
        return;
    }

    /*<! STATE: REG_NETWORK */
    if (!decision_flag.reg_network) {
        gsm_cur_state = GSM_REG_NETWORK_STATE;
        return;
    }

    /*<! STATE: ATTACH_PSD */
    if (!decision_flag.attach_psd) {
        gsm_cur_state = GSM_ATTACH_PSD;
        return;
    }

    /*<! STATE: ACTIVATE_PDP */
    if (!decision_flag.activate_pdp) {
        gsm_cur_state = GSM_ACTIVATE_PDP;
        return;
    }

    /*<! STATE: READY */
    if (!decision_flag.ready) {
        gsm_cur_state = GSM_READY;
        return;
    }
}


/*<! GSM_POWER_OFF_STATE ---------------------------------------------*/
static void process_gsm_power_off_state(void)
{
    if (gsm_power_off()){
        decision_flag.power_on = true;
        gsm_cur_state = GSM_DECISION_STATE;
        delay_ms(500);
    }
}


/*<! GSM_POWER_ON_STATE ----------------------------------------------*/
static void process_gsm_power_on_state(void)
{
    if (gsm_power_on()){
        decision_flag.power_on = true;
        gsm_cur_state = GSM_DECISION_STATE;
        delay_ms(500);
    }
}


/*<! GSM_SYNC_AT_STATE ------------------------------------------------*/
static bool process_gsm_sync_at_state_entry(void){
    at_command_t cmd = {
        .cmd = "AT",
        .expect = "",
        .timeout_ms =  7000,
        .cb = gsm_basic_callback
    };
    return send_at_cmd(cmd);
}
static bool process_gsm_sync_at_state_wait(void){
    event_t event;
    static uint8_t timeout_count = 0;
    static uint8_t max_timeout = 3;
    static uint8_t err_count = 0;
    static uint8_t err_max_count = 3;
    if (!pop_event(&response_event_queue, &event))
        return false;

    switch (event.response)
    {
        case EVT_OK:
            timeout_count = 0; 
            decision_flag.sync_at = true;
            gsm_cur_state = GSM_DECISION_STATE;

            return true;


        case EVT_TIMEOUT:
            decision_flag.sync_at = false;
            timeout_count++;
            if (timeout_count >= max_timeout){
                decision_flag.error = true;
                timeout_count = 0;
            }
            gsm_cur_state = GSM_DECISION_STATE;
            return false;
        

        case EVT_ERR:
            if (err_count >= err_max_count){
                err_count++;
                delay_ms(2000);
                return false;
            }
            timeout_count = 0;
            decision_flag.error = true;
            gsm_cur_state = GSM_DECISION_STATE;
            return false;
    }    
}

static void process_gsm_sync_at_state(void)
{
    static uint8_t step = 0;
    bool tmp = false;
    switch (step)
    {
    case 0:
        tmp = process_gsm_sync_at_state_entry();
        if (tmp){
            step++;
        }
        break;
    
    case 1:
        if (process_gsm_sync_at_state_wait())
            step = 0;
        break;

    }
}


/*<! GSM_CONFIG_STATE -------------------------------------------------*/
static bool process_gsm_config_state_entry(void){
    switch (sharedStep)
    {
        case 0:{
            at_command_t cmd = {
                .cmd = "AT+CFUN=1",
                .expect = "",
                .timeout_ms =  5000,
                .cb = gsm_basic_callback
            };
            return send_at_cmd(cmd);
        }
        /*<! ADD More later */
    }
    return false;
}
static bool process_gsm_config_state_wait(void){
    event_t event;
    static uint8_t timeout_count = 0;
    static uint8_t max_timeout = 3;
    if (!pop_event(&response_event_queue, &event))
        return false;

    switch (event.response)
    {
        case EVT_OK:
            timeout_count = 0; 
            decision_flag.config = true;
            gsm_cur_state = GSM_DECISION_STATE;
            sharedStep = 0;
            return true;


        case EVT_TIMEOUT:
            decision_flag.config = false;
            timeout_count++;
            if (timeout_count >= max_timeout){
                sharedStep = 0;
                decision_flag.error = true;
                timeout_count = 0;
            }
            gsm_cur_state = GSM_DECISION_STATE;
            return false;
        

        case EVT_ERR:
            sharedStep = 0;
            timeout_count = 0;
            decision_flag.error = true;
            gsm_cur_state = GSM_DECISION_STATE;
            return false;
    }    
}

static void process_gsm_config_state(void){
    static uint8_t step = 0;
    bool tmp = false;
    switch (step)
    {
    case 0:
        tmp = process_gsm_config_state_entry();
        if (tmp){
            step++;
        }
        break;
    
    case 1:
        if (process_gsm_config_state_wait())
            step = 0;
        break;

    }
}


/*<! GSM_CHECK_SIM_STATE ----------------------------------------------*/

static bool process_gsm_check_sim_state_entry(void)
{
    at_command_t cmd = {
        .cmd = "AT+CPIN?",
        .expect = "+CPIN:",
        .timeout_ms =  5000,
        .cb = gsm_check_sim_callback
    };
    return send_at_cmd(cmd);
}
static bool process_gsm_check_sim_state_wait(void)
{
    event_t event;
    static uint8_t timeout_count = 0;
    static uint8_t max_timeout = 3;
    if (!pop_event(&response_event_queue, &event))
        return false;

    switch (event.response)
    {
        case EVT_SIM_RDY:
            timeout_count = 0; 
            decision_flag.check_sim = true;
            gsm_cur_state = GSM_DECISION_STATE;
            return true;

        case EVT_SIM_PIN:
            timeout_count = 0; 
            decision_flag.check_sim = false;
            gsm_cur_state = GSM_DECISION_STATE;
            return false;

        case EVT_SIM_PUK:
            timeout_count = 0; 
            decision_flag.check_sim = false;
            gsm_cur_state = GSM_DECISION_STATE;
            return false;


        case EVT_TIMEOUT:
            decision_flag.check_sim = false;
            timeout_count++;
            if (timeout_count >= max_timeout){
                decision_flag.error = true;
                timeout_count = 0;
            }
            gsm_cur_state = GSM_DECISION_STATE;
            return false;
        

        case EVT_ERR:
            timeout_count = 0;
            decision_flag.error = true;
            gsm_cur_state = GSM_DECISION_STATE;
            return false;
    }    
}

static void process_gsm_check_sim_state(void)
{
    static uint8_t step = 0;
    bool tmp = false;
    switch (step)
    {
    case 0:
        tmp = process_gsm_check_sim_state_entry();
        if (tmp){
            step++;
        }
        break;
    
    case 1:
        process_gsm_check_sim_state_wait();
        step = 0;
        break;

    }
}


/*<! GSM_REG_NETWORK_STATE --------------------------------------------*/
static bool process_gsm_reg_network_state_entry(void){
    at_command_t cmd = {
        .cmd = "AT+CREG?",
        .expect = "+CREG:",
        .timeout_ms = 10000,
        .start_tick = get_systick_ms(),
        .cb = gsm_reg_network_callback
    };
    return send_at_cmd(cmd);
}
static bool process_gsm_reg_network_state_wait(void){
    event_t event;
    static uint8_t timeout_count = 0;
    static uint8_t max_timeout = 3;
    if (!pop_event(&response_event_queue, &event))
        return false;

    switch (event.response)
    {
        case EVT_CREG_REGISTERED_HOME:
            timeout_count = 0; 
            decision_flag.reg_network = true;
            gsm_cur_state = GSM_DECISION_STATE;
            return true;

        case EVT_CREG_REGISTERED_ROAMING:
            timeout_count = 0; 
            decision_flag.reg_network = true;
            gsm_cur_state = GSM_DECISION_STATE;
            return true;

        case EVT_CREG_NOT_REGISTERED: return false;;
        case EVT_CREG_SEARCHING: return false;;
        case EVT_CREG_REG_DENIED: return false;;
        case EVT_CREG_UNKNOWN: return false;;
        case EVT_CREG_REGISTERED_SMS: return false;;

        case EVT_CREG_REGISTERED_SMS_ROAM:
            timeout_count = 0; 
            decision_flag.reg_network = true;
            gsm_cur_state = GSM_DECISION_STATE;
            return true;;


        case EVT_TIMEOUT:
            decision_flag.reg_network = false;
            timeout_count++;
            if (timeout_count >= max_timeout){
                decision_flag.error = true;
                timeout_count = 0;
            }
            gsm_cur_state = GSM_DECISION_STATE;
            return false;;
        

        case EVT_ERR:
            timeout_count = 0;
            decision_flag.error = true;
            gsm_cur_state = GSM_DECISION_STATE;
            return false;;
    }    
}


static void process_gsm_reg_network_state(void)
{
    static uint8_t step = 0;
    bool tmp = false;
    switch (step)
    {
    case 0:
        tmp = process_gsm_reg_network_state_entry();
        if (tmp){
            step++;
        }
        break;
    
    case 1:
        if (process_gsm_reg_network_state_wait())
            step = 0;
        break;

    }
}


/*<! GSM_ATTACH_PSD ---------------------------------------------------*/
static bool process_gsm_attach_psd_state_entry(void){
    at_command_t cmd = {
        .cmd = "AT+CGATT=1",
        .expect = "",
        .timeout_ms = 10000,
        .cb = gsm_basic_callback
    };
    return send_at_cmd(cmd);  
}
static bool process_gsm_attach_psd_state_wait(void){
    event_t event;
    static uint8_t timeout_count = 0;
    static uint8_t max_timeout = 3;
    if (!pop_event(&response_event_queue, &event))
        return false;

    switch (event.response)
    {
        case EVT_OK:
            timeout_count = 0; 
            decision_flag.attach_psd = true;
            gsm_cur_state = GSM_DECISION_STATE;
            return true;


        case EVT_TIMEOUT:
            decision_flag.attach_psd = false;
            timeout_count++;
            if (timeout_count >= max_timeout){
                decision_flag.error = true;
                timeout_count = 0;
            }
            gsm_cur_state = GSM_DECISION_STATE;
            return false;
        

        case EVT_ERR:
            timeout_count = 0;
            decision_flag.error = true;
            gsm_cur_state = GSM_DECISION_STATE;
            return false;
    }  
}

static void process_gsm_attach_psd_state(void)
{
    static uint8_t step = 0;
    bool tmp = false;
    switch (step)
    {
    case 0:
        tmp = process_gsm_attach_psd_state_entry();
        if (tmp){
            step++;
        }
        break;
    
    case 1:
        if (process_gsm_attach_psd_state_wait())
            step = 0;
        break;

    }
}


/*<! GSM_ACTIVATE_PDP -------------------------------------------------*/
static bool process_gsm_activate_pdp_state_entry(void){
    switch (sharedStep){
        case 0:{
            at_command_t cmd = {
                .cmd = "AT+CGDCONT=1,\"IP\",\"v-internet\"",
                .expect = "",
                .timeout_ms = 10000,
                .cb = gsm_basic_callback
            };
            if (send_at_cmd(cmd)) 
                return true;  
            return false;
        }
        case 1:{
            at_command_t cmd = {
                .cmd = "AT+CGACT=1,1",
                .expect = "",
                .timeout_ms = 150000, 
                .cb = gsm_basic_callback
            };
            if (send_at_cmd(cmd))
                return true; 
            return false;
        }
        default:
            return false;
    }
}
static bool process_gsm_activate_pdp_state_wait(void){
    event_t event;
    static uint8_t timeout_count = 0;
    static uint8_t max_timeout = 3;
    
    if (!pop_event(&response_event_queue, &event))
        return false;

    switch (event.response)
    {
        case EVT_OK:
            timeout_count = 0;
            sharedStep++; 
            
            if (sharedStep >= 2) {  
                sharedStep = 0;
                decision_flag.activate_pdp = true;
                return true;  
            }
            else {
                return false; 
            }

        case EVT_TIMEOUT:
            decision_flag.activate_pdp = false;
            timeout_count++;
            if (timeout_count >= max_timeout){
                decision_flag.error = true;
                timeout_count = 0;
            }
            sharedStep = 0; 
            return false;
        
        case EVT_ERR:
            timeout_count = 0;
            sharedStep = 0;  
            decision_flag.error = true;
            return false;
    }
    return false;
}

static void process_gsm_activate_pdp_state(void)
{
    static uint8_t step = 0;
    bool tmp = false;
    
    switch (step)
    {
    case 0: 
        tmp = process_gsm_activate_pdp_state_entry();
        if (tmp){
            step = 1;  
        }
        break;
    
    case 1:  
        tmp = process_gsm_activate_pdp_state_wait();
        if (tmp) {
            step = 0;
            gsm_cur_state = GSM_DECISION_STATE;
        }
        else if (sharedStep > 0 && sharedStep < 2) {
            step = 0;
        }
        else if (decision_flag.error) {
            step = 0;
            gsm_cur_state = GSM_DECISION_STATE;
        }
        break;
    }
}


/*<! GSM_ERROR --------------------------------------------------------*/

static void process_gsm_error_state(void)
{
    clear_decision_flags();
    gsm_cur_state = GSM_DECISION_STATE;
    gsm_power_reset();
}


/*<! GSM_READY --------------------------------------------------------*/
static void process_gsm_ready_state(void)
{
    
}


void gsm_process(void){
    switch (gsm_cur_state)
    {
        case GSM_DECISION_STATE:
            process_gsm_decision_state();
            break;

        case GSM_POWER_ON_STATE:
            process_gsm_power_on_state();
            break;

        case GSM_SYNC_AT_STATE:
            process_gsm_sync_at_state();
            break;

        case GSM_CONFIG_STATE:
            process_gsm_config_state();
            break;

        case GSM_CHECK_SIM_STATE:
            process_gsm_check_sim_state();
            break;

        case GSM_REG_NETWORK_STATE:
            process_gsm_reg_network_state();
            break;

        case GSM_ATTACH_PSD:
            process_gsm_attach_psd_state();
            break;

        case GSM_ACTIVATE_PDP:
            process_gsm_activate_pdp_state();
            break;

        case GSM_ERROR:
            process_gsm_error_state();
            break;

        case GSM_READY:
            process_gsm_ready_state();
            break;
    }
    delay_ms(100);
}