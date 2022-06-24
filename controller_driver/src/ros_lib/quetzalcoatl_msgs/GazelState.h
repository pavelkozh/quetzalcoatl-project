#ifndef _ROS_quetzalcoatl_msgs_GazelState_h
#define _ROS_quetzalcoatl_msgs_GazelState_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "ros/time.h"

namespace quetzalcoatl_msgs
{

  class GazelState : public ros::Msg
  {
    public:
      typedef ros::Time _stamp_type;
      _stamp_type stamp;
      typedef float _linear_speed_type;
      _linear_speed_type linear_speed;
      typedef float _angle_steering_type;
      _angle_steering_type angle_steering;

    GazelState():
      stamp(),
      linear_speed(0),
      angle_steering(0)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const override
    {
      int offset = 0;
      *(outbuffer + offset + 0) = (this->stamp.sec >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->stamp.sec >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (this->stamp.sec >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (this->stamp.sec >> (8 * 3)) & 0xFF;
      offset += sizeof(this->stamp.sec);
      *(outbuffer + offset + 0) = (this->stamp.nsec >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->stamp.nsec >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (this->stamp.nsec >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (this->stamp.nsec >> (8 * 3)) & 0xFF;
      offset += sizeof(this->stamp.nsec);
      union {
        float real;
        uint32_t base;
      } u_linear_speed;
      u_linear_speed.real = this->linear_speed;
      *(outbuffer + offset + 0) = (u_linear_speed.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_linear_speed.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_linear_speed.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_linear_speed.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->linear_speed);
      union {
        float real;
        uint32_t base;
      } u_angle_steering;
      u_angle_steering.real = this->angle_steering;
      *(outbuffer + offset + 0) = (u_angle_steering.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_angle_steering.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_angle_steering.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_angle_steering.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->angle_steering);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer) override
    {
      int offset = 0;
      this->stamp.sec =  ((uint32_t) (*(inbuffer + offset)));
      this->stamp.sec |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      this->stamp.sec |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      this->stamp.sec |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      offset += sizeof(this->stamp.sec);
      this->stamp.nsec =  ((uint32_t) (*(inbuffer + offset)));
      this->stamp.nsec |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      this->stamp.nsec |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      this->stamp.nsec |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      offset += sizeof(this->stamp.nsec);
      union {
        float real;
        uint32_t base;
      } u_linear_speed;
      u_linear_speed.base = 0;
      u_linear_speed.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_linear_speed.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_linear_speed.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_linear_speed.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->linear_speed = u_linear_speed.real;
      offset += sizeof(this->linear_speed);
      union {
        float real;
        uint32_t base;
      } u_angle_steering;
      u_angle_steering.base = 0;
      u_angle_steering.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_angle_steering.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_angle_steering.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_angle_steering.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->angle_steering = u_angle_steering.real;
      offset += sizeof(this->angle_steering);
     return offset;
    }

    virtual const char * getType() override { return "quetzalcoatl_msgs/GazelState"; };
    virtual const char * getMD5() override { return "2524b384aab21e5e75cba5953b6f2c7b"; };

  };

}
#endif
