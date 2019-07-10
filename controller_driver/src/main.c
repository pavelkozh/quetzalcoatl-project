#include <common.h>
#include <tests.h>
#include <chprintf.h>
#include <speed.h>
#include <emergency_stop.h>


static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};

void onSet( float speed, float angle ) {
    if (speed < 1.0 && speed > -1.0) {
       // emergen
    }
    speedSetVehiclePIDReferenceValue( speed );
    //steerSet...


}

void onStart( void ) {
    chprintf( (BaseSequentialStream *)&SD3,"In function onStart\n\r" );
}

void onStop( void ) {
    chprintf( (BaseSequentialStream *)&SD3,"In function onStop\n\r" );
}




static double _speed = 0.0;

static int8_t speed_state = 0 ;
bool start( void ){

    if(mtControlGetCurrentGearNum() != 1){
        if(pedalsClutchGetPosition() <= 90000)
            pedalsClutchPress(1000);
        else{
            mtControlMannualyShiftGear(1);
            speed_state = 0;
        }
    }else{
        switch(speed_state){
            case 0: 
                if(pedalsClutchGetPosition()>75000)
                    pedalsClutchRelease(1000);
                else
                    pedalsClutchRelease(15000);
                if(_speed>1.5)
                    speed_state = -1;
                break;

            case 1: 
                    pedalsClutchRelease(4000);
                    return 1;
                break;

            case -1:
                    pedalsClutchRelease(20000);
                    if(_speed>4.5)
                        speed_state = 1;
                break;
            default: break;
        }
    }
    return 0;
};

int main(void)
{
    chSysInit();
    halInit();

    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX

    feedbackInit();
    pedalsInit();
    mtControlInit();

    char  sd_buff[10] = {'?','?','?','?','?','?','?','?','?','?'} ;
    bool start_flag = 0;
    int8_t state = 0;
    #if (MAIN_PROGRAM_ROUTINE != PROGRAM_ROUTINE_MASTER) // need fixing

        testsRoutines();

    #else

while(1){
    sdReadTimeout( &SD3, sd_buff, 10, TIME_IMMEDIATE   );

    if(sd_buff[0] == 's') _speed = atoi(&sd_buff[1])/10.0;
    if(sd_buff[0] == 'a') start_flag = 1;
    if(sd_buff[0] == 'o') start_flag = 0;
    if(sd_buff[0] == 'q') mtControlMannualyShiftGear(0);
    if(sd_buff[0] == 'w') mtControlMannualyShiftGear(1);
    if(sd_buff[0] == 'e') mtControlMannualyShiftGear(2);
    if(start_flag)
        state = start();
    else{
        state = -1;
        //mtControlMannualyShiftGear(0);
        pedalsClutchStop();
    }

    chprintf( (BaseSequentialStream *)&SD3,"state:\t%d\tCl_pos:\t%d\tspeed\t%.2f\tgear: %d\n\r",state,pedalsClutchGetPosition(),_speed,mtControlGetCurrentGearNum());
    for (int i = 0; i < 9; i++)
    {
      sd_buff[i]='?';
    }
    chThdSleepMilliseconds(100);
}


    #endif

}
