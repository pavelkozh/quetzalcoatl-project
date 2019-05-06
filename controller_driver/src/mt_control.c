#include <mt_control.h>
#include <lld_control.h>



void risingEdgeMTVerticalCallback(PWMDriver *pwmd);
void fallingEdgeMTVerticalCallback(PWMDriver *pwmd);

void risingEdgeMTGorisontalCallback(PWMDriver *pwmd);
void fallingEdgeMTGorisontalCallback(PWMDriver *pwmd);


MotorDriver m_vertical = {
                          .pwmd              = &PWMD11,
                          .dir_line          = PAL_LINE (GPIOG, 1 ),
                          .rising_edge_cb    = risingEdgeMTVerticalCallback,
                          .falling_edge_cb   = fallingEdgeMTVerticalCallback,
                          .max_position      = 54000

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
                          .pwmd              = &PWMD14,
                          .dir_line          = PAL_LINE (GPIOE, 3 ),
                          .rising_edge_cb    = risingEdgeMTGorisontalCallback,
                          .falling_edge_cb   = fallingEdgeMTGorisontalCallback,
                          .max_position      = 54000
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

//    /*neutral gear coordinates*/
//    points_array[0].x = 0; //neutral gear horizontal coordinate
//    points_array[0].y = 0; //neutral gear vertical coordinate
//    /*first gear coordinates*/
//    points_array[1].x = m_gorisontal.max_position / 2; //first gear horizontal coordinate
//    points_array[1].y = m_vertical.max_position / 2; //first gear vertical coordinate
//    /*second gear coordinates*/
//    points_array[2].x = m_gorisontal.max_position / 2; //second gear horizontal coordinate
//    points_array[2].y = -m_vertical.max_position / 2; //second gear vertical coordinate
//    /*third gear coordinates*/
//    points_array[3].x = 0; //third gear horizontal coordinate
//    points_array[3].y = m_vertical.max_position / 2; //third gear vertical coordinate
//    /*fourth gear coordinates*/
//    points_array[4].x = 0; //fourth gear horizontal coordinate
//    points_array[4].y = -m_vertical.max_position / 2; //fourth gear vertical coordinate
//    /*fifth gear coordinates*/
//    points_array[5].x = -m_gorisontal.max_position / 2; //fifth gear horizontal coordinate
//    points_array[5].y = m_vertical.max_position / 2; //fifth gear vertical coordinate
//    /*reverse gear coordinates*/
//    points_array[6].x = -m_gorisontal.max_position / 2; //reverse gear horizontal coordinate
//    points_array[6].y = -m_vertical.max_position / 2; //reverse gear vertical coordinate

    /*neutral gear coordinates*/
     points_array[0].x = m_gorisontal.max_position/2; //neutral gear horizontal coordinate
     points_array[0].y = m_vertical.max_position/2; //neutral gear vertical coordinate
     /*first gear coordinates*/
     points_array[1].x = m_gorisontal.max_position; //first gear horizontal coordinate
     points_array[1].y = m_vertical.max_position; //first gear vertical coordinate
     /*second gear coordinates*/
     points_array[2].x = m_gorisontal.max_position; //second gear horizontal coordinate
     points_array[2].y = 0; //second gear vertical coordinate
     /*third gear coordinates*/
     points_array[3].x = m_gorisontal.max_position / 2; //third gear horizontal coordinate
     points_array[3].y = m_vertical.max_position; //third gear vertical coordinate
     /*fourth gear coordinates*/
     points_array[4].x = m_gorisontal.max_position / 2; //fourth gear horizontal coordinate
     points_array[4].y = 0; //fourth gear vertical coordinate
     /*fifth gear coordinates*/
     points_array[5].x = 0; //fifth gear horizontal coordinate
     points_array[5].y = m_vertical.max_position; //fifth gear vertical coordinate
     /*reverse gear coordinates*/
     points_array[6].x = 0; //reverse gear horizontal coordinate
     points_array[6].y = 0; //reverse gear vertical coordinate

}

void setTrackedMode ( uint16_t vertical_speed, uint16_t gorisontal_speed )
{
    MotorRunTracking( &m_vertical,   vertical_speed);
    MotorRunTracking( &m_gorisontal, gorisontal_speed);
}

void verticalCaclibration( bool dir, uint16_t speed, uint16_t step )
{
    MotorRunCaclibration( &m_vertical, dir, speed, step );
}

void gorisontalCaclibration( bool dir, uint16_t speed, uint16_t step )
{
    MotorRunCaclibration( &m_gorisontal, dir, speed, step );
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

static bool neutral_gear_flag = false;
int8_t currently_selected_gear = -1;

void shiftMTToNeutral ( void )
{
    setTrackedMode ( 4000, 4000 );
    m_vertical.tracked_position   =  points_array[0].y;
    if ( m_vertical.position == m_vertical.tracked_position)
    {
        m_gorisontal.tracked_position =  points_array[0].x;
    }
    if ( ( m_gorisontal.position == m_gorisontal.tracked_position) && ( m_vertical.position == m_vertical.tracked_position) )
    {
        currently_selected_gear = 0;
        palSetLine(LINE_LED1);
    }


}


void shiftMTToNextGear (int8_t gear_num, uint16_t speed)
{
    setTrackedMode ( speed, speed );
    if ( currently_selected_gear != gear_num )
    {
        if ( currently_selected_gear != 0 ) //currently selected gear is not a neutral gear
        {
            shiftMTToNeutral (); // firstly disable currently selected gear
        }
        else
        {
            m_gorisontal.tracked_position =  points_array[gear_num].x;
            if ( m_gorisontal.position == m_gorisontal.tracked_position )
            {
                //palToggleLine(LINE_LED2);
                palSetLine(LINE_LED2);
                m_vertical.tracked_position   =  points_array[gear_num].y;
            }
            if ((   m_vertical.position == m_vertical.tracked_position ) && ( m_gorisontal.position == m_gorisontal.tracked_position ))
            {
                palSetLine(LINE_LED3);
                currently_selected_gear = gear_num;
            }
        }
    }
}


/* Debug function */
void resetLEDs ( void )
{
    palClearLine(LINE_LED1);
    palClearLine(LINE_LED2);
    palClearLine(LINE_LED3);

}



#if 0

void shiftToFirstGear ( void )
{
    setTrackedMode ( 4000, 4000 );
    if ((m_vertical.position != m_vertical.max_position ) || (m_gorisontal.position != m_gorisontal.max_position ))
    {
        if ( !neutral_gear_flag )
        {
            shiftMTToNeutral ();
        }

        if ((m_vertical.position == m_vertical.max_position / 2) && (m_gorisontal.position == m_gorisontal.max_position / 2))
        {
            neutral_gear_flag = true;
        }
        if ( neutral_gear_flag )
        {
            m_gorisontal.tracked_position =  m_gorisontal.max_position;
            if ( m_gorisontal.position == m_gorisontal.tracked_position )
            {
                m_vertical.tracked_position   =  m_vertical.max_position;
                palToggleLine(LINE_LED2);
                if ( m_vertical.position >= m_vertical.tracked_position-100 )
                {
                    neutral_gear_flag = false;
                    palToggleLine(LINE_LED2);
                }
            }
        }
    }
}

void shiftToSecondGear ( void )
{
    setTrackedMode ( 4000, 4000 );
    //neutral_gear_flag = false;

    if ((m_vertical.position != 0 ) || (m_gorisontal.position != m_gorisontal.max_position ))
        {
            if ( !neutral_gear_flag )
            {
                shiftMTToNeutral ();
            }

            if ((m_vertical.position == m_vertical.max_position / 2) && (m_gorisontal.position == m_gorisontal.max_position / 2))
            {
                neutral_gear_flag = true;
            }
            if ( neutral_gear_flag )
            {
                m_gorisontal.tracked_position =  m_gorisontal.max_position;
                if ( m_gorisontal.position == m_gorisontal.tracked_position )
                {
                    m_vertical.tracked_position   =  0;
                    if ( m_vertical.position == m_vertical.tracked_position )
                    {
                        neutral_gear_flag = false;
                    }
                }
            }
        }
}

void shiftToThirdGear ( void )
{
    setTrackedMode ( 4000, 4000 );

    if ((m_vertical.position != m_vertical.max_position ) || (m_gorisontal.position != m_gorisontal.max_position/2 ))
        {
            if ( !neutral_gear_flag )
            {
                shiftMTToNeutral ();
            }

            if ((m_vertical.position == m_vertical.max_position / 2) && (m_gorisontal.position == m_gorisontal.max_position / 2))
            {
                neutral_gear_flag = true;
            }
            if ( neutral_gear_flag )
            {
                m_gorisontal.tracked_position =  m_gorisontal.max_position/2;
                if ( m_gorisontal.position == m_gorisontal.tracked_position )
                {
                    m_vertical.tracked_position   =  m_vertical.max_position;
                    if ( m_vertical.position == m_vertical.tracked_position )
                    {
                        neutral_gear_flag = false;
                    }
                }
            }
        }
}

void shiftToForthGear ( void )
{
    setTrackedMode ( 4000, 4000 );

    if ((m_vertical.position != 0 ) || (m_gorisontal.position != m_gorisontal.max_position/2 ))
    {
        if ( !neutral_gear_flag )
        {
            shiftMTToNeutral ();
        }

        if ((m_vertical.position == m_vertical.max_position / 2) && (m_gorisontal.position == m_gorisontal.max_position / 2))
        {
            neutral_gear_flag = true;
        }
        if ( neutral_gear_flag )
        {
            m_gorisontal.tracked_position =  m_gorisontal.max_position/2;
            if ( m_gorisontal.position == m_gorisontal.tracked_position )
            {
                m_vertical.tracked_position   =  0;
                if ( m_vertical.position == m_vertical.tracked_position )
                {
                    neutral_gear_flag = false;
                }
            }
        }
    }



}

void shiftToFifthGear ( void )
{
    setTrackedMode ( 4000, 4000 );

    if ((m_vertical.position != m_vertical.max_position ) || (m_gorisontal.position != 0 ))
    {
        if ( !neutral_gear_flag )
        {
            shiftMTToNeutral ();
        }

        if ((m_vertical.position == m_vertical.max_position / 2) && (m_gorisontal.position == m_gorisontal.max_position / 2))
        {
            neutral_gear_flag = true;
        }
        if ( neutral_gear_flag )
        {
            m_gorisontal.tracked_position =  0;
            if ( m_gorisontal.position == m_gorisontal.tracked_position )
            {
                m_vertical.tracked_position   =  m_vertical.max_position;
                if ( m_vertical.position == m_vertical.tracked_position )
                {
                    neutral_gear_flag = false;
                }
            }
        }
    }

}

void shiftToReverseGear ( void )
{
    setTrackedMode ( 4000, 4000 );
   // neutral_gear_flag = false;

    if ((m_vertical.position != 0 ) || (m_gorisontal.position != 0 ))
    {
        if ( !neutral_gear_flag )
        {
            shiftMTToNeutral ();
        }

        if ((m_vertical.position == m_vertical.max_position / 2) && (m_gorisontal.position == m_gorisontal.max_position / 2))
        {
            neutral_gear_flag = true;
        }
        if ( neutral_gear_flag )
        {
            m_gorisontal.tracked_position =  0;
            if ( m_gorisontal.position == m_gorisontal.tracked_position )
            {
                m_vertical.tracked_position   =  0;
                if ( m_vertical.position == m_vertical.tracked_position )
                {
                    neutral_gear_flag = false;
                }
            }
        }
    }


}

#endif
