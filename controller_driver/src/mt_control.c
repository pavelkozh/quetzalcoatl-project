#include <mt_control.h>
#include <lld_control.h>
#include <common.h>

/* LS - Limit Switch */
#define VERTICAL_UPPER_LS_PAD 12
#define VERTICAL_UPPER_LS_LINE PAL_LINE( GPIOC, VERTICAL_UPPER_LS_PAD )

#define VERTICAL_LOWER_LS_PAD 11
#define VERTICAL_LOWER_LS_LINE PAL_LINE( GPIOC, VERTICAL_LOWER_LS_PAD )

#define HORIZONTAL_RIGHT_LS_PAD 2
#define HORIZONTAL_RIGHT_LS_LINE PAL_LINE( GPIOG, HORIZONTAL_RIGHT_LS_PAD )


#define HORIZONTAL_LEFT_LS_PAD 3
#define HORIZONTAL_LEFT_LS_LINE PAL_LINE( GPIOG, HORIZONTAL_LEFT_LS_PAD )



void risingEdgeMTVerticalCallback(PWMDriver *pwmd);
void fallingEdgeMTVerticalCallback(PWMDriver *pwmd);

void risingEdgeMTGorisontalCallback(PWMDriver *pwmd);
void fallingEdgeMTGorisontalCallback(PWMDriver *pwmd);


MotorDriver m_vertical = {
                          .pwmd              = &PWMD14,
                          .dir_line          = PAL_LINE (GPIOE, 3 ),
                          .rising_edge_cb    = risingEdgeMTGorisontalCallback,
                          .falling_edge_cb   = fallingEdgeMTGorisontalCallback,
                          .max_position      = 20000,//15000,
                          .min_position      = -25000//-25000


};

void risingEdgeMTVerticalCallback(PWMDriver *pwmd)
{

    (void) pwmd;
    risingEdgeCb(&m_vertical);

}
void fallingEdgeMTVerticalCallback(PWMDriver *pwmd)
{

    (void) pwmd;
    fallingEdgeCb(&m_vertical);
}


MotorDriver m_gorisontal = {
                              .pwmd              = &PWMD11,
                              .dir_line          = PAL_LINE (GPIOG, 1 ),
                              .rising_edge_cb    = risingEdgeMTVerticalCallback,
                              .falling_edge_cb   = fallingEdgeMTVerticalCallback,
                              .max_position      = 20000,//14000,
                              .min_position      = -20000//-14000
};

void risingEdgeMTGorisontalCallback(PWMDriver *pwmd)
{

    (void) pwmd;
    risingEdgeCb(&m_gorisontal);

}
void fallingEdgeMTGorisontalCallback(PWMDriver *pwmd)
{

    (void) pwmd;
    fallingEdgeCb(&m_gorisontal);
}



typedef struct {
    int32_t x;
    int32_t y;
}gearCoordinates;

gearCoordinates points_array [7];

void mtControlInit ( void )
{
    /*Motor driver Setting */
    palSetLineMode( PAL_LINE( GPIOF, 7),  PAL_MODE_ALTERNATE(3) );
    palSetLineMode( m_vertical.dir_line, PAL_MODE_OUTPUT_PUSHPULL);
    MotorlldControlInit( &m_vertical );

    palSetLineMode( PAL_LINE( GPIOF, 9),  PAL_MODE_ALTERNATE(9) );
    palSetLineMode( m_gorisontal.dir_line, PAL_MODE_OUTPUT_PUSHPULL);
    MotorlldControlInit( &m_gorisontal );

    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );    //Led 1
    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led 2
    palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );    //Led 3

    /*neutral gear coordinates*/
    points_array[0].x = 0; //neutral gear horizontal coordinate
    points_array[0].y = 0; //neutral gear vertical coordinate
    /*first gear coordinates*/
    points_array[1].x = -15000; //first gear horizontal coordinate
    points_array[1].y = 15000; //first gear vertical coordinate
    /*second gear coordinates*/
    points_array[2].x = -15000; //second gear horizontal coordinate
    points_array[2].y = -20000; //second gear vertical coordinate
    /*third gear coordinates*/
    points_array[3].x = 0; //third gear horizontal coordinate
    points_array[3].y = 18000; //third gear vertical coordinate
    /*fourth gear coordinates*/
    points_array[4].x = 0; //fourth gear horizontal coordinate
    points_array[4].y = -18000; //fourth gear vertical coordinate
    /*fifth gear coordinates*/
    points_array[5].x = 12500; //fifth gear horizontal coordinate
    points_array[5].y = 13000; //fifth gear vertical coordinate
    /*reverse gear coordinates*/
    points_array[6].x = 16000; //reverse gear horizontal coordinate
    points_array[6].y = -22000; //reverse gear vertical coordinate
}

void setTrackedMode ( uint16_t vertical_speed, uint16_t gorisontal_speed )
{
    MotorRunTracking( &m_vertical,   vertical_speed);
    MotorRunTracking( &m_gorisontal, gorisontal_speed);
}

void setTrackedModePositionVerticalMotor ( int32_t pos )
{
    setTrackedMode ( 10000, 10000 );
    m_vertical.tracked_position = pos;
}

void setTrackedModePositionGorisontalMotor ( int32_t pos )
{
    setTrackedMode ( 10000, 10000 );
    m_gorisontal.tracked_position = pos;
}

void verticalMotorStop ( void )
{
    MotorStop( &m_vertical );
}

void gorisontalMotorStop ( void )
{
    MotorStop( &m_gorisontal );
}

void setVerticalMotorMaxPosition ( int32_t max_pos )
{
    m_vertical.max_position = max_pos;
}

void setGorisontalMotorMaxPosition ( int32_t max_pos )
{
    m_gorisontal.max_position = max_pos;
}

void verticalCaclibration( bool dir, uint16_t speed, uint16_t step )
{
    MotorRunCaclibration( &m_vertical, dir, speed, step );
}

void gorisontalCaclibration( bool dir, uint16_t speed, uint16_t step )
{
    MotorRunCaclibration( &m_gorisontal, dir, speed, step );
}

void verticalMotorRunContinuous (bool direction, uint16_t speed)
{
    MotorRunContinuous( &m_vertical, direction, speed);
}

void gorisontalMotorRunContinuous (bool direction, uint16_t speed)
{
    MotorRunContinuous( &m_gorisontal, direction, speed);
}



int32_t getVerticalPosition ( void )
{
    return m_vertical.position;
}

int32_t getGorisontalPosition ( void )
{
    return m_gorisontal.position;
}


int32_t getVerticalMaxPosition ( void )
{
    return m_vertical.max_position;
}

int32_t getGorisontalMaxPosition ( void )
{
    return m_gorisontal.max_position;
}


int32_t getVerticalTargetPosition ( void )
{
    return m_vertical.tracked_position;
}

int32_t getGorisontalTargetPosition ( void )
{
    return m_gorisontal.tracked_position;
}

bool getGorisontalState ( void )
{
    return m_gorisontal.state;
}

bool getVerticalState ( void )
{
    return m_vertical.state;
}

uint8_t getGorisontalMode ( void )
{
    return m_gorisontal.mode;
}

uint8_t getVerticalMode ( void )
{
    return m_vertical.mode;
}

uint16_t getGorisontalSpeed ( void )
{
    return  m_gorisontal.pwmd->period;
}

uint16_t getVerticalSpeed ( void )
{
    return m_vertical.pwmd->period;
}

//static bool neutral_gear_flag = false;
int8_t currently_selected_gear = -1;

int8_t shiftMTToNeutral ( uint16_t speed )
{
    setTrackedMode ( speed, speed );
    m_vertical.tracked_position   =  points_array[0].y;
    if ( m_vertical.position == m_vertical.tracked_position)
    {
        m_gorisontal.tracked_position =  points_array[0].x;
    }
    if ( ( m_gorisontal.position == m_gorisontal.tracked_position) && ( m_vertical.position == m_vertical.tracked_position) )
    {
        currently_selected_gear = 0;
        //palSetLine(LINE_LED1);
    }

    return currently_selected_gear;
}


int8_t shiftMTToNextGear (int8_t gear_num, uint16_t speed)
{
    setTrackedMode ( speed, speed );
    if ( currently_selected_gear != gear_num )
    {
        if ( currently_selected_gear != 0 ) //currently selected gear is not a neutral gear
        {
//            if (  (( currently_selected_gear == 1 ) && (gear_num == 2)) ||  (( currently_selected_gear == 2 ) && (gear_num == 1))  )
//            {
//                currently_selected_gear = 0;
//            }
//            else
//            {
//                shiftMTToNeutral ( speed ); // firstly disable currently selected gear
//            }
            shiftMTToNeutral ( speed ); // firstly disable currently selected gear

        }
        else
        {
            m_gorisontal.tracked_position =  points_array[gear_num].x;
            if ( m_gorisontal.position == m_gorisontal.tracked_position )
            {
                //palToggleLine(LINE_LED2);
               // palSetLine(LINE_LED2);
                m_vertical.tracked_position   =  points_array[gear_num].y;
            }
            if ((   m_vertical.position == m_vertical.tracked_position ) && ( m_gorisontal.position == m_gorisontal.tracked_position ))
            {
                palSetLine(LINE_LED3);
                currently_selected_gear = gear_num;
            }
        }
    }
    return currently_selected_gear;
}


/****************************************/
/********GEARSHIFT CALIBRATION***********/
/****************************************/


/***     EXT interrupts channel processing     ***/
static void extcb_vertical_upper_sensor(EXTDriver *extp, expchannel_t channel)
{
    (void)extp;
    (void)channel;


    palToggleLine(LINE_LED1);
    verticalMotorStop ();
   // verticalCaclibration( 1, 20000, 2000 );
   //extChannelDisable(&EXTD1, VERTICAL_UPPER_LS_PAD);
}

static void extcb_vertical_lower_sensor(EXTDriver *extp, expchannel_t channel)
{
    (void)extp;
    (void)channel;

    palToggleLine(LINE_LED1);
    verticalMotorStop ();
   // verticalCaclibration( 0, 20000, 2000 );
   //extChannelDisable(&EXTD1, VERTICAL_LOWER_LS_PAD);
}

static void extcb_horizontal_left_sensor(EXTDriver *extp, expchannel_t channel)
{
    (void)extp;
    (void)channel;

    palToggleLine(LINE_LED2);
    gorisontalMotorStop ();

   // gorisontalCaclibration( 0, 20000, 2000 );
   //extChannelDisable(&EXTD1, HORIZONTAL_LEFT_LS_PAD);
}

static void extcb_horizontal_right_sensor(EXTDriver *extp, expchannel_t channel)
{
    (void)extp;
    (void)channel;

    palToggleLine(LINE_LED3);
    gorisontalMotorStop ();

    //gorisontalCaclibration( 1, 20000, 2000 );
    //    extChannelDisable(&EXTD1, HORIZONTAL_RIGHT_LS_PAD);
}


void calibrationMTInit ( void )
{
    /*EXT driver initialization*/
    commonExtDriverInit();

    /* Define channel config structure
     * ch1_conf stands for vertical upper limit switch
     * ch2_conf stands for vertical lower limit switch
     * ch3_conf stands for horizontal left limit switch
     * ch4_conf stands for horizontal right limit switch*/
    EXTChannelConfig ch1_conf,ch2_conf,ch3_conf,ch4_conf;

    /* Fill in configuration for channels */
    ch1_conf.mode     = EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOC;
    ch1_conf.cb       = extcb_vertical_upper_sensor;

    ch2_conf.mode     = EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOC;
    ch2_conf.cb       = extcb_vertical_lower_sensor;

    ch3_conf.mode     = EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOG;
    ch3_conf.cb       = extcb_horizontal_left_sensor;

    ch4_conf.mode     = EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOG;
    ch4_conf.cb       = extcb_horizontal_right_sensor;

    /* Set up EXT channel hardware pin mode as digital input  */
    palSetLineMode( VERTICAL_UPPER_LS_LINE,   PAL_MODE_INPUT_PULLDOWN );
    palSetLineMode( VERTICAL_LOWER_LS_LINE,   PAL_MODE_INPUT_PULLDOWN );
    palSetLineMode( HORIZONTAL_LEFT_LS_LINE,  PAL_MODE_INPUT_PULLDOWN );
    palSetLineMode( HORIZONTAL_RIGHT_LS_LINE, PAL_MODE_INPUT_PULLDOWN );

    /* Set channel (second arg) mode with filled configuration */
    extSetChannelMode( &EXTD1, VERTICAL_UPPER_LS_PAD,   &ch1_conf );
    extSetChannelMode( &EXTD1, VERTICAL_LOWER_LS_PAD,   &ch2_conf );
    extSetChannelMode( &EXTD1, HORIZONTAL_LEFT_LS_PAD,  &ch3_conf );
    extSetChannelMode( &EXTD1, HORIZONTAL_RIGHT_LS_PAD, &ch4_conf );

}


uint8_t doCalibrationMT ( void )
{
    /* MT motors control initialization*/
    mtControlInit ();

    /*EXT Init, EXT interrupt enable */
    calibrationMTInit ();


    /* for sensors state return */
    uint8_t vupLS_state  = 0,
            vlowLS_state = 0,
            hlLS_state   = 0,
            hrLS_state   = 0;

    vupLS_state  = palReadLine( VERTICAL_UPPER_LS_LINE );
    vlowLS_state = palReadLine( VERTICAL_LOWER_LS_LINE );
    hrLS_state   = palReadLine( HORIZONTAL_RIGHT_LS_LINE );
    hlLS_state   = palReadLine( HORIZONTAL_LEFT_LS_LINE );

    /* check vertical axis upper and lower sensors PADs */
    /*if upper sensor is on -> move down until lower sensor external interrupt occur
     *if lower sensor is on -> move up until upper sensor external interrupt occur */
    if ( !palReadLine( VERTICAL_UPPER_LS_LINE ) )
    {
        verticalMotorRunContinuous (0, 20000);
    }
    if ( !palReadLine( VERTICAL_LOWER_LS_LINE ) )
    {
        verticalMotorRunContinuous (1, 20000);
    }

   /* check horizontal axis left and right sensors PADs
     * if left sensor is on -> move to the right until right sensor external interrupt occur
     * if right sensor is on -> move to the left until left sensor external interrupt occur */
    if ( !palReadLine( HORIZONTAL_RIGHT_LS_LINE ) )
    {
        extChannelDisable(&EXTD1, HORIZONTAL_RIGHT_LS_PAD);
        gorisontalMotorRunContinuous (1, 20000);
    }
    if ( !palReadLine( HORIZONTAL_LEFT_LS_LINE ) )
    {
        extChannelDisable(&EXTD1, HORIZONTAL_LEFT_LS_PAD);
        gorisontalMotorRunContinuous (0, 20000);
    }

//    uint8_t sensors_state = (vupLS_state << 3)||(vlowLS_state << 2)||(hrLS_state << 1)||(hlLS_state << 0);


    return 0;// sensors_state;
}




