#ifndef _LLD_GYROSCOPE_H
#define _LLD_GYROSCOPE_H

#include <common.h>

// constants for conversion
#define OFFSET 1024
#define SENSITIVITY 819.0

typedef struct gyro_angles
{

    float x_angle_deg;// x angle (degrees)
    float y_angle_deg;// y angle (degrees)

} gyro_t;
 
void gyroInit( void );

void spiReceiveGyroXY(void);
float calculateGyroAngle(uint8_t* buf);

float getGyroX(void);
float getGyroY(void);


#endif
