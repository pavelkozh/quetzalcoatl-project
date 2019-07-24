#include <steer_control.h>
#include <lld_steer.h>
#include <feedback.h>
#include <pid.h>
#include <errno.h>
#include <math.h>

static double steer_control_val = 0.0;
static float speed_ref = 0.0,
             prev_speed_ref = 0.0,
             steer_angle_ref = 0.0;
static bool steer_control_start = false,
            steer_move_to_the_right = false,
            steer_move_to_the_left = false;

static PIDControllerContext_t steer_pidCtx = {
    .kp = 0.2,
    .ki = 0.0,
    .kd = 0.0,
    .integrLimit = 0.2,
    .integZone = 0.3};
static uint8_t CSErrorDeadzoneHalfwidth = 1;

static float position;
static float prev_position;
static int8_t position_cnt;
float steerPosControl(float steer_angle_ref)
{
    float error = steer_angle_ref - position;

    /* Dead zone for (p) error */
    if (abs(error) < CSErrorDeadzoneHalfwidth)
    {
        steer_pidCtx.err = 0;
    }
    else
    {
        steer_pidCtx.err = error;
    }
    steer_pidCtx.integZone_abs = steer_angle_ref * steer_pidCtx.integZone;
    steer_control_val = PIDControlResponse(&steer_pidCtx);

    /*  roughly reset integral */
    steer_control_val = CLIP_VALUE(steer_control_val, -10.0, 10.0);

    return steer_control_val;
}

static void steerPositionCalculate(void)
{
    float c_position = steerGetPosition();
    if (position_cnt == 0)
    {
        if (c_position > 180)
            c_position -= 360;
    }
    else
    {
        c_position += position_cnt * 360.0;
    }

    if (c_position - prev_position < -220)
    {
        position_cnt++;
        c_position += 360.0;
    }

    if (c_position - prev_position > 220)
    {
        position_cnt--;
        c_position -= 360.0;
    }
    /* Update global variable */
    position = prev_position = c_position;
}

// TODO: Engine speed enable

static THD_WORKING_AREA(steer_pos_control_wa, 512);
static THD_FUNCTION(steer_pos_control, arg)
{
    (void)arg;
    while (1)
    {
        // palToggleLine(LINE_LED2);
        steerPositionCalculate();

        if (steer_control_start)
        {

            speed_ref = steerPosControl(steer_angle_ref);

            if (((speed_ref < 0) && (!steerMotorGetDirection() /* CCW */)) ||
                ((speed_ref > 0) && (steerMotorGetDirection() /* CW */)))
            {
                steerMotorDirChange();
            }

            steerMotorSetSpeed(abs(speed_ref));

            prev_speed_ref = speed_ref;
        }
        else
        {
            steer_pidCtx.err = 0;
            steer_pidCtx.prevErr = 0;
            steer_pidCtx.integrSum = 0;
            speed_ref = 0;
            // //Motor stop if he run
            // if(prev_position != position){
            //     steerMotorStartStopControl();
            //     steerMotorEnableInvert();
            // }
        }

        chThdSleepMilliseconds(100);
    }
}


#define STEER_RIGHT_POS_LIMIT -290 //deg
#define STEER_LEFT_POS_LIMIT 289

static THD_WORKING_AREA(steer_control_wa, 512);
static THD_FUNCTION(steer_control, arg)
{
    (void)arg;
    while (1)
    {
        palToggleLine(LINE_LED2);
        steerPositionCalculate();

        if (steer_control_start)
        {
//            if (((position >= STEER_RIGHT_POS_LIMIT) && steer_move_to_the_right)|| (( position <= STEER_LEFT_POS_LIMIT ) && steer_move_to_the_left)) // position out of the range
//            {
//                steerMotorSetSpeed(0);
//                chThdSleepMilliseconds(20);
//                continue;
//                // steerStop?
//            }
//            else{
                if (( (!steerMotorGetDirection() /* CCW */) && steer_move_to_the_right) ||
                       ((steerMotorGetDirection() /* CW */) && steer_move_to_the_left))
                {
                    steerMotorDirChange();
                }

//            }
        }
        else{
            steerMotorSetSpeed(0);
        }

        chThdSleepMilliseconds(200);
    }
}

static bool is_initialized = false;

void steerInit(void)
{
    if (is_initialized)
    {
        return;
    }

    PIDControlInit(&steer_pidCtx);

    steerEncInit();

    steerMotorInit();
    position = steerGetPosition();
    if (position > 180)
        position -= 360;
    position_cnt = 0;
    prev_position = position;

    chThdCreateStatic(steer_pos_control_wa,
                      sizeof(steer_pos_control_wa),
                      NORMALPRIO,
                      steer_pos_control,
                      NULL);

//    chThdCreateStatic(steer_control_wa,
//                          sizeof(steer_control_wa),
//                          NORMALPRIO,
//                          steer_control,
//                          NULL);

    is_initialized = true;
}

void steerControlStart(void)
{
    steer_control_start = true;
    if (!steerIsMotorEnable())
    {
        steerMotorStartStopControl();
    }
}

void steerControlStop(void)
{
    steer_control_start = false;
    if (steerIsMotorEnable())
    {
        steerMotorStartStopControl();
    }
    else
    {
        steer_pidCtx.err = 0;
        steer_pidCtx.prevErr = 0;
        steer_pidCtx.integrSum = 0;
        speed_ref = steerGetPosition();
    }
}

//TODO write correct values (from encoder)
#define STEER_POS_MAX_VAL 200.0
#define STEER_POS_MIN_VAL -200.0

void steerSetPosition(float val)
{
    if (val > STEER_POS_MAX_VAL)
        val = STEER_POS_MAX_VAL;
    if (val < -STEER_POS_MIN_VAL)
        val = -STEER_POS_MIN_VAL;
    steer_angle_ref = val;
}

float steerGetPos(void)
{
    return position;
}

void steerSetMoveToTheRight ( void )
{
    steer_move_to_the_right = true;
    steer_move_to_the_left = false;
    steerMotorSetSpeed( 2.5 );
}
void steerSetMoveToTheLeft ( void )
{
    steer_move_to_the_right = false;
    steer_move_to_the_left = true;
    steerMotorSetSpeed( 2.5 );
}
void steerSetStop ( void )
{
    //steer_move_to_the_right = false;
    //steer_move_to_the_left = false;
    steerMotorSetSpeed( 0.1 );
}



/**DBG functions**/

float steerDbgGetMotorCalcSpeedRef(void)
{
    return speed_ref;
}

float steerDbgGetMotorCalcPosErr(void)
{
    return steer_pidCtx.err;
}

float steerDbgGetMotorPosRef(void)
{
    return steer_angle_ref;
}

bool steerDbgGetEnableFlag(void)
{
    return steerIsMotorEnable();
}

bool steerDbgGetDir(void)
{
    return steerMotorGetDirection();
}

int pos_cnt(void)
{
    return position_cnt;
}


