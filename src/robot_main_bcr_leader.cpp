// LG data aquisition code from eCAN module

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include <signal.h>
#include <chrono>
#include <thread>

#include <sys/select.h>  // For select()

// ROS
#include "std_msgs/Float64.h"
#include "ros/ros.h"
#include "bcr_leading/four.h"
#include "bcr_leading/two.h"
#include "std_msgs/Int16.h"
#include "std_msgs/Int32.h"
#include "std_msgs/Float64.h"

#include <arpa/inet.h> // For htonl and ntohl

const char *IP_ADDR = "192.168.111.25";
const int LISTEN_PORT = 4001;
const int BCR_UDP_PORT = 23;

int sock; // socket file descriptor
int opt = 1;

// --------------- System parameters --------------- //
int flag_enc = 1; // for ctrl+C
int bcr_mode = 0;
int flag = 0;
int bcr_flag_1 = 0;
int prv_mode= -1;
int bcr_check_1 = 0;
int bcr_check_2 = 0;
int bcr_check_3 = 0;
int detect_flag = 0;
int conv_mode = 0;
int cnt = 0; // counter
int msg_flag_101 = 0;
int msg_flag_102 = 0;
int prev_msg = 0;

int valread = 0;

 int timer_running = 0;
double ttimesec = 0;
double time_start =0;
double time_end = 0;
double ttime = 0;
double ttime_st = 0;
double ttime_bst = 0;

bool counting = false;        // To check if the timer is running
bool on_counting = false;
bool off_counting = false;

int buf_flag = 0;

void bcr_oepration(int op_mode);

void catch_signal(int sig)
{
    flag_enc = 0;
}




// -------------- remote receiver ---------//
void msgCallbackcmd(const bcr_leading::four::ConstPtr &msg)
{
    bcr_mode= msg->a;
    bcr_check_1 = msg->b;
    bcr_check_2 = msg->c;
    bcr_check_3 = msg->d;

    prv_mode = -1;
    prev_msg = 0;
    counting = false; 
    on_counting = false; 
    off_counting = false;

    valread = 0;
    buf_flag = 0;

     ttime = 0;
     ttime_st = 0;
     ttime_bst = 0;
    //flag++;
    std::cout << "receive!" << std::endl;
}



void bcr_oepration(int op_mode)
{
    if (bcr_mode == 1) 
                         
            {
                if(prv_mode != bcr_mode)
                {   
                    //command
                    std::string trigger_command = "||>trigger on\r\n";
                    send(sock, trigger_command.c_str(), trigger_command.length(), 0);
                    std::cout << "Sent trigger on command to server." << std::endl;
                    prv_mode = bcr_mode;

                    ssize_t bytes_sent = send(sock, trigger_command.c_str(), trigger_command.length(), 0);
                    if (bytes_sent < 0) {
                        std::cerr << "Failed to send: " << strerror(errno) << std::endl;
                    } else {
                        std::cout << "Sent trigger on command: " << bytes_sent << " bytes" << std::endl;
                    }
                }                
            
            } 
    else if (bcr_mode == 2) 
            {
                if(prv_mode != bcr_mode)
            {
            std::string trigger_command = "||>trigger off\r\n";
                send(sock, trigger_command.c_str(), trigger_command.length(), 0);
                std::cout << "Sent trigger on command to server." << std::endl;
                prv_mode = bcr_mode;

                ssize_t bytes_sent = send(sock, trigger_command.c_str(), trigger_command.length(), 0);
                    if (bytes_sent < 0) {
                        std::cerr << "Failed to send: " << strerror(errno) << std::endl;
                    } else {
                        std::cout << "Sent trigger on command: " << bytes_sent << " bytes" << std::endl;
                    }
                
            }
            } 
    else if (bcr_mode == 3) 
            {
                if(prv_mode != bcr_mode)
            {
            std::string trigger_command = "||>set illumination on\r\n";
                send(sock, trigger_command.c_str(), trigger_command.length(), 0);
                std::cout << "Sent trigger on command to server." << std::endl;
                prv_mode = bcr_mode;

                ssize_t bytes_sent = send(sock, trigger_command.c_str(), trigger_command.length(), 0);
                    if (bytes_sent < 0) {
                        std::cerr << "Failed to send: " << strerror(errno) << std::endl;
                    } else {
                        std::cout << "Sent trigger on command: " << bytes_sent << " bytes" << std::endl;
                    }
            }
            } 
    else if (bcr_mode == 4) 
            {
                if(prv_mode != bcr_mode)
            {
            std::string trigger_command = "||>set illumination off\r\n";
                send(sock, trigger_command.c_str(), trigger_command.length(), 0);
                std::cout << "Sent trigger on command to server." << std::endl;
                prv_mode = bcr_mode;

                ssize_t bytes_sent = send(sock, trigger_command.c_str(), trigger_command.length(), 0);
                    if (bytes_sent < 0) {
                        std::cerr << "Failed to send: " << strerror(errno) << std::endl;
                    } else {
                        std::cout << "Sent trigger on command: " << bytes_sent << " bytes" << std::endl;
                    }
            }
            } 
    else if (bcr_mode ==9)
            {
                if(prv_mode != bcr_mode)
                {
                    std::cout << "ready" << std::endl;
                }
                // // 사용자가 'q'를 입력하면 프로그램 종료
                // std::cout << "ready" << std::endl;
                // break;
            }
            else {
                // std::cout << "Invalid command. Try again." << std::endl;
                // continue;
            }
}
// --------------- MAIN --------------- //
int main(int argc, char *argv[])
{
    ros::init(argc, argv, "bcr_result_publisher");

    ros::NodeHandle nh;
    int sockfd = 0;
    int sockfd2 = 0;
    char buffer[1024] = {0};

	ros::Subscriber sub_command;
    sub_command = nh.subscribe("bcr_command", 10, &msgCallbackcmd);


    // ros::Publisher bcr_pub = nh.advertise<bcr_leading::two>("/bcr_result", 10);
    ros::Publisher remote_cmd_pub = nh.advertise<bcr_leading::two>("conv_remote_command", 100);
    
    ros::Rate loop_rate(1);


    signal(SIGTERM, catch_signal); // Termination
    signal(SIGINT, catch_signal);  // Active


    //creat socket file
    sock = socket(AF_INET, SOCK_STREAM, 0); //SOCK_DGRAM
    if (sock < 0)
    {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

    struct sockaddr_in BCRaddress;
    BCRaddress.sin_family = AF_INET;
    BCRaddress.sin_port = htons(BCR_UDP_PORT);
    
    //check server
    if (inet_pton(AF_INET, IP_ADDR, &BCRaddress.sin_addr) <= 0) {
        std::cerr << "Invalid address or Address not supported" << std::endl;
        return -1;
    }

    std::cout << "BCR Master IP: " << IP_ADDR << std::endl;
    std::cout << "BCR Master UDP PORT: " << BCR_UDP_PORT << std::endl;


    // 서버에 연결
    if (connect(sock, (struct sockaddr *)&BCRaddress, sizeof(BCRaddress)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return -1;
    }


    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
        std::cerr << "Failed to set socket timeout" << std::endl;
    }

    //counting
    auto start = std::chrono::high_resolution_clock::now();  // Timer start placeholder
    auto on_start = std::chrono::high_resolution_clock::now();  // Timer start placeholder
    auto off_start = std::chrono::high_resolution_clock::now();  // Timer start placeholder
   
    
   

    while (ros::ok())
    {
        if (flag_enc == 0)
        {
            break;
        }

        // when the msg '1' from robot, start barcoding 
        bcr_oepration(bcr_mode);

        // Set up the file descriptor set
        fd_set read_fds;
        FD_ZERO(&read_fds);            // Clear the set
        FD_SET(sock, &read_fds);       // Add the socket to the set
         // Initialize the timeout for each iteration

        if(bcr_mode == 1)
        {   
                if(detect_flag == 0)
                {
                    memset(buffer, 0, sizeof(buffer));
                    valread = read(sock, buffer, 1024);
                    if (valread > 0) {
                        std::cout << "Server response: " << buffer << std::endl;
                        buf_flag = 1;
                    } else {
                        std::cerr << "Read error or connection closed by server." << std::endl;
                        buf_flag = 0;
                    }
                }                   
 
                    
                if(buf_flag>0 && strlen(buffer)>2 && buffer[0] != '?' && buffer[1] != '?')
                {
                    detect_flag = 1;
                    
                }
                
                else
                {
                    detect_flag = 0;
                    
                }

              
            
            
        }

        else if(bcr_mode==2)
        {
            //step - 2 operation each case
                if(detect_flag == 0)
                {
                 if (!off_counting) 
                        {
                            // Start the timer when '1' is received for the first time
                            off_start = std::chrono::high_resolution_clock::now();
                            off_counting = true;
                            std::cout << "Timer started/reset.\n";
                        } 

                        if(off_counting)
                        {
                            //  // barcode trigger off
                            //     std::string trigger_command = "||>trigger off\r\n";
                            //     send(sock, trigger_command.c_str(), trigger_command.length(), 0);
                               
                            // // Timeout occurred
                            // std::cout << "No data received within 2 seconds." << std::endl;

                            auto now = std::chrono::high_resolution_clock::now();
                            std::chrono::duration<double> elapsed_seconds = now - off_start;
                            ttime_bst = elapsed_seconds.count();

                            // Check if 'a' should be printed (only once within the first 5 seconds)
                            if (ttime_bst < 2 && prev_msg != 101) 
                            {
                                bcr_leading::two msgp;
                                msgp.a = 101; // mode num
                                msgp.b = 0; // 
                        
                                remote_cmd_pub.publish(msgp);
                                prev_msg = 101;
                            }

                            // After 5 seconds, perform other actions or reset
                            if (ttime_bst >= 2 && prev_msg != 9) 
                            {
                               

                                std::cout << "2 seconds have passed.\n";
                                counting = false;  // Stop the timer
                                // off_counting = false; 
                                bcr_leading::two msgp;
                                msgp.a = 9; // mode num
                                msgp.b = 0; // 
                            
                                remote_cmd_pub.publish(msgp);
                                prev_msg = 9;
                                bcr_mode = 0; // stanby
                            }
                        }

                                                
                }

                else if(detect_flag == 1)
                {
                           
                 if (!on_counting) 
                            {
                                // Start the timer when '1' is received for the first time
                                on_start = std::chrono::high_resolution_clock::now();
                                on_counting = true;
                                std::cout << "Timer started/reset.\n";
                            } 

                            if(on_counting)
                            {
                                // Timeout occurred
                                std::cout << "Data received within 2 seconds." << std::endl;
                                auto now = std::chrono::high_resolution_clock::now();
                                std::chrono::duration<double> elapsed_seconds = now - on_start;
                                ttime = elapsed_seconds.count();

                                // Check if 'a' should be printed (only once within the first 5 seconds)
                                if (ttime < 2 && prev_msg != 102) 
                                {
                                    bcr_leading::two msgp;
                                    msgp.a = 102; // mode num
                                    msgp.b = 0; // 
                                
                                    remote_cmd_pub.publish(msgp);
                                    prev_msg = 102;
                                }

                                // After 2 seconds, perform other actions or reset
                                if (ttime >= 2 && prev_msg != 9) 
                                {
                                    std::cout << "2 seconds have passed.\n";
                                    counting = false;  // Stop the timer
                                    // on_counting = false;  // Stop the timer

                                    bcr_leading::two msgp;
                                    msgp.a = 9; // mode num
                                    msgp.b = 0; // 
                                
                                    remote_cmd_pub.publish(msgp);

                                    prev_msg = 9;
                                    bcr_mode = 0; // stanby
                                }
                            }

                        
                }
        }
        else
        {
            
        }
         


        // prv_mode = bcr_mode;

        
        ros::spinOnce();
    }

    close(sock);
    return 0;
}
