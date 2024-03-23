#include <iostream>
#include <vector>
#include <queue>
#include <string.h>
#include <algorithm>
using namespace std;

const int n = 200;
const int robot_num = 10;
const int berth_num = 10;
const int N = 210;
int dir[4][2] = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}};
int boat_ship_second[10] = {0};
queue<vector<int>> new_goods;


struct Robot
{
    int x, y, goods;
    int status;
    int value;
    int mbx, mby;
    int nx, ny;  // 下一个位置
    int tx, ty; //目标货物的坐标
    Robot() {}
    Robot(int startX, int startY)
    {
        x = startX;
        y = startY;
    }
}robot[robot_num + 10];

struct Berth
{
    int x;
    int y;
    int id; // 记录序号
    int transport_time;
    int loading_speed;
    queue<int> values;
    float Efficiency_ratio;
    Berth(){}
    Berth(int x, int y, int transport_time, int loading_speed)
    {
        this -> x = x;
        this -> y = y;
        this -> transport_time = transport_time;
        this -> loading_speed = loading_speed;
        this -> Efficiency_ratio = float(transport_time) / float(loading_speed);
    }
}berth[berth_num + 10];

struct Boat
{
    int num, pos, status, margin;
}boat[10];

int money, boat_capacity, id;
char ch[N][N];
int gds[N][N][3];
int transport_time[10] = {0};
int loading_speed[10] = {0};
int boat_berth_id[5] = {0};
int berth_used[10] = {0};

bool compareEfficiency(const Berth& a, const Berth& b) 
{
    return a.Efficiency_ratio > b.Efficiency_ratio; // 降序
}

void Init()
{
    for(int i = 0; i < n; i ++)
    {
        scanf("%s", ch[i]);
    }
    //the (x, y) starts from (0, 0);
    for(int i = 0; i < berth_num; i++)
    {
        int id;
        scanf("%d", &id);
        scanf("%d%d%d%d", &berth[id].x, &berth[id].y, &berth[id].transport_time, &berth[id].loading_speed);
        transport_time[i] = berth[i].transport_time;
        loading_speed[i] = berth[i].loading_speed;
        berth[i].id = id;
        float Efficiency_ratio = float (berth[i].loading_speed) / float(berth[i].transport_time);
        berth[i].Efficiency_ratio = Efficiency_ratio;
    }//berth initialization

    // sort the berth
    int num_elements_to_sort = std::min(10, berth_num);
    // 使用 sort 函数和自定义比较函数仅排序前十个元素
    std::sort(berth, berth + num_elements_to_sort, compareEfficiency);
    for (int i = 0; i < 10; i++) 
    {
        boat_berth_id[i] = berth[i].id;
    }
    scanf("%d", &boat_capacity);

    for(int i = 0;i < 5;i ++)
    {
        boat[i].margin = boat_capacity;
        boat[i].num = 0;
        // fprintf(stderr, "margin: %d\n", boat[i].margin);
    }
    char okk[100];
    scanf("%s", okk);
    printf("OK\n");//the information of the map ends with 'OK'
    fflush(stdout);
}

int Input()
{
    scanf("%d%d", &id, &money);//zhen numth and money now
    int num;
    scanf("%d", &num);

    for(int i = 1; i <= num; i ++)
    {
        int x, y, val;
        scanf("%d%d%d", &x, &y, &val);
        gds[x][y][0] = val;
        gds[x][y][1] = id;
        vector<int> t;
        t.push_back(x);
        t.push_back(y);
        t.push_back(id);
        new_goods.push(t);
    }//the info of new goods
    int flag = 1;
    while(flag && !new_goods.empty())
    {
        vector<int> tmp = new_goods.front();
        if(tmp[2] == gds[tmp[0]][tmp[1]][1] && id - tmp[2] >= 1000)
        {
            gds[tmp[0]][tmp[1]][0] = 0;//货物从未改变且已经超时
            new_goods.pop();
        }
        else if(tmp[2] != gds[tmp[0]][tmp[1]][1])
        {
            new_goods.pop();//货物已经改变
        }
    }
    for(int i = 0; i < robot_num; i ++)
    {
        int sts;
        scanf("%d%d%d%d", &robot[i].goods, &robot[i].x, &robot[i].y, &robot[i].status);
    }//the info of robot

    for(int i = 0; i < 5; i ++)
    {
        scanf("%d%d\n", &boat[i].status, &boat[i].pos);
    }//the info of ships

    char okk[100];
    scanf("%s", okk);//the information of ctrl ends with 'OK'
    return id;
}

//判断下一步是否可以
int vis[N][N];
bool is_valid(int nowx, int nowy, int nx, int ny, int roboth, int step) // 只有第一步考虑是否与机器人碰撞
{
    for (int i = 0 ;i < roboth; i++) 
    {
        if (step == 0 && (nx == robot[i].nx && ny == robot[i].ny || nowx == robot[i].nx && nowy == robot[i].ny && nx == robot[i].x && ny == robot[i].y))
        {
            return false;
        }
    }
    if(0 <= nx < 200 && 0 <= ny < 200 && (ch[nx][ny] == '.' || ch[nx][ny] == 'B') && !vis[nx][ny])
    {
        return true;
    }
    return false;
}

struct last_dir
{
    int lx, ly;
    int dir;
    int step;
    double v;
};

int bfs(int roboth, int id)
{
    struct last_dir path[210][210];
    memset(vis, 0, sizeof(vis)); // 重置访问状态
    vector<int> route;
    queue<pair<int, int>> q; // 使用 pair 而不是 vector<int> 来存储坐标
    q.push({robot[roboth].x, robot[roboth].y});
    path[robot[roboth].x][robot[roboth].y].lx = -1; // 使用 -1 表示起点
    path[robot[roboth].x][robot[roboth].y].ly = -1;
    vis[robot[roboth].x][robot[roboth].y] = 1; // 标记起始点为已访问
    path[robot[roboth].x][robot[roboth].y].step = 0;
    path[robot[roboth].x][robot[roboth].y].v = -1.0;
    int gx = -1, gy = -1, depth;
    double min = -1.0;
    while (!q.empty() && depth <= 1000)
    {
        auto [x, y] = q.front(); q.pop();
        depth = path[x][y].step;
        if (gds[x][y][0] != 0 && (1000 - id + gds[x][y][1]) > depth && gds[x][y][2] == 0 && 200.0 / depth > min)
        {
            int val = gds[x][y][0];
            int step = path[x][y].step;
            if(path[x][y].lx == -1 && path[x][y].ly == -1)
            {
                path[x][y].v = (double)0x7fffffff;
            }//表明机器人一开始就位于有货物的地方
            else
            {
                path[x][y].v = double(gds[x][y][0]) / step;
            }
            if(path[x][y].v > min)
            {
                gx = x; 
                gy = y;
                min = path[x][y].v;
            }
            break;
        }//search the best good to choose

        for (int i = 0; i < 4; i++)
        {
            int nx = x + dir[i][0], ny = y + dir[i][1];
            if (!is_valid(x, y, nx, ny, roboth, depth) || vis[nx][ny]) continue; // 检查是否有效且未访问
            vis[nx][ny] = 1; // 标记为已访问
            q.push({nx, ny}); // 加入队列
            path[nx][ny].lx = x, path[nx][ny].ly = y, path[nx][ny].step = path[x][y].step + 1; // 记录父节点位置
            path[nx][ny].dir = i;
        }
    }
    memset(vis, 0, sizeof(vis));
    if(gx == -1)
    {
        return -1;
    }
    if(gx != -1)
    {
        if(robot[roboth].tx != -1 && robot[roboth].ty != -1)
        {
            gds[robot[roboth].tx][robot[roboth].ty][2] = 0;
        }
        gds[gx][gy][2] = 1;
        robot[roboth].tx = gx, robot[roboth].ty = gy;
        int prex, prey;
        while (path[gx][gy].lx != -1 && path[gx][gy].ly != -1)
        {
            int px = path[gx][gy].lx, py = path[gx][gy].ly; // 父节点位置
            prex = gx, prey = gy;
            gx = px, gy = py; // 移动到父节点位置继续回溯
        }
        return path[prex][prey].dir;
    }
    return -1;
}

bool if_bfs(int roboth)
{
    for(int i = 1;i <= 200;i++)
    {
        for(int j = 0;j <= 200;j++)
        {
            if(gds[i][j][0])
            {
                return true;
            }
        }
    }
    return false;
}

int Is_in_berth(int x , int y) 
{
    for (int i = 0; i < 10; i++) 
    {
        if (x - berth[i].x <= 3 && y - berth[i].y <= 3  && ch[x][y] == 'B') 
        {
            return berth[i].id; // 真正的港口
        }
    }
    return -1;
}

int search_berth(int roboth) 
{
    struct last_dir path[210][210];
    memset(vis, 0, sizeof(vis)); // 重置访问状态
    vector<int> route;
    queue<pair<int, int>> q; // 使用 pair 而不是 vector<int> 来存储坐标
    q.push({robot[roboth].x, robot[roboth].y});
    path[robot[roboth].x][robot[roboth].y].lx = -1; // 使用 -1 表示起点
    path[robot[roboth].x][robot[roboth].y].ly = -1;
    path[robot[roboth].x][robot[roboth].y].dir = -1;
    path[robot[roboth].x][robot[roboth].y].step = 0;
    vis[robot[roboth].x][robot[roboth].y] = 1; // 标记起始点为已访问
    int gx = -1, gy = -1;

    while (!q.empty())
    {
        auto [x, y] = q.front(); q.pop();
        int depth = path[x][y].step;
        if (Is_in_berth(x, y) != -1)
        {   
            // fprintf(stderr, "robot[%d] has found the berth!\n", roboth);
            gx = x; gy = y;
            // fprintf(stderr, "Found berth: %d %d!\n", x, y);
            break;
        }
        for (int i = 0; i < 4; i++)
        {
            int nx = x + dir[i][0], ny = y + dir[i][1];
            if (!is_valid(x, y, nx, ny, roboth, depth) || vis[nx][ny]) continue; // 检查是否有效且未访问
            vis[nx][ny] = 1; // 标记为已访问
            q.push({nx, ny}); // 加入队列
            path[nx][ny].lx = x, path[nx][ny].ly = y, path[nx][ny].step = path[x][y].step + 1; // 记录父节点位置
            path[nx][ny].dir = i;
        }
    }

    memset(vis, 0, sizeof(vis));
    if(gx == -1)
    {
        // fprintf(stderr, "robot.x, robot.y %d %d\n", robot[roboth].x, robot[roboth].y);
        // fprintf(stderr, "return -1 from gx == -1\n");
        return -1;
    }
    if(gx != -1)
    {
        int prex = gx, prey = gy;
        // fprintf(stderr, "robot.goods : %d, gx: %d gy %d robot.x %d robot.y %d\n", robot[roboth].goods, gx, gy, robot[roboth].x, robot[roboth].y);
        while (path[gx][gy].lx != -1 && path[gx][gy].ly != -1)
        {
            // fprintf(stderr, "path.dir%d\n", path[gx][gy].dir);
            int px = path[gx][gy].lx, py = path[gx][gy].ly; // 父节点位置
            prex = gx, prey = gy;
            gx = px, gy = py; // 移动到父节点位置继续回溯
        }
        // fprintf(stderr, "robot[%d] will mov to %d\n", roboth, path[prex][prey].dir);
        return path[prex][prey].dir;
    }
    return -1;
}

int value_sum(struct Berth k, int m, int index)// m is margin
{
    int ans = 0;
    int l = 0;
    queue<int> tmp = k.values;
    // fprintf(stderr, "k.values.size = %d tmp.size = %d\n",k.values.size(), tmp.size());
    while(!tmp.empty() && l < m)
    {
        l++;
        int d = tmp.front();
        ans += d;
        tmp.pop();
    }
    // fprintf(stderr, "berth[%d] now has goods about %d\n", index, ans);
    return ans;
}

int max_money;
void ssort(struct Berth *berth, int margin, int n[], int boath)
{
    int max_index = 0;
    max_money = 0;
    for(int i = 1; i <= 9; i ++)
    {
        int money1 = value_sum(berth[i], margin, i);
        int money2 = value_sum(berth[max_index], margin, max_index);
        if(money1 > money2 && berth_used[i] == 0 || berth_used[max_index] == 1)
        {
            max_money = money1;
            max_index = i;
        }
    }
    n[0] = max_index;
}

int main()
{
    memset(vis, 0, sizeof(vis));
    memset(gds, 0, sizeof(gds));
    Init();
    for(int zhen = 1; zhen <= 15000; zhen ++)
    {
        int id = Input();
        for (int i = 0; i < robot_num; i++) 
        {
            if (robot[i].status == 0) 
            {
                fprintf(stderr, "robot[%d] has a crack!\n", i);
            }
        }
        for(int i = 0; i < robot_num; i ++)
        {   
            int mov = -1;
            if (robot[i].goods == 0) 
            { // 此时没有货物，搜索货物
                mov = bfs(i, zhen);
                if (mov == -1) 
                {
                    for (int j = 0; j < 4; j++) 
                    {
                        int nx = robot[i].x + dir[j][0], ny = robot[i].y + dir[j][1];
                        if (is_valid(robot[i].x, robot[i].y, nx, ny, i, 0)) 
                        {
                            mov = j; break;
                        }
                    }
                }
            }
            else 
            { // 有货物， 找港口
                mov = search_berth(i); 
                if (mov == -1) 
                {
                    for (int j = 0; j < 4; j++) 
                    {
                        int nx = robot[i].x + dir[j][0], ny = robot[i].y + dir[j][1];
                        if (is_valid(robot[i].x, robot[i].y, nx, ny, i, 0)) 
                        {
                            mov = j; break;
                        }
                    }
                }
            }
            int nx = robot[i].x + dir[mov][0], ny = robot[i].y + dir[mov][1]; // 下一步的位置
            robot[i].nx = nx; robot[i].ny = ny;
            printf("move %d %d\n", i, mov);
            if (gds[nx][ny][0] != 0 && robot[i].goods == 0) 
            {
                printf("get %d\n", i);
                robot[i].goods = 1; 
                robot[i].value = gds[nx][ny][0];//表示已经有货物
                gds[nx][ny][0] = 0;
                gds[nx][ny][2] = 0;
            }
            else 
            {
                if (robot[i].goods == 1 && ch[nx][ny] == 'B') 
                { // 有货物，有港口
                    printf("pull %d\n", i);
                    robot[i].goods = 0;
                    berth[Is_in_berth(nx,ny)].values.push(robot[i].value);
                    // fprintf(stderr, "robot[%d] has pull goods in berth[%d]\n", i, Is_in_berth(nx,ny));
                    // fprintf(stderr, "berth[%d] has goods of %d\n", Is_in_berth(nx,ny), berth[Is_in_berth(nx,ny)].values.size());
                    robot[i].value = 0;
                    // int m = berth[Is_in_berth(nx,ny)].values.size();
                }
            }
        }
        for (int i = 0; i < 5; i++) 
        {
            if(boat[i].status == 1 && boat[i].pos != -1) 
            {
                if(boat[i].margin != 0 && !berth[boat[i].pos].values.empty())
                {
                    // fprintf(stderr, "boat[%d].margin:%d\n", i, boat[i].margin);
                    int speed = loading_speed[boat[i].pos];
                    while(speed > 0 && boat[i].margin > 0 && !berth[boat[i].pos].values.empty())
                    {
                        boat[i].num += berth[boat[i].pos].values.front();
                        berth[boat[i].pos].values.pop();
                        boat[i].margin--;
                        // fprintf(stderr, "boat[%d].berth %d .margin:%d\n",i,  boat[i].pos, boat[i].margin);
                        speed--;
                    } 
                }  
                if (boat[i].margin > 0 && berth[boat[i].pos].values.empty()) 
                {
                    // if(boat_ship_second[i] == 0){    
                    int n[10];
                    for(int i = 0;i < 10;i ++)
                    {
                        n[i] = i;
                    }
                    ssort(berth, boat[i].margin, n, i);
                    float target_money = float(boat[i].num + max_money) / float(500 + transport_time[n[0]]);
                    float now_money = float(boat[i].num) / float(transport_time[boat[i].pos]);
                    if (target_money > now_money)
                    {
                        berth_used[boat[i].pos] = 0;
                        // fprintf(stderr, "boat[%d] will ship to %d\n", i, n[0]);
                        printf("ship %d %d\n",i, n[0]);
                        boat_ship_second[i] = 1;
                        berth_used[n[0]] = 1;
                    }
                    else 
                    {
                        boat[i].num = 0;
                        berth_used[boat[i].pos] = 0;
                        boat[i].margin = boat_capacity;
                        printf("go %d\n", i);
                    }
                }
                if(boat[i].margin <= 0 || 15000 - zhen < transport_time[boat[i].pos] + 10)
                {
                  berth_used[boat[i].pos] = 0;
                //   fprintf(stderr, "berth[%d] will be not used!\n", boat[i].pos);
                  printf("go %d\n", i);
                  boat[i].num = 0;
                  boat_ship_second[i] = 0;
                //   fprintf(stderr, "boat[%d] will go!\n", i);
                  boat[i].margin = boat_capacity;
                }
            }

            if(boat[i].status == 1 && boat[i].pos == -1 && zhen > 2) 
            {
                int n[10];
                for(int i = 0;i < 10;i ++)
                {
                    n[i] = i;
                }
                ssort(berth, boat[i].margin, n, i);
                // fprintf(stderr, "boat[%d] will ship to %d\n", i, n[0]);
                printf("ship %d %d\n",i, n[0]);
                berth_used[n[0]] = 1;
            }

            if(boat[i].status == 1 && boat[i].pos == -1 && zhen == 2) 
            {
                for (int i = 0; i < 5; i++) 
                {
                    printf("ship %d %d\n", i, boat_berth_id[i]);
                    berth_used[boat_berth_id[i]] = 1;
                }
            }
        }
        puts("OK");
        fflush(stdout);
    }
    return 0;
}