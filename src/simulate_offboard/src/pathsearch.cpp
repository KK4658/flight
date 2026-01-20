// #include <stdio.h>
#include"stdio.h"

#define go_to_right 0
#define go_to_down 1
#define go_to_left 2
#define go_to_up 3
#define way_out 4

int point[7][9] = { {0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0},
                    {1,0,0,1,1,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0}};
                    // int current_point[2]={7,9};




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
int path1[70][2];
int path2[70][2];
int path3[70][2];
int not_get_path[70][2];
int cnt=0;


int go_to_where2(int& x,int& y){
    walk_way++;
    grid[x-1][y-1].have_arrived = 1;

        if(y-1>=1){//不是边界
    if(grid[x-1][y-1-1].is_obstacle  == 0  &&grid[x-1][y-1-1].have_arrived==0){//不是障碍物
        y--;
        grid[x-1][y-1].last_x=x;
        grid[x-1][y-1].last_y=y+1;
        path2[cnt][0]=x;
        path2[cnt][1]=y;
        cnt++;
       // printf("\ngo to x:%d,y:%d",x,y);
        return go_to_left ;
    }
}
    if(x-1>=1){//不是边界
    if(grid[x - 1-1][y-1].is_obstacle  == 0 && grid[x - 1-1][y-1].have_arrived==0){//不是障碍物
        x--;
                grid[x-1][y-1].last_x=x+1;
        grid[x-1][y-1].last_y=y;
        path2[cnt][0]=x;
        path2[cnt][1]=y;
        cnt++;
       // printf("\ngo to x:%d,y:%d",x,y);
        
        return go_to_up ;
    }
}

    if(y+1<=10){//不是边界
    if(grid[x - 1][y+1-1].is_obstacle  == 0  && grid[x - 1][y+1-1].have_arrived==0){//不是障碍物
        y++;
                grid[x-1][y-1].last_x=x;
        grid[x-1][y-1].last_y=y-1;
        path2[cnt][0]=x;
        path2[cnt][1]=y;
        cnt++;
        //printf("\ngo to x:%d,y:%d",x,y);
        return go_to_right ;
    }
}

    if(x+1<=7){//不是边界
    if(grid[x + 1 - 1][y-1].is_obstacle  == 0 && grid[x + 1 - 1][y-1].have_arrived == 0){//不是障碍物，没有来过
        x++;
        grid[x-1][y-1].last_x=x+1;
        grid[x-1][y-1].last_y=y;
        path2[cnt][0]=x;
        path2[cnt][1]=y;
        cnt++;
        return go_to_down ;
    }
}
return way_out;
}

int go_to_where1(int& x,int& y){
    walk_way++;
    grid[x-1][y-1].have_arrived = 1;

    if(x+1<=7){//不是边界
    if(grid[x + 1 - 1][y-1].is_obstacle  == 0 && grid[x + 1 - 1][y-1].have_arrived == 0){//不是障碍物，没有来过
        x++;
        grid[x-1][y-1].last_x=x+1;
        grid[x-1][y-1].last_y=y;
        path1[cnt][0]=x;
        path1[cnt][1]=y;
        cnt++;
        return go_to_down ;
    }
}

    if(y-1>=1){//不是边界
    if(grid[x-1][y-1-1].is_obstacle  == 0  &&grid[x-1][y-1-1].have_arrived==0){//不是障碍物
        y--;
        grid[x-1][y-1].last_x=x;
        grid[x-1][y-1].last_y=y+1;
        path1[cnt][0]=x;
        path1[cnt][1]=y;
        cnt++;
       // printf("\ngo to x:%d,y:%d",x,y);
        
        return go_to_left ;
    }
}

    if(x-1>=1){//不是边界
    if(grid[x - 1-1][y-1].is_obstacle  == 0 && grid[x - 1-1][y-1].have_arrived==0){//不是障碍物
        x--;
                grid[x-1][y-1].last_x=x+1;
        grid[x-1][y-1].last_y=y;
        path1[cnt][0]=x;
        path1[cnt][1]=y;
        cnt++;
       // printf("\ngo to x:%d,y:%d",x,y);
        
        return go_to_up ;
    }
}
    if(y+1<=10){//不是边界
    if(grid[x - 1][y+1-1].is_obstacle  == 0  && grid[x - 1][y+1-1].have_arrived==0){//不是障碍物
        y++;
                grid[x-1][y-1].last_x=x;
        grid[x-1][y-1].last_y=y-1;
        path1[cnt][0]=x;
        path1[cnt][1]=y;
        cnt++;
        //printf("\ngo to x:%d,y:%d",x,y);
        
        return go_to_right ;
    }
}
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

point_init();
do
{
int n = go_to_where1(x,y);
if(n==way_out)
{
    printf("////////////////////no way/////////////////////////////\n");
    cnt=0;
    walk_way = 0;
    x=7;
    y=9;
    break;
}
if(walk_way==63-obstacle_num-1)
{
    get_every_point_flag1 = 1;
    break;
}
}while(get_every_point_flag1==0);

point_init();
do
{//////////////////////找第二条路径///////////////
int n = go_to_where2(x,y);
if(n==way_out)
{
    printf("no way\n");
    cnt=0;
    walk_way = 0;
    break;
}
if(walk_way==63-obstacle_num-1){
    get_every_point_flag2 = 1;
    break;
}
printf("asdasd");
}while(get_every_point_flag2==0);
        for(int i =0;i<cnt;i++){
            printf("%d:(%d,%d)\n",way_out,path2[i][0],path2[i][1]);
        }
    }

int main(){
//x-1是行,y-1是列
point_init();
get_traject();


// if(get_every_point_flag == 0){
//     int c = 0;
// // for(int i =1;i<=7;i++){
// //     printf("\n");
// //     for(int j =1;j<=9;j++){
// //         printf("%d",grid[i][j].have_arrived);
// //         if(grid[i-1][j-1].have_arrived ==0 && grid[i-1][j-1].is_obstacle==0 ){
// //             not_get_path[c][0] = i;
// //             not_get_path[c][1] = j;
// //             //printf("asd\n");
// //             c++;
// //         }
// //     }
// // }
//         // for(int i =0;i<c;i++){
//         //     printf("%d:(%d,%d)\n",c,not_get_path[i][0],not_get_path[i][1]);
//         // }

// }
printf("\n%d      %d  \n",get_every_point_flag1,get_every_point_flag2);



//printf("asasdasdasdasdasdd\n");
// cout<<"asd"<<endl;
    return 0;
}