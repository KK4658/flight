//一系列头文件

//ros库
#include <ros/ros.h>
//#include <Eigen/Dense>
//发布的位置消息体对应的头文件，该消息体的类型为geometry_msgs::PoseStamped
//用来进行发送目标位置
/*
ros官网上这样定义
# A Pose with reference coordinate frame and timestamp
Header header
Pose pose
实际上就是一个带有头消息和位姿的消息
*/
#include <geometry_msgs/PoseStamped.h>

//CommandBool服务的头文件，该服务的类型为mavros_msgs::CommandBool
//用来进行无人机解锁
/*
其结构如下（来源于ros wiki）
# Common type for switch commands
bool value
---
bool success
uint8 result
可以看到，发送的请求是一个bool类型的数据，为True则解锁，为False则上锁
返回的响应中success是一个bool类型的参数，表示上电/断电操作是否成功执行。
如果操作成功执行，success值为True，否则为False。
result是一个int32类型的参数，表示执行上电/断电操作的结果。
如果解锁/上锁操作成功执行，result值为0，
否则为其他值，表示执行解锁/上锁操作时发生了某种错误或异常。可以根据这个数值查看是哪种问题导致
*/
#include <mavros_msgs/CommandBool.h>

//SetMode服务的头文件，该服务的类型为mavros_msgs::SetMode
//用来设置无人机的飞行模式，切换offboard
/*
wiki上的消息定义如下
# set FCU mode
#
# Known custom modes listed here:
# http://wiki.ros.org/mavros/CustomModes

# basic modes from MAV_MODE
uint8 MAV_MODE_PREFLIGHT = 0
uint8 MAV_MODE_STABILIZE_DISARMED = 80
uint8 MAV_MODE_STABILIZE_ARMED = 208
uint8 MAV_MODE_MANUAL_DISARMED = 64
uint8 MAV_MODE_MANUAL_ARMED = 192
uint8 MAV_MODE_GUIDED_DISARMED = 88
uint8 MAV_MODE_GUIDED_ARMED = 216
uint8 MAV_MODE_AUTO_DISARMED = 92
uint8 MAV_MODE_AUTO_ARMED = 220
uint8 MAV_MODE_TEST_DISARMED = 66
uint8 MAV_MODE_TEST_ARMED = 194

uint8 base_mode # filled by MAV_MODE enum value or 0 if custom_mode != ''
string custom_mode # string mode representation or integer
---
bool success

String类型的变量custom_mode就是我们想切换的模式，有如下选择：
MANUAL，ACRO，ALTCTL，POSCTL，OFFBOARD，STABILIZED，RATTITUDE，AUTO.MISSION
AUTO.LOITER，AUTO.RTL，AUTO.LAND，AUTO.RTGS，AUTO.READY，AUTO.TAKEOFF
*/
#include <mavros_msgs/SetMode.h>

//订阅的消息体的头文件，该消息体的类型为mavros_msgs::State
//查看无人机的状态
/*
wiki上是这样的
std_msgs/Header header
bool connected
bool armed
bool guided
bool manual_input
string mode
uint8 system_status

解析如下：
header：消息头，包含时间戳和框架信息；
connected：表示是否连接到了 mavros 节点；
armed：表示无人机当前是否上锁；
guided：表示无人机当前是否处于 GUIDED 模式；
mode：表示当前无人机所处的模式，包括以下几种：
MANUAL，ACRO，ALTCTL，POSCTL，OFFBOARD，STABILIZED，RATTITUDE，AUTO.MISSION
AUTO.LOITER，AUTO.RTL，AUTO.LAND，AUTO.RTGS，AUTO.READY，AUTO.TAKEOFF
*/
#include <mavros_msgs/State.h>  
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Quaternion.h>
#include "tf/transform_listener.h"
//关于路劲规划的宏定义
#define go_to_right 0
#define go_to_down 1
#define go_to_left 2
#define go_to_up 3
#define way_out 4

#define HAVE_POINT_NO_GET 10
#define NO_POINT_NO_GET 20
//建立一个订阅消息体类型的变量，用于存储订阅的信息
mavros_msgs::State current_state;

   //实例化一个geometry_msgs::PoseStamped类型的消息对象，并对其赋值，最后将其发布出去
    //尽管PX4在航空航天常用的NED坐标系下操控飞机，但MAVROS将自动将该坐标系切换至常规的ENU坐标系下
    geometry_msgs::PoseStamped aim_pose_msg;
    geometry_msgs::PoseStamped drone_pose;
//降落信号，切换为降落状态的时候赋值1
int land_sign = 0;

//当前位置的欧拉角
double drone_roll,drone_pitch,drone_yaw;
bool arrive_flag = 0;
int ct = 0;//count
  const int num_point = 1;//点数
// double point[num_point][3]={{1.0,2.0,3.0},{0.0,2.0,3.0},{0.0,0.0,3.0}};
double point[num_point][3]={{0.0,0.0,0.2}};//点的规定
//double drone_pose[3]={0};//飞机的位置
const double distance = 0.05;//规定的距离
int point_count = 0;
int stay_hight =  1.2;

ros::Time last_request;


//点的定义，从上往下数是行，从左往右数是列
int point_map[7][9] = { {0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0}};

int x=7,y=9;//初始点
int walk_way = 0;//存到的点数
class GRID
{
private:
    /* data    */
public:
    int is_obstacle = 0;
    int have_arrived = 0;
    int last_x;
    int last_y;
};
GRID grid[7][9];//格子地图的类
int path[70][2];
int cnt=0;


//回调函数的定义
void get_pose_cb(const geometry_msgs::PoseStampedConstPtr pose_msg);
void state_cb(const mavros_msgs::State::ConstPtr& msg);
bool target_msg_set( float aim_x,float aim_y,float aim_z,float linear_distance,double aim_yaw,double angle_distance);

   
    //ros::Publisher local_pos_pub = nh.advertise<geometry_msgs::PoseStamped>("pose_control", 10);
    ros::Subscriber pose_sub ;

    //这是一个订阅者对象，可以订阅无人机的状态信息（状态信息来源为MAVROS发布），用来储存无人机的状态，在回调函数中会不断更新这个状态变量
    ros::Subscriber state_sub ;

    //这是一个发布者对象，用来在本地坐标系下发布目标点，后面会以20Hz频率发布目标点
    ros::Publisher aim_pos_pub ;

    //一个客户端，用来解锁无人机，这是因为无人机如果降落后一段时间没有收到信号输入，会自动上锁来保障安全
    ros::ServiceClient arming_client;

    //一个客户端，用来切换飞行模式
    ros::ServiceClient set_mode_client ;


    //建立一个类型为SetMode的服务端offb_set_mode，并将其中的模式mode设为"OFFBOARD"，作用便是用于后面的客户端与服务端之间的通信（服务）
    mavros_msgs::SetMode offb_set_mode;
    mavros_msgs::SetMode land_set_mode;
    mavros_msgs::SetMode take_off_mode;


    //建立一个类型为CommandBool的服务端arm_cmd，并将其中的是否解锁设为"true"，作用便是用于后面的客户端与服务端之间的通信（服务）
    mavros_msgs::CommandBool arm_cmd;
    



/// @brief 订阅mavros/state时的回调函数，接受到该消息体的内容时执行里面的内容，内容是储存飞控当前的状态
/// @param msg 
void state_cb(const mavros_msgs::State::ConstPtr& msg){
    current_state = *msg;
}

void set_mode_land(){

    mavros_msgs::SetMode offb_set_mode;
    offb_set_mode.request.custom_mode = "AUTO.LAND";

}

/// @brief 返回两点间的欧氏距离
/// @param point1 
/// @param point2 
/// @return 
double get_distance(double point1,double point2){
return abs(point1-point2);
}





/// @brief 接受位姿消息的回调函数
/// @param pose_msg 
void get_pose_cb(const geometry_msgs::PoseStampedConstPtr pose_msg){
drone_pose=*pose_msg;
 tf::Quaternion quat;
 tf::quaternionMsgToTF(pose_msg->pose.orientation,quat);
 tf::Matrix3x3(quat).getRPY(drone_roll,drone_pitch,drone_yaw);
}


/// @brief 位置控制函数
/// @param aim_x 
/// @param aim_y 
/// @param aim_z 
/// @param linear_distance 
/// @param aim_yaw 
/// @param angle_distance 
/// @param stay_time 
void pose_control(float aim_x,float aim_y,float aim_z,float linear_distance,double aim_yaw,double angle_distance,double stay_time){
   ros::Rate pos_rate(20.0);
   arrive_flag = 0;
   ros::Time last_times = ros::Time::now();
    while(!(target_msg_set(aim_x,aim_y,aim_z,linear_distance, aim_yaw, angle_distance))){
        //发布位置信息，所以综上飞机只有先打开offboard模式然后解锁才能飞起来
        aim_pos_pub.publish(aim_pose_msg); 
        //ROS_INFO("first point");
         ros::spinOnce();
        pos_rate.sleep();
   }

   while(ros::ok()){
 if(ros::Time::now() -last_times> ros::Duration(stay_time)){
                    break;
                }
            else{
                last_times = ros::Time::now();
            }


    
   }
}



/// @brief 调制发布信息,准备发布到setpoint话题里,并且计算距离
/// @param msg 
/// @param aim_x 
/// @param aim_y 
/// @param aim_z 
/// @param aim_roll 
/// @param aim_pitch 
/// @param aim_yaw 
/// @param  
/// @return 
bool target_msg_set( float aim_x,float aim_y,float aim_z,float linear_distance,double aim_yaw,double angle_distance){
geometry_msgs::Quaternion q;
q=tf::createQuaternionMsgFromRollPitchYaw(0,0,aim_yaw);//转化成四元素    
    aim_pose_msg.pose.orientation=q;
    aim_pose_msg.pose.position.x = aim_x;
    aim_pose_msg.pose.position.y = aim_y;
    aim_pose_msg.pose.position.z = aim_z;


    if(get_distance(aim_x,drone_pose.pose.position.x)<linear_distance && get_distance(aim_y,drone_pose.pose.position.y)<linear_distance && get_distance(aim_z,drone_pose.pose.position.z)<linear_distance 
    && get_distance(aim_yaw,drone_yaw)<angle_distance){
        arrive_flag = true;
        return true;
    }else{
        arrive_flag = false;
        return false;
    }
}

/// @brief 检测地图还有没有没到达的点
/// @return 
int test_no_achieve(){
for(int i = 1;i<=7;i++){
    for(int j =1;j<=9;j++){
        if(grid[i-1][j-1].have_arrived==0 && grid[i-1][j-1].is_obstacle==0){
            return HAVE_POINT_NO_GET;
        }
    }
}
return NO_POINT_NO_GET;
}

/// @brief 判断去哪
/// @param x 
/// @param y 
/// @return 
int go_to_where(int& x,int& y){
     walk_way++;
    grid[x-1][y-1].have_arrived = 1;

    if(x+1<=7){//不是边界
    if(grid[x + 1 - 1][y-1].is_obstacle  == 0 && grid[x + 1 - 1][y-1].have_arrived == 0){//不是障碍物，没有来过
        x++;
        grid[x-1][y-1].last_x=x+1;
        grid[x-1][y-1].last_y=y;
        path[cnt][0]=x;
        path[cnt][1]=y;
        cnt++;
        //printf("\ngo to x:%d,y:%d",x,y);
        return go_to_down ;
    }
}
    if(y-1>=1){//不是边界
    if(grid[x-1][y-1-1].is_obstacle  == 0  &&grid[x-1][y-1-1].have_arrived==0){//不是障碍物
        y--;
        grid[x-1][y-1].last_x=x;
        grid[x-1][y-1].last_y=y+1;
        path[cnt][0]=x;
        path[cnt][1]=y;
        cnt++;
        //printf("\ngo to x:%d,y:%d",x,y);
       // printf("\ngo to x:%d,y:%d",x,y);
        
        return go_to_left ;
    }
}

    if(x-1>=1){//不是边界
    if(grid[x - 1-1][y-1].is_obstacle  == 0 && grid[x - 1-1][y-1].have_arrived==0){//不是障碍物
        x--;
                grid[x-1][y-1].last_x=x+1;
        grid[x-1][y-1].last_y=y;
        path[cnt][0]=x;
        path[cnt][1]=y;
        cnt++;
       // printf("\ngo to x:%d,y:%d",x,y);
       // printf("\ngo to x:%d,y:%d",x,y);
        
        return go_to_up ;
    }
}
    if(y-1<=9){//不是边界
    if(grid[x - 1][y+1-1].is_obstacle  == 0  && grid[x - 1][y+1-1].have_arrived==0){//不是障碍物
        y++;
                grid[x-1][y-1].last_x=x;
        grid[x-1][y-1].last_y=y-1;
        path[cnt][0]=x;
        path[cnt][1]=y;
        cnt++;
    //printf("\ngo to x:%d,y:%d",x,y);
        return go_to_right ;
    }
}


x=grid[x-1][y-1].last_x;
y=grid[x-1][y-1].last_y;
path[cnt][0]=x;
path[cnt][1]=y;
cnt++;

return way_out;
}

//初始化类
void point_init(){
    for(int i = 0;i<7;i++){
    for(int j =0;j<9;j++){
        if(point_map[i][j]==1){
            grid[i][j].is_obstacle = 1;
        }
    }
}
}

/// @brief 生成轨迹
void get_traject(){
    do
{
    ROS_INFO("%d",cnt);
if(test_no_achieve()==NO_POINT_NO_GET){
break;
}
int n = go_to_where(x,y);
// if(n==way_out)//遇到死胡同了
// {
//     printf("////////no way////////////////////\n");
//     //break;
// }
}while(1);
        for(int i =0;i<cnt;i++){
            //printf("%d:(%d,%d)\n",walk_way,path[i][0],path[i][1]);
        }
}



int main(int argc, char **argv)
{
    ros::init(argc, argv, "offb_node"); 
    ros::NodeHandle nh;
     //ros::Publisher local_pos_pub = nh.advertise<geometry_msgs::PoseStamped>("pose_control", 10);
     pose_sub = nh.subscribe<geometry_msgs::PoseStamped>("/mavros/local_position/pose", 200, get_pose_cb);

    //这是一个订阅者对象，可以订阅无人机的状态信息（状态信息来源为MAVROS发布），用来储存无人机的状态，在回调函数中会不断更新这个状态变量
     state_sub = nh.subscribe<mavros_msgs::State>("mavros/state", 10, state_cb);

    //这是一个发布者对象，用来在本地坐标系下发布目标点，后面会以20Hz频率发布目标点
     aim_pos_pub = nh.advertise<geometry_msgs::PoseStamped>("mavros/setpoint_position/local", 10);

    //一个客户端，用来解锁无人机，这是因为无人机如果降落后一段时间没有收到信号输入，会自动上锁来保障安全
    arming_client = nh.serviceClient<mavros_msgs::CommandBool>("mavros/cmd/arming");

    //一个客户端，用来切换飞行模式
     set_mode_client = nh.serviceClient<mavros_msgs::SetMode>("mavros/set_mode");



    //官方要求aim_pos_pub发布速率必须快于2Hz，这里设置为20Hz
    //PX4在两个Offboard命令之间有一个500ms的延时，如果超过此延时，系统会将回到无人机进入Offboard模式之前的最后一个模式。
    ros::Rate rate(20.0);
    point_init();//点的初始化
    get_traject();
    path[cnt-1][0] = 7;
    path[cnt-1][1] = 9;//删掉最后一个点，我也不知道为什么....
    // 等待飞控和MAVROS建立连接，current_state是我订阅的MAVROS的状态，在收到心跳包之后连接成功跳出循环
    while(ros::ok() && !current_state.connected){
        ros::spinOnce();
        rate.sleep();
    } 
    
    //set_mode_client.call(take_off_mode);
    aim_pose_msg.pose.position.x=0.0;
    aim_pose_msg.pose.position.y=0.0;
    aim_pose_msg.pose.position.z=1.0;
aim_pose_msg.header.stamp = ros::Time::now();

    land_set_mode.request.custom_mode = "AUTO.LAND";
    offb_set_mode.request.custom_mode = "OFFBOARD";
    take_off_mode.request.custom_mode = "AUTO.TAKEOFF";
    arm_cmd.request.value = true;
     

    //更新时间
    last_request = ros::Time::now();
while(!(  current_state.armed)){
        ROS_INFO("waiting for arming");
    aim_pose_msg.pose.position.x=0.0;
    aim_pose_msg.pose.position.y=0.0;
    aim_pose_msg.pose.position.z = stay_hight;//这个点要和第一个点一样
    aim_pos_pub.publish(aim_pose_msg);
    ros::spinOnce();
    rate.sleep();
}

set_mode_client.call(offb_set_mode);
ROS_INFO("takeoff!!");
pose_control(0,0,stay_hight,0.1,0,0.01,0);//起飞






for(int i =0;i<cnt;i++){
    pose_control(path[i][0]*0.5,path[i][1]*0.5,stay_hight,0.1,0,0.01,0);
    //ROS_INFO("(%d,%d)\n",path[i][0],path[i][1]);
}







//set_mode_client.call(land_set_mode);
ROS_INFO("arrive");


ros::shutdown();

    while(ros::ok())
    {
       
        ros::spinOnce();

        //根据前面ros::Rate rate(20.0);制定的发送频率自动休眠 休眠时间 = 1/频率
        rate.sleep();
    }
    
    return 0;
}
   

