#include <tests.h>
#include <chprintf.h>
#include <common.h>

#include <pedals.h>
#include <feedback.h>
#include <engine_ignition.h>
#include <MT.h>
#include <emergency_stop.h>
#include <speed.h>
#include <pid.h>
#include <ros_proto.h>
#include <fuzzy_logic.h>

static PIDControllerContext_t  pidCtxV = {
    .kp   = 5.0,
    .ki   = 0.01,
    .kd   = 0,
    .integrLimit  = 100,
    .integZone = 0.9
};

int8_t mt_shifting=-1;
static uint8_t flag_release_clutch=0;
static bool flag_start_shift = false;
static bool target_zone_flag=false;
int8_t flag_joystick=0;
float clutch_pos;
static uint8_t CSErrorDeadzoneHalfwidth = 1;
static bool brake_flag=1;
static int counter_ros=0;
static uint32_t max_clutch_pos     = 50000;
static uint32_t min_clutch_pos     = 25000;
static uint16_t max_break_pos     = 2000;
static uint16_t min_break_pos      = 0;

uint32_t cl_sp=0;

static uint16_t max_clutch_speed=6000;
static uint16_t min_clutch_speed=20000;
static uint32_t max_break_speed = 6000;
static uint32_t min_break_speed = 20000;

static double VSpeed = 0.0;
static double VSpeed_e = 0.0;
static double VSpeed_e_prev = 0.0;
static double dErr= 0.0;
static double res_buff[2] = {0.0,0.0};
double  vs = 4;
float steer_angle_rad=0;
static int32_t steer_angle = 0;

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};

static thread_reference_t trp_speed_control = NULL;
static thread_reference_t trp_start = NULL;
static thread_reference_t trp_px4flow = NULL;
static thread_reference_t trp_steer = NULL;
static bool start_enable_thr = false;
static bool speed_control_enable_thr = false;
static bool flag_stop=false;

float speedPIDVehicleControl( float speed )

{
    //speed = uint32_map(speed,0,100,0,1000);
    //int32_t current_speed = uint32_map(gazelGetSpeed(),0,100,0,1000);
    float error = speed - gazelGetSpeed();

    /* Dead zone for (p) error */
    if ( abs(error) < CSErrorDeadzoneHalfwidth )
    {
        pidCtxV.err = 0;
    }
    else
    {
        pidCtxV.err = error;
    }
    pidCtxV.integZone_abs = speed * pidCtxV.integZone;
    float VehicleControl    = PIDControlResponse( &pidCtxV );


    /*  roughly reset integral */
    if(VehicleControl>20000){
        VehicleControl = 20000;
    }else{
        if(VehicleControl<-20000){
            VehicleControl = -20000;
        }
    }
    if(VehicleControl>-5000 && VehicleControl<5000){
        if(VehicleControl<0){
            VehicleControl=-5000;
        }else{
            VehicleControl=5000;
        }
    }

    //VehicleControl = uint32_map(VehicleControl,-20000,20000,-100,100);
    //uint32_t cl_sp =   map(100 - res_buff[0], 0,100, max_clutch_speed, min_clutch_speed);

    return VehicleControl;
}

bool startBackward ( void )
{
    if (start_enable_thr == 0) {
        start_enable_thr = 1;
        /* Wake up engine start thread */
        chSysLock();
        chThdResumeS(&trp_start, MSG_OK);
        chSysUnlock();
    }
    //palClearLine(STARTER_PAL_LINE);

    return  ( !start_enable_thr );

}

bool startSpeedControl ( bool enable_thread )
{
    if (!enable_thread) {
       /* chSysLock();
        chThdSuspendS(&trp_speed_control);
        chSysUnlock();*/
        speed_control_enable_thr=0;
        /* Wake up engine start thread */

    }
    else{
        PIDControlInit( &pidCtxV );
       /* chSysLock();
        chThdResumeS(&trp_speed_control, MSG_OK);
        chSysUnlock();*/
        speed_control_enable_thr=1;
    }

    return  ( speed_control_enable_thr );

}



static THD_WORKING_AREA(speed_control_wa, 512);
static THD_FUNCTION(speed_control_thr, arg) {
    (void)arg;

    while(1){

        if(speed_control_enable_thr==1){

            palToggleLine(LINE_LED3);


            if(target_zone_flag == 0 ){
                if(pedalsClutchGetPosition()> 34000)
                    pedalsClutchRelease( 5000 );
                else
                    pedalsClutchRelease( 35000 );

                if( gazelGetSpeed()>=1.0){
                            target_zone_flag = 1;
                    //max_clutch_pos = pedalsClutchGetPosition() + 7000;
                    //min_clutch_pos = pedalsClutchGetPosition() -15000;
                }
            }else{

                VSpeed_e= vs - gazelGetSpeed_px4flow();
                dErr =  VSpeed_e - VSpeed_e_prev;
                //calculate fl
                calculateFLReg(VSpeed_e, dErr, &res_buff);
                VSpeed_e_prev = VSpeed_e;
                if( (res_buff[0]<0) && (pedalsClutchGetPosition() >= min_clutch_pos)){
                    cl_sp =  map(100 + res_buff[0], 0,100, max_clutch_speed, min_clutch_speed);
                    pedalsClutchRelease( cl_sp ); //100 + res_buff becouse max_clutch_speed < min_clutch_speed and res_buff < 0
                }else
                    if((res_buff[0]>0) && (pedalsClutchGetPosition() <= max_clutch_pos)){
                        cl_sp =   map(100 - res_buff[0], 0,100, max_clutch_speed, min_clutch_speed);
                        pedalsClutchPress ( cl_sp); //100 + res_buff becouse max_clutch_speed < min_clutch_speed and res_buff < 0
                    }else
                        pedalsClutchStop();
                if( (res_buff[1]<0) && (pedalsBrakeGetPosition() >= min_break_pos)){
                    uint32_t br_sp =  map(100 + res_buff[1], 0,100, max_break_speed, min_break_speed);
                    pedalsBrakeRelease( br_sp ); //100 + res_buff becouse max_break_speed < min_break_speed and res_buff < 0
                }else
                    if((res_buff[1]>0) && (pedalsBrakeGetPosition() <= max_break_pos)){

                        uint32_t br_sp =   map(100 - res_buff[1], 0,100, max_break_speed, min_break_speed);
                        pedalsBrakePress( br_sp); //100 + res_buff becouse max_break_speed < min_break_speed and res_buff < 0
                    }else
                        pedalsBrakeStop();
                }

                // if(res_buff[1]<0)
                //     MotorRunContinuous( &BreakM, 1, map(100 + res_buff[1], 0,100, max_break_speed, min_break_speed) ); //100 + res_buff becouse max_breake_speed < min_break_speed and res_buff < 0
                // else if(res
                //     MotorRunContinuous( &BreakM, 0, map(100 - res_buff[1], 0,100, max_break_speed, min_break_speed) ); //100 + res_buff becouse max_breake_speed < min_break_speed and res_buff < 0
                // else
                //     MotorStop(&BreakM);
                // }
            // }
        // }else{
            //MotorRunContinuous( &ClutchM, 0, 600);
        }else{
            target_zone_flag = 0;
        }

            chThdSleepMilliseconds( 20 );



    }
}

static THD_WORKING_AREA(emergency_stop_wa, 512);
static THD_FUNCTION(emergency_stop_thr, arg) {
    (void)arg;

    while(1){

        if(palReadPad(GPIOA,0)==0) {

//
            if(flag_stop==false){
                pedalsBrakePress( 1000 );
                while(pedalsBrakeGetState()){
                    chThdSleepMilliseconds( 50 );
                }

                pedalsClutchPress ( 1000 );
                while(pedalsClutchGetState()){
                    chThdSleepMilliseconds( 50 );
                }

                while ( mtControlMannualyShiftGear(0) !=0 ){
                    chThdSleepMilliseconds( 50 );
                }
                flag_stop=true;
                soundSignalStopContiniousSignals();
            }

        }
        chThdSleepMilliseconds( 200 );

    }
}


static THD_WORKING_AREA(mt_shift_wa, 512);
static THD_FUNCTION(mt_shift, arg)
{
    (void)arg;

    while (1)
    {
        if (mt_shifting != -1)
        {
            startSpeedControl(0);

            chThdSleepSeconds(1);
            while (pedalsClutchGetPosition() != pedalsClutchGetMaxPosition())
            {
                pedalsClutchPress( 1000 );
                chThdSleepMilliseconds(20);

            }

            while (pedalsBrakeGetPosition() != pedalsBrakeGetMaxPosition())
            {
                pedalsBrakePress( 2000 );
                chThdSleepMilliseconds(20);

            }


            //chprintf( (BaseSequentialStream *)&SD3, " cl:%d\n\r",pedalsClutchGetPosition());
            if (pedalsClutchGetPosition() == pedalsClutchGetMaxPosition())
            {
                switch (mt_shifting)
                {
                case 0:
                    mtControlMannualyShiftGear(0);
                    break;

                case 1:
                    mtControlMannualyShiftGear(1);
                    break;

                case 6:
                    mtControlMannualyShiftGear(6);
                    break;
                }
                if (mtControlGetCurrentGearNum() == mt_shifting)
                {
                    mt_shifting = -1;
                    //flag_release_clutch=1;
                    while (pedalsBrakeGetPosition() != 0)
                    {
                        pedalsBrakeRelease( 2000 );
                        chThdSleepMilliseconds(20);

                    }

                    if(vs>0){startSpeedControl(1);}



                }

            }
        }

        chThdSleepMilliseconds(100);
    }
}

static THD_WORKING_AREA(start_wa, 512);
static THD_FUNCTION(start_thr, arg) {
    (void)arg;

    while(1){
        if ( start_enable_thr == false ){
            chSysLock();
            chThdSuspendS(&trp_start);
            chSysUnlock();
        }
        else{



            while(engStarterSwitchOn()){

                chThdSleepMilliseconds( 50 );

            }



           //mt_shifting=1;

          /* while(flag_release_clutch!= 1){


               chThdSleepMilliseconds( 50 );

           }*/

           /*while (pedalsBrakeGetPosition() != 0)
           {
               pedalsBrakeRelease( 2000 );
               chThdSleepMilliseconds(20);

           }
           pedalsClutchMove(68000,25000);*/

           /*while(pedalsClutchGetPosition() > 1000){
              chThdSleepMilliseconds( 50 );
          }*/

           //flag_release_clutch = 0;


           start_enable_thr = false;


        }

//

        chThdSleepMilliseconds( 50 );

    }
}

static THD_WORKING_AREA(px4flow_wa, 512);
static THD_FUNCTION(px4flow_thr, arg) {
    (void)arg;

    while(1){

        px4_filter();
        chThdSleepMilliseconds( 20 );

    }
}

static THD_WORKING_AREA(steer_wa, 512);
static THD_FUNCTION(steer_thr, arg) {
    (void)arg;

    while(1){

        if(flag_joystick==1){
            steer_angle=double_map((float)steer_angle_rad,-10.472,10.472,-10000.0,10000.0);
            //chprintf( (BaseSequentialStream *)&SD3, ">>2 speed:%f angle:%d\n\r",vs,steer_angle);
            //chprintf( (BaseSequentialStream *)&SD3, ">>2 cl_pos:%d angle:%d\n\r",pedalsClutchGetPosition(),steer_angle);
            lldSteerSMSetPosition (steer_angle, 20000 );
            pedalsClutchMove((uint32_t)clutch_pos,12500);
            flag_joystick=0;

        }
        chThdSleepMilliseconds( 20 );

    }
}


void testBackwardMoving ( void )
{

    uint8_t sd_buff[10];

    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX


    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );   //Led

    engIgnitionInit();
    emergencyStopInit();
    pedalsInit();
    feedbackInit();
    mtControlInit();
    fuzzylogicInit();
    px4flowInit();
    lldSteerSMInit();
    soundSignalInit();
    rosInit(NORMALPRIO+15);
    //speedInit();
      chThdCreateStatic(emergency_stop_wa, sizeof(emergency_stop_wa), NORMALPRIO, emergency_stop_thr, NULL);//prio+15
      chThdCreateStatic(start_wa, sizeof(start_wa), NORMALPRIO, start_thr, NULL);
      chThdCreateStatic(mt_shift_wa, sizeof(mt_shift_wa), NORMALPRIO, mt_shift, NULL);
      //chThdCreateStatic(speed_control_wa, sizeof(speed_control_wa), NORMALPRIO, speed_control_thr, NULL);
      chThdCreateStatic(px4flow_wa, sizeof(px4flow_wa), NORMALPRIO+1, px4flow_thr, NULL);
      chThdCreateStatic(steer_wa, sizeof(steer_wa), NORMALPRIO, steer_thr, NULL);

    chprintf( (BaseSequentialStream *)&SD3, "Start\n\r");

    while(1) {
        /*if(brake_flag){
            brake_flag=0;
            pedalsBrakePress(4000);
        }*/
        //startBackward();

        sdReadTimeout( &SD3, sd_buff, 9, TIME_IMMEDIATE );

if(counter_ros++>20){

    gazel_ros_send_state_t state = {
            .linear_speed = 12,
            .steering_angle = 0.1
        };
    rosSendState(state);
    counter_ros=0;
    //chprintf( (BaseSequentialStream *)&SD3, "cl pos:%d br pos:%d vert:%d reg_val:%d speed:%f cl sp:%d rot:%f vs:%f steer%d\n\r",pedalsClutchGetPosition(),pedalsBrakeGetPosition(), getVerticalPosition(),cl_sp,gazelGetSpeed(),cl_sp,gazelGetEngineSpeed(),vs,steer_angle);

}

        if (sd_buff[0]=='z') pedalsBrakeMove(20000,4000);
        if (sd_buff[0]=='s') startBackward();
        if (sd_buff[0]=='w') shiftMTToNextGear(1,2000);//mt_shifting=1;
        if (sd_buff[0]=='y') shiftMTToNextGear(6,2000);//mt_shifting=1;
        //chprintf( (BaseSequentialStream *)&SD3, ">>2 speed:%f angle:%d\n\r",vs,steer_angle);
        chprintf( (BaseSequentialStream *)&SD3, "cl_pos:%d angle:%d\n\r",(uint32_t)clutch_pos,steer_angle);
        if (sd_buff[0]=='b') mt_shifting=6;
        if (sd_buff[0]=='n') mt_shifting=0;
        if (sd_buff[1]=='v') vs=atoi(sd_buff[0]);
        if (sd_buff[0]=='w') mt_shifting=1;
        if (sd_buff[6]=='e') pedalsClutchMove(atoi(sd_buff),25000);
        if (sd_buff[6]=='m') pedalsBrakeMove(atoi(sd_buff),10000);
        if (sd_buff[5]=='a') {

           //flag_rul=1;
        }
        if (sd_buff[0]=='f') emergencyFullStop();
        if (sd_buff[0]=='r') {
            pedalsClutchRelease(1000);pedalsBrakeRelease(1000);
        }
        //chprintf( (BaseSequentialStream *)&SD3, ">>3 speed:%f angle:%d\n\r",vs,steer_angle);
        //chprintf( (BaseSequentialStream *)&SD3, "steer:%d pos%d flag%d\n\r",steer_angle,lldSteerSMGetPosition(),flag_rul);
          //chprintf( (BaseSequentialStream *)&SD3, "rot:%f\n\r",gazelGetEngineSpeed());
        //chprintf( (BaseSequentialStream *)&SD3, "speed:%f angle:%d\n\r",vs,steer_angle_rad);

        for (int i = 0; i < 9; i++)
        {
            sd_buff[i]='?';
        }
        
        chThdSleepMilliseconds( 50 );
    }
}
