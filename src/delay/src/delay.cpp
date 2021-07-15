#include <ros/ros.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <zmq.h>
#include "../include/delay/delay.pb.h"
//#include "../include/delay/delay.pb.cc"
using namespace std;

char subaddr[30]="tcp://localhost:7050";
char pubaddr[30]="tcp://*:7050";

void* ctx = zmq_ctx_new();
void* subscriber = zmq_socket(ctx, ZMQ_SUB);
void* publisher = zmq_socket(ctx, ZMQ_PUB);

bool publishMsg(int uav_id, double lat, double lon, double alt, double vx, double vy, double vz){
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
    ROS_INFO("send message (%d bytes) success.",send_byte);
    zmq_msg_close(&send_msg);

    return true;
}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "delay");
    ros::NodeHandle nh;

    string nodeType("publish");
    ros::param::get("~node_type", nodeType);

    if(nodeType=="subscriber"){
        if(0 == zmq_connect(subscriber, subaddr)){
            ROS_INFO("subscriber connect success");//return 0 if success
        }else{
            perror("subscriber connect failed:");
            return -1;
        }
        zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "", 0);
    }
    else{
        if(0 == zmq_bind(publisher, pubaddr)){
            ROS_INFO("publisher bind success");//return 0 if success
        }else{
            perror("publisher bind failed:");
            return -1;
        }
    }

    delayMessage::DelayMsg delaymsg;
    ros::Rate loop_rate(1);

    if(nodeType == "publish"){
        while(ros::ok()){
            publishMsg(1,1.0,1.0,1.0,1.0,1.0,1.0);
            loop_rate.sleep();
        }
    }
    else{
        while(ros::ok()){
            delaymsg.Clear();
            zmq_msg_t recv_msg;
            zmq_msg_init(&recv_msg);
            int recv_byte = zmq_msg_recv(&recv_msg,subscriber,0);//ZMQ_DONTWAIT

            if(recv_byte > 0)
            {
                ROS_INFO("receive message (%d bytes) success.",recv_byte);

                string str;
                str.assign((char*)zmq_msg_data(&recv_msg),recv_byte);
                zmq_msg_close(&recv_msg);
                //    int index = str.find_first_of('\0');
                //    ROS_INFO("%d",index);
                //    delaymsg.ParseFromString(str.substr(0,index));
                delaymsg.ParseFromString(str);
                ROS_INFO("msg_id:%d send time:%.9lf",delaymsg.msg_id(),delaymsg.send_time());
                ROS_INFO("id:%d, lat:%f, lon:%f, alt:%f, vx:%f, vy:%f, vz:%f", delaymsg.uav_id(), delaymsg.lat(), delaymsg.lon(), delaymsg.alt(), delaymsg.vx(), delaymsg.vy(), delaymsg.vz());
            }    

            ros::spinOnce();
            loop_rate.sleep();
        }
    }

    zmq_close(subscriber);
    zmq_ctx_destroy(ctx);

    ros::shutdown();
    return 0;
}
