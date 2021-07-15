#include <ros/ros.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <zmq.h>
#include "../include/delay/delay.pb.h"
//#include "../include/delay/delay.pb.cc"
#include <nav_msgs/Odometry.h>
using namespace std;

char pubaddr[30]="tcp://*:7050";

void* ctx = zmq_ctx_new();
void* publisher = zmq_socket(ctx, ZMQ_PUB);

int publishMsg(int uav_id, double lat, double lon, double alt, double vx, double vy, double vz){
    delayMessage::DelayMsg delaymsg;
    delaymsg.set_uav_id(uav_id);
    delaymsg.set_lat(lat);
    delaymsg.set_lon(lon);
    delaymsg.set_alt(alt);
    delaymsg.set_vx(vx);
    delaymsg.set_vy(vy);
    delaymsg.set_vz(vz);
    string send_str = delaymsg.SerializeAsString();

    //string转成zmq_msg_t
    zmq_msg_t send_msg;
    zmq_msg_init_size(&send_msg,send_str.length());
    memcpy(zmq_msg_data(&send_msg),send_str.c_str(),send_str.size());
    
    //发送zmq_msg_t
    int send_byte = zmq_msg_send(&send_msg,publisher,0);
    ROS_INFO("publish message (%d bytes) success.",send_byte);
    zmq_msg_close(&send_msg);

    return send_byte;
}

int publishMsg(delayMessage::DelayMsg delaymsg){
    string send_str = delaymsg.SerializeAsString();

    //string转成zmq_msg_t
    zmq_msg_t send_msg;
    zmq_msg_init_size(&send_msg,send_str.length());
    memcpy(zmq_msg_data(&send_msg),send_str.c_str(),send_str.size());
    
    //发送zmq_msg_t
    int send_byte = zmq_msg_send(&send_msg,publisher,0);
    ROS_INFO("publish message (%d bytes) success.",send_byte);
    zmq_msg_close(&send_msg);

    return send_byte;
}

void status_callback(const nav_msgs::Odometry& status){
    delayMessage::DelayMsg delaymsg;
    delaymsg.set_uav_id(1);
    delaymsg.set_lat(status.pose.pose.position.x);
    delaymsg.set_lon(status.pose.pose.position.y);
    delaymsg.set_alt(status.pose.pose.position.z);
    delaymsg.set_vx(status.twist.twist.linear.x);
    delaymsg.set_vy(status.twist.twist.linear.y);
    delaymsg.set_vz(status.twist.twist.linear.z);

    publishMsg(delaymsg);
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "publish_node");
    ros::NodeHandle nh;

    //ros::Subscriber statusSub = nh.subscribe("/delay/status",10,&status_callback);
    if(0 == zmq_bind(publisher, pubaddr)){
        ROS_INFO("publisher bind success");//return 0 if success
    }else{
        perror("publisher bind failed:");
        return -1;
    }

    delayMessage::DelayMsg delaymsg;
    ros::Rate loop_rate(1);
   
    while(ros::ok()){
        ros::spinOnce();
        loop_rate.sleep();
    }

    zmq_close(publisher);
    zmq_ctx_destroy(ctx);

    ros::shutdown();
    return 0;
}
