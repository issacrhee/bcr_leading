#include <iostream>
#include <cctype>

#include <termios.h>
#include <unistd.h>

#include <ros/ros.h>
#include "std_msgs/Int16.h"
#include "std_msgs/Int32.h"
#include "bcr_leading/four.h"

int rm_mode[] = {0,0,0,0};
double cccount = 0;
int main(int argc, char **argv)
{
    ros::init(argc, argv, "bcr_commandd");

    ros::NodeHandle nh;

    ros::Publisher remote_cmd_pub = nh.advertise<bcr_leading::four>("bcr_command", 100);
    
    ros::Rate loop_rate(10);
    
    std_msgs::Int32 motor_num_current_msg;
    std_msgs::Int16 target_Current_msg;
    std_msgs::Int16 initializing_msg;
    

    int32_t input_motor_num_current;
    int16_t input_target_Current;
    int16_t input_initializing_motor;
    int16_t input_target_Position;
    int16_t input_angle_of_rotation;

    while (ros::ok())
    {
        std::cout << "Select Mdoe(9: stop, 1: bcr_on, 2: bcr_off :" << ", quit: -1)" << std::endl;
        std::cin >> rm_mode[0];
       
        if(!std::cin)
        {
            std::cout << "Wrong Input, try again." << std::endl;
            std::cin.clear();
            std::cin.ignore(INT_MAX, '\n');
            continue;
        }
        else if(rm_mode[0] == -1)
        {
            rm_mode[0] = 9;

             bcr_leading::four msgp;
                msgp.a = rm_mode[0]; // mode num
                // msgp.b = rm_mode[1]; // length
                // msgp.c = rm_mode[2]; // angle
                // msgp.d = rm_mode[3]; // finger stroke
                
                remote_cmd_pub.publish(msgp);
           
            printf("Quit Program...\n");
            break;
        }
         else if( rm_mode[0] == 1 )
        {
            printf("bcr_on mode\n");
            bcr_leading::four msgp;
                msgp.a = rm_mode[0]; // mode num
                // msgp.b = rm_mode[1]; // length
                // msgp.c = rm_mode[2]; // angle
                // msgp.d = rm_mode[3]; // finger stroke
                
                remote_cmd_pub.publish(msgp);
	       
        }
         else if( rm_mode[0] == 2 )
        {
            printf("bcr_off mode \n");
            bcr_leading::four msgp;
                msgp.a = rm_mode[0]; // mode num
                // msgp.b = rm_mode[1]; // length
                // msgp.c = rm_mode[2]; // angle
                // msgp.d = rm_mode[3]; // finger stroke
                
                remote_cmd_pub.publish(msgp);

	        continue;
        }
         else if( rm_mode[0] == 3 )
        {
            printf("illumination on mode \n");
            bcr_leading::four msgp;
                msgp.a = rm_mode[0]; // mode num
                // msgp.b = rm_mode[1]; // length
                // msgp.c = rm_mode[2]; // angle
                // msgp.d = rm_mode[3]; // finger stroke
                
                remote_cmd_pub.publish(msgp);

	        continue;
        }
         else if( rm_mode[0] == 4 )
        {
            printf("illumination off mode \n");
            bcr_leading::four msgp;
                msgp.a = rm_mode[0]; // mode num
                // msgp.b = rm_mode[1]; // length
                // msgp.c = rm_mode[2]; // angle
                // msgp.d = rm_mode[3]; // finger stroke
                
                remote_cmd_pub.publish(msgp);

	        continue;
        }
         else if( rm_mode[0] == 9 )
        {
            printf("all stop \n");
            bcr_leading::four msgp;
                msgp.a = rm_mode[0]; // mode num
                // msgp.b = rm_mode[1]; // length
                // msgp.c = rm_mode[2]; // angle
                // msgp.d = rm_mode[3]; // finger stroke
                
                remote_cmd_pub.publish(msgp);
	        continue;
        }

        else
        {
            printf("Wrong Input, try again.");
            continue;
        }

        ros::spinOnce();
    }

    return 0;
}
