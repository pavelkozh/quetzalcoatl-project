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

void mtControlInit ( void )
{
    /*Motor driver Setting */
    palSetLineMode( PAL_LINE( GPIOF, 7),  PAL_MODE_ALTERNATE(3) );
    palSetLineMode( m_vertical.dir_line, PAL_MODE_OUTPUT_PUSHPULL);

    MotorlldControlInit( &m_vertical );

    palSetLineMode( PAL_LINE( GPIOF, 9),  PAL_MODE_ALTERNATE(9) );
    palSetLineMode( m_gorisontal.dir_line, PAL_MODE_OUTPUT_PUSHPULL);
    MotorlldControlInit( &m_gorisontal );

    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led 2
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
static bool neutral_gear_flag = false;

void shiftMTToNeutral ( void )
{
    setTrackedMode ( 4000, 4000 );
    m_vertical.tracked_position   =  m_vertical.max_position / 2;
    if ( m_vertical.position == m_vertical.tracked_position)
    {
        m_gorisontal.tracked_position =  m_gorisontal.max_position / 2;
    }

}

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

