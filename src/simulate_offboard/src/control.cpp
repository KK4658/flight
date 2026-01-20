#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>  
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Quaternion.h>
#include "tf/transform_listener.h"
geometry_msgs::PoseStamped my_pose;
double drone_roll,drone_pitch,drone_yaw;

double get_distance(double point1,double point2){
return abs(point1-point2);
}

ros::Subscriber pose_sub = nh.subscribe<geometry_msgs::PoseStamped>("/mavros/local_position/pose", 200, pose_sub_cb);
geometry_msgs::PoseStamped aim_pose_msg;


/// @brief 这个函数是接受飞机位姿的回调函数,其位姿信息放在my_pose和drone_roll...里
/// @param pose_msg 
void pose_sub_cb(const geometry_msgs::PoseStampedConstPtr pose_msg){
drone_pose=*pose_msg;
 tf::Quaternion quat;
 tf::quaternionMsgToTF(pose_msg->pose.orientation,quat);
 //double roll,pitch,yaw;
 tf::Matrix3x3(quat).getRPY(drone_roll,drone_pitch,drone_yaw);
 ROS_INFO("rpy:%lf   %lf   %lf",drone_roll,drone_pitch,drone_roll);
}


void fly_vel_control_with_time(fly_pkg::vel_srv& vel_pos,ros::ServiceClient &vel_client,float pos_x,float pos_y,float pos_z,float yaw,
                            float vel_x,float vel_y,float vel_z,
                            short vel_control_flag, float vel_times,float vel_hz,
                            float vel_angle_accuracy,float vel_pos_accuracy){
    ros::Time last_times = ros::Time::now();
    ros::Rate pos_rate(vel_hz);
    fly_vel_control(vel_pos.request, pos_x , pos_y , pos_z , yaw , vel_control_flag,vel_angle_accuracy,vel_pos_accuracy,vel_x,vel_y,vel_z); 
    while(ros::ok()){         
        while(ros::ok()){
            if(vel_client.call(vel_pos))
                break;
        }
        
        if(vel_control_flag==Vel_Land){  //降落模式
            if(!current_state.armed){ //等待模式切换完毕
                break;
            }
        }
        //处理持续时间
        else{
            if(vel_pos.response.vel_arrive_flag){
                if(ros::Time::now() -last_times> ros::Duration(vel_times)){
                    break;
                }
            else{
                last_times = ros::Time::now();
            }
            }
        }
        ros::spinOnce();
        pos_rate.sleep();
    }
}




/// @brief 控制发布信息,准备发布到setpoint话题里
/// @param msg 
/// @param aim_x 
/// @param aim_y 
/// @param aim_z 
/// @param aim_roll 
/// @param aim_pitch 
/// @param aim_yaw 
/// @param  
/// @return 
bool pos_control(geometry_msgs::PoseStamped msg,double aim_x,double aim_y,double aim_z,double linear_distance,double aim_roll,double aim_pitch,double aim_yaw,double angle_distance){
geometry_msgs::Quaternion q;
q=tf::createQuaternionMsgFromRollPitchYaw(aim_roll,aim_pitch,aim_yaw);
    msg.pose.orientation=q;
    msg.pose.position.x = aim_x;
    msg.pose.position.y = aim_y;
    msg.pose.position.z = aim_z;

    if(get_distance(aim_x,my_pose.pose.position.x)<linear_distance && get_distance(aim_y,my_pose.pose.position.y)<linear_distance && get_distance(aim_z,my_pose.pose.position.z)<linear_distance 
    && get_distance(aim_yaw,drone_yaw)<angle_distance)[
        return true;
    ]else{

        return false;
    }
}




int main(int argc, char **argv)
{

    ros::init(argc, argv, "control"); 
    

    ros::NodeHandle nh;

    ros::Rate rate(20.0);

    //更新时间
    ros::Time last_request = ros::Time::now();

    while(ros::ok())
    {
       
        
        


 
        ros::spinOnce();
        rate.sleep();
    }
    
    return 0;
}
   

