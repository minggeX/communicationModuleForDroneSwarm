#include <ros/ros.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <zmq.h>
#include "../include/delay/delay.pb.h"
//#include "../include/delay/delay.pb.cc"
#include <nav_msgs/Odometry.h>
#include "../include/delay/config.h"
using namespace std;

void* ctx = zmq_ctx_new();
void* client = zmq_socket(ctx, ZMQ_REQ);
string uavName("uav");

int uavIndex;

int sendMsg(delayMessage::DelayMsg delaymsg){
    string send_str = delaymsg.SerializeAsString();

    //string转成zmq_msg_t
    zmq_msg_t send_msg;
    zmq_msg_init_size(&send_msg,send_str.length());
    memcpy(zmq_msg_data(&send_msg),send_str.c_str(),send_str.size());
    
    //发送zmq_msg_t
    int send_byte = zmq_msg_send(&send_msg,client,0);
    ROS_INFO("%s client send message (%d bytes) success.",uavName.c_str(), send_byte);
    zmq_msg_close(&send_msg);

    zmq_msg_t recv_msg;
    zmq_msg_init(&recv_msg);
    zmq_msg_recv(&recv_msg,client,0);

    return send_byte;
}

void status_callback(const nav_msgs::Odometry& status){
    delayMessage::DelayMsg delaymsg;
    delaymsg.set_uav_id(uavIndex);
    delaymsg.set_send_time(ros::Time::now().toSec());
    delaymsg.set_lat(status.pose.pose.position.x);
    delaymsg.set_lon(status.pose.pose.position.y);
    delaymsg.set_alt(status.pose.pose.position.z);
    delaymsg.set_vx(status.twist.twist.linear.x);
    delaymsg.set_vy(status.twist.twist.linear.y);
    delaymsg.set_vz(status.twist.twist.linear.z);

    sendMsg(delaymsg);
}


int main(int argc, char** argv)
{
    ros::init(argc, argv, "client_node");
    ros::NodeHandle nh;

    ros::param::get("~uav_name", uavName);
    ros::param::get("~uav_index", uavIndex);

    if(0 == zmq_connect(client, connect_addr_list[uavIndex])){
        ROS_INFO("%s client connect success", uavName.c_str());//return 0 if success
    }else{
        perror(string(uavName+"client connect failed:").c_str());
        return -1;
    }

    // if(0 == zmq_connect(client, server_addr)){
    //     ROS_INFO("%s client connect success", uavName.c_str());//return 0 if success
    // }else{
    //     perror(string(uavName+"client connect failed:").c_str());
    //     return -1;
    // }

    ros::Subscriber statusSub = nh.subscribe(uavName+"/delay/status",10,&status_callback);

    delayMessage::DelayMsg delaymsg;
    ros::Rate loop_rate(10);

    while(ros::ok()){
        ros::spinOnce();
        loop_rate.sleep();
    }

    zmq_close(client);
    zmq_ctx_destroy(ctx);

    ros::shutdown();
    return 0;

}