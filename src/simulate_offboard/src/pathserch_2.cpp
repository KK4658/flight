// #include <stdio.h>
#include"stdio.h"

#define go_to_right 0
#define go_to_down 1
#define go_to_left 2
#define go_to_up 3
#define way_out 4

#define HAVE_POINT_NO_GET 10
#define NO_POINT_NO_GET 20

int point[7][9] = { {0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0},
                    {0,0,0,1,0,0,0,0,0},
                    {0,1,1,1,0,0,0,0,0},
                    {0,0,0,1,0,0,0,0,0},
                    {0,1,1,1,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0}};
                    // int current_point[2]={7,9};

float real_point[100][2];


                    int x=7,y=9;//从上往下数，从左往右数。第x-1行，第y-1列


int obstacle_num = 0;
int get_every_point_flag1 = 0;
int get_every_point_flag2 = 0;
int walk_way = 0;
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
GRID grid[7][9];
int path[150][2];


int cnt=0;



int go_to_where1(int& x,int& y){
    walk_way++;
    grid[x-1][y-1].have_arrived = 1;

    if(x+1<=7){//不是边界
    if(grid[x + 1 - 1][y-1].is_obstacle  == 0 && grid[x + 1 - 1][y-1].have_arrived == 0){//不是障碍物，没有来过
        x++;
        grid[x-1][y-1].last_x=x-1;
        grid[x-1][y-1].last_y=y;
        path[cnt][0]=x;
        path[cnt][1]=y;
        cnt++;
        printf("\ngo to x:%d,y:%d",x,y);
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
        printf("\ngo to x:%d,y:%d",x,y);
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
        printf("\ngo to x:%d,y:%d",x,y);
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
    printf("\ngo to x:%d,y:%d",x,y);
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

void point_init(){

    for(int i = 0;i<7;i++){
    for(int j =0;j<9;j++){
        grid[i][j].have_arrived = 0;
        if(point[i][j]==1){
            obstacle_num++;
            grid[i][j].is_obstacle = 1;
        }
    }
}


}


void get_traject(){
do
{

if(test_no_achieve()==NO_POINT_NO_GET){
break;
}

int n = go_to_where1(x,y);

if(n==way_out)//遇到死胡同了
{
    printf("////////no way////////////////////\n");
    //break;
}
}while(1);
        for(int i =0;i<cnt;i++){
            printf("%d:(%d,%d)\n",walk_way,path[i][0],path[i][1]);
        }
    }

int main(){
//x-1是行,y-1是列
point_init();
get_traject();



for(int i =0;i<cnt;i++){
real_point[i][0] = float(8-path[i][0])*0.5;
real_point[i][1] = float(10-path[i][1])*0.5;
printf("(%f,%f)\n",real_point[i][0],real_point[i][1]);
}

     int c = 0;
for(int i =1;i<=7;i++){
    printf("\n");
    for(int j =1;j<=9;j++){
        printf("%d",grid[i-1][j-1].have_arrived);
        if(grid[i-1][j-1].have_arrived ==0 && grid[i-1][j-1].is_obstacle==0 ){
            c++;
        }
    }
}


    return 0;
}