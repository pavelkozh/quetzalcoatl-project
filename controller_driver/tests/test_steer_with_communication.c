#include <ch.h>
#include <hal.h>
#include <communication.h>
#include <lld_steer.h>

#include <steer_control.h>


// void changeSpeedSteer(comm_speed_t speed, comm_steer_t angle)
// {
//     // palToggleLine(LINE_LED1);
//     if(speed < 0){
//         speed = speed * (-1);
//     }
//     comm_dbgprintf_info("I m in steerMotorSetSpeed, im get %d \n", speed);
    
//     steerMotorSetSpeed((float) speed);
    
// }

void changeSpeedSteer(comm_speed_t angle, comm_steer_t speed)
{
    // palToggleLine(LINE_LED1);
 
    comm_dbgprintf_info("I m in steerMotorSetSpeed %d a %d \n", angle, speed);
    
    steerSetPosition((float) angle);
    
}

void setStartSteer(void)
{
    // palToggleLine(LINE_LED2);
    // uint16_t potenc = (uint16_t)(steerGetPosition()*100);
    comm_dbgprintf_info("I m in setStartSteer \n");
     
    steerControlStart(); //pid

    // steerMotorStartStopControl();

}



void setDirection(void){
    // palToggleLine(LINE_LED3);
    comm_dbgprintf_info("I m in setDirection \n");
    // steerMotorDirChange();

    steerControlStop(); // pid
}

void testCommunicationWithSteer(void)
{
    communicationEventFun_t structForFunc = getDefaultCfg();
 
    structForFunc.on_set    = changeSpeedSteer;
    structForFunc.on_start  = setStartSteer;
    structForFunc.on_stop   = setDirection;

    structForFunc.on_interrupt_timer = NULL;
    
    uint32_t time_for_vt_MS = 2000;

    comm_init(&structForFunc, time_for_vt_MS, true);

    // steerEncInit();
    // steerMotorInit();
    steerInit();
    // palToggleLine(LINE_LED1);

    while ( true )

    {   
        // palToggleLine(LINE_LED3);
        // comm_dbgprintf_info("PosRef %.02f\t Encoder %d\t PosErr %.02f\t PidOut %.02f\t MotorEn %d\t \n\r",steerDbgGetMotorPosRef(),(uint16_t)(steerGetPosition()), steerDbgGetMotorCalcPosErr(),steerDbgGetMotorCalcSpeedRef(), steerDbgGetEnableFlag());
        comm_dbgprintf_info("Ref %.02f\t Enc %d\t  ", steerDbgGetMotorPosRef(),(int16_t)(steerGetPos()));
        comm_dbgprintf_info("Err %.02f\t Out %.02f\t ", steerDbgGetMotorCalcPosErr(),steerDbgGetMotorCalcSpeedRef());
        comm_dbgprintf_info("MotorEn %d Motordir:%d\n", steerDbgGetEnableFlag(),steerMotorDirection());
        // comm_dbgprintf_info("PosRef %d  \n",steerDbgGetEnableFlag());
        // comm_dbgprintf_info("I m in setStartSteer \n");
      
        chThdSleepMilliseconds(200);
    }
}
