#include <lld_steer_step_motor.h>
#include <lld_control.h>


/* Service callback functions declaration (prototypes)*/
void risingEdgeSteerMCallback(PWMDriver *pwmd);
void fallingEdgeSteerMCallback(PWMDriver *pwmd);

//TODO: check PWM channel and DIR pin!

/* Clutch motor structure declaration*/
MotorDriver SteerM = {
    .pwmd            =   &PWMD12,
    .dir_line        =   PAL_LINE(GPIOF, 2),
    .rising_edge_cb  =   risingEdgeSteerMCallback,
    .falling_edge_cb =   fallingEdgeSteerMCallback,
    .max_position    =   50000,
    .min_position    =  -50000
};
void risingEdgeSteerMCallback(PWMDriver *pwmd){

    (void) pwmd;
    risingEdgeCb(&SteerM);
}
void fallingEdgeSteerMCallback(PWMDriver *pwmd){

    (void) pwmd;
    fallingEdgeCb(&SteerM);
    //palToggleLine(LINE_LED1);
}


static bool if_lld_steer_module_initialized = 0;

/*@brief      Perephery setup and lld initialization */
void lldSteerSMInit ( void ){
     if ( if_lld_steer_module_initialized ){
         return;
     }

     /* Clutch init*/
     palSetLineMode( PAL_LINE( GPIOB, 14),  PAL_MODE_ALTERNATE(9) );
     palSetLineMode( SteerM.dir_line, PAL_MODE_OUTPUT_PUSHPULL);
     MotorlldControlInit( &SteerM );

     if_lld_steer_module_initialized = 1;
}

void lldSteerSMMoveToTheRight ( uint16_t speed ){
    MotorRunContinuous(&SteerM, 0, speed);
}
void lldSteerSMMoveToTheLeft ( uint16_t speed ){
    MotorRunContinuous(&SteerM, 1, speed);
}

int32_t lldSteerSMSetPosition (double position, uint16_t speed ){
    MotorRunTracking( &SteerM,   speed);
    SteerM.tracked_position = position;
    return  SteerM.position;
}

void lldSteerSMStop ( void ){
    MotorStop( &SteerM );
}

void lldSteerSMChangeSpeed ( uint16_t new_speed ){
    MotorSetSpeed( &SteerM, new_speed );
}

int32_t lldSteerSMGetPosition ( void ){
    return SteerM.position;
}

bool lldSteerSMGetState ( void ){
    return SteerM.state;
}

uint8_t lldSteerSMGetMode ( void ){
    return SteerM.mode;
}

uint16_t lldSteerSMGetSpeed ( void ){
    return  SteerM.pwmd->period;
}

int32_t lldSteerSMGetMaxPosition ( void )
{
    return SteerM.max_position;
}

int32_t lldSteerSMGetMinPosition ( void )
{
    return SteerM.min_position;
}

