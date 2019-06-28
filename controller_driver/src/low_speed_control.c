#include <low_speed_control.h>

uint32_t map(double x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


static double res_buff[2] = {0.0,0.0};
static double VSpeed_e_prev = 0.0;
static double VSpeed_e = 0.0;
static double VSpeed_e_buf[20] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
static double VSpeed_e_sum = 0.0;
static double VSpeed_e_filter = 0.0;
static double VSpeed = 0.0;
static double dErr= 0.0;
static uint8_t  E_cnt = 0;
static double  vs = 0;
static bool fl_start_flag = 0;
static bool target_zone_flag = 0;
static uint8_t pedal_calibration_start_fag = 0;

static uint32_t max_clutch_pos     = 75000;
static uint32_t min_clutch_pos     = 50000;
static uint16_t max_break_pos     = 2000;
static uint16_t min_break_pos      = 0;

static uint32_t max_clutch_speed = 3500;
static uint32_t min_clutch_speed = 20000;
static uint32_t max_break_speed = 6000;
static uint32_t min_break_speed = 20000;

static THD_WORKING_AREA(fl_wa, 256);
static THD_FUNCTION(fl, arg) {
    (void)arg;
    /*Clutch and Breake speed*/
    /*Fuzzy logic controller*/
    bool Clutch_old = 0;
    bool Break_old = 0;
    fuzzylogicInit();

    while(1){

        if(fl_start_flag == 1){


                if(target_zone_flag == 0 ){

                    if(pedalsClutchGetPosition()> 72000)
                        pedalsClutchRelease( 1000 );
                    else
                        pedalsClutchRelease( 35000 );

                    if( gazelGetSpeed() >= 0.1){
                        max_clutch_pos = pedalsClutchGetPosition() + 7000;
                        min_clutch_pos = pedalsClutchGetPosition() -15000;
                        target_zone_flag = 1;
                    }
                }else{

                    VSpeed_e= vs - gazelGetSpeed();

                    dErr =  VSpeed_e - VSpeed_e_prev;

                    //calculate fl
                    calculateFLReg(VSpeed_e, dErr, &res_buff);
                    VSpeed_e_prev = VSpeed_e;



                if( (res_buff[0]<0) && (pedalsClutchGetPosition() >= min_clutch_pos)){
                    uint32_t cl_sp =  map(100 + res_buff[0], 0,100, max_clutch_speed, min_clutch_speed);
                    pedalsClutchRelease( cl_sp ); //100 + res_buff becouse max_clutch_speed < min_clutch_speed and res_buff < 0
                }else
                    if((res_buff[0]>0) && (pedalsClutchGetPosition() <= max_clutch_pos)){
                        uint32_t cl_sp =   map(100 - res_buff[0], 0,100, max_clutch_speed, min_clutch_speed);
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
                // else if(res_buff[1]>0)
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
            chThdSleepMilliseconds( 30 );
        
    }
}

void lowSpeedStart(void){
	fl_start_flag = 1;
};
void lowSpeedStop(void){
	fl_start_flag = 0;
};
/*
*	note: ref in km/h.
*/
void lowSpeedSetReference( double ref){
	vs = ref;
};

double lowSpeedGetErr(void){
	return VSpeed_e;
};


double lowSpeedGetdErr(void){
	return dErr;
};

void lowSpeedControlIntit(void){
    //Fuzzy logic controller
     fuzzylogicInit();
     chThdCreateStatic(fl_wa, sizeof(fl_wa), NORMALPRIO + 6, fl, NULL);
       
};
