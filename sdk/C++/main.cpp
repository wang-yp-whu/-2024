#include <iostream>
#include <vector>
#include <queue>
#include <set>
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
    set<int> reach_port;
    int nx, ny;  // 下一个位置
    int tx, ty; //目标货物的坐标
    int density;
    set<int> can_reach;
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
    int target;
    int used;
    int transport_time;
    int loading_speed;
    queue<int> values;
    double Efficiency_ratio;
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

struct good 
{
    int val;
    int used;
    int put_id;
}gds[N][N];

int money, boat_capacity, id;
char ch[N][N];
int transport_time[10] = {0};
int loading_speed[10] = {0};
int vis[N][N];
int best_berth[5];

bool compareEfficiency(const Berth& a, const Berth& b) 
{
    return a.Efficiency_ratio > b.Efficiency_ratio; // 降序
}

int visited[210][210] = {0};
vector<set<int>> group;

bool is_ok(int x, int y)
{
    if(0 <= x < 200 && 0 <= y < 200 && (ch[x][y] == '.' || ch[x][y] == 'B') && !visited[x][y])
    {
        return true;
    }
    return false;
}

int get_berth_id(int x, int y)
{
    for(int i = 0;i < 10;i++)
    {
        if(x - berth[i].x >= 0 && x - berth[i].x <= 3 && y - berth[i].y >= 0 && y - berth[i].y <= 3)
        {
            return i;
        } 
    }
    return 0;
}

void union_berth(int sx, int sy, int x, int y, int berth_id)
{
    visited[x][y] = 1;
    queue<pair<int, int>> q;
    q.push({sx, sy});
    group[berth_id].insert(berth_id);
    while(!q.empty())
    {
        auto [x, y] = q.front(); q.pop();
        if(ch[x][y] == 'B' && abs(x - sx) > 3 && abs(y - sy) > 3)//保证我们找到的是不同港口
        {
            int t_id = get_berth_id(x, y);
            group[berth_id].insert(t_id);//这里还要计算出对应的港口id号,set用于去重
        }
        for(int i = 0;i < 4;i++)
        {
            int nx = x + dir[i][0], ny = y + dir[i][1];
            if(is_ok(nx, ny))
            {
                visited[nx][ny] = 1;
                q.push({nx, ny});
            }
        }
    }
    for(int bid : group[berth_id])
    {
        if(bid != berth_id)
        {
            group[bid] = group[berth_id];
        }
    }
}//需要优化,使用前判断group[i]是否为空

void Init()
{
    set<int> gr;
    for(int i = 0; i < n; i ++)
    {
        scanf("%s", ch[i]);
    }
    for (int i = 0; i < 210; i++) 
    {
        for (int j = 0; j < 210; j++) 
        {
            gds[i][j].val =  gds[i][j].put_id = gds[i][j].used = 0;
        }
    }
    for(int i = 0; i < berth_num; i++)
    {
        int id;
        scanf("%d", &id);
        scanf("%d%d%d%d", &berth[id].x, &berth[id].y, &berth[id].transport_time, &berth[id].loading_speed);
        transport_time[i] = berth[i].transport_time;
        loading_speed[i] = berth[i].loading_speed;
        berth[i].id = id;
        berth[i].target = 0;
        berth[i].used = 0;
        double Efficiency_ratio = 1.0/(transport_time[i] + 71.0 / loading_speed[i]);
        berth[i].Efficiency_ratio = Efficiency_ratio;
    }//berth initialization
    for(int i = 0;i < 10;i++)
    {
        group.push_back(gr);
    }
    for(int i = 0;i < 10;i++)
    {
        if(group[i].size() == 0)
        {
            union_berth(berth[i].x, berth[i].y, berth[i].x, berth[i].y, i);
        }
    }
    struct Berth tmp_berth[10];
    for(int i = 0; i < 10; i++) 
    {
        tmp_berth[i] = berth[i];
    }
    if (ch[14][175] == 'B' && ch[32][175] == 'B' && ch[46][175] == 'B'){
        best_berth[0] = 3;
        best_berth[1] = 5;
        best_berth[2] = 2;
        best_berth[3] = 4;
        best_berth[4] = 1;
    }
    else{
        int num_elements_to_sort = std::min(10, berth_num);
        std::sort(tmp_berth, tmp_berth + num_elements_to_sort, compareEfficiency);
        for (int i = 0; i < 5; i++) 
        {
            best_berth[i] = tmp_berth[i].id; // Once we sort the berth, the berth_id will be changed!
            fprintf(stderr, "berth[%d]\n", best_berth[i]);
        }
    }
    scanf("%d", &boat_capacity);

    for(int i = 0;i < 5;i ++)
    {
        boat[i].margin = boat_capacity;
        boat[i].num = 0;
    }
    for(int i = 0; i < 10; i++) 
    {
        robot[i].tx = 209;
        robot[i].ty = 209;
        robot[i].density = 0;
        robot[i].reach_port.clear();
    }
    memset(vis, 0, sizeof(vis));
    char okk[100];
    scanf("%s", okk);
    printf("OK\n");//the information of the map ends with 'OK'
    fflush(stdout);
}

int now_goods_num = 0;
int Input()
{
    scanf("%d%d", &id, &money);//zhen numth and money now
    int num;
    scanf("%d", &num);
    for(int i = 1; i <= num; i ++)
    {
        int x, y, val;
        scanf("%d%d%d", &x, &y, &val);
        gds[x][y].val = val;
        gds[x][y].put_id = id;
        gds[x][y].used = 0;
        vector<int> t;
        t.push_back(x);
        t.push_back(y);
        t.push_back(id);
        new_goods.push(t);
    }//the info of new goods
    while(new_goods.empty()) 
    { // 把超时和换过的货物都清除掉
        vector<int> tmp = new_goods.front();
        if (tmp[2] != gds[tmp[0]][tmp[1]].put_id)
        { // 已经换过
            new_goods.pop();
        }
        else 
        {
            if (id - gds[tmp[0]][tmp[1]].put_id > 1000) 
            {
                gds[tmp[0]][tmp[1]].val = 0;
                new_goods.pop();
            }
            else 
            {
                break;
            }
        }
    }
    now_goods_num = new_goods.size();
    // 剪掉场上被运送的货物
    for(int i = 0; i < 10; i++) 
    {
        if (robot[i].goods == 1) 
        {
            now_goods_num--;
        }
        robot[i].density = 0;
    }

    queue<vector<int>> new_queue = new_goods;
    while (!new_queue.empty()) 
    {
        vector<int> tmp = new_queue.front();
        for(int i = 0; i < 10; i++) 
        {
            if (abs(tmp[0] - robot[i].x) + abs(tmp[1] - robot[i].y) <= 40) 
            {
                robot[i].density++;
            }
        }
        new_queue.pop();
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
bool is_valid(int nowx, int nowy, int nx, int ny, int roboth, int step) // 只有第一步考虑是否与机器人碰撞
{
    if (step == 0 || nowy == 149) 
    {
        for (int i = 0 ;i < roboth; i++) 
        {
            if ((nx == robot[i].nx && ny == robot[i].ny || nowx == robot[i].nx && nowy == robot[i].ny && nx == robot[i].x && ny == robot[i].y))
            {
                return false;
            }
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
    queue<pair<int, int>> q; // 使用 pair 而不是 vector<int> 来存储坐标
    q.push({robot[roboth].x, robot[roboth].y});
    path[robot[roboth].x][robot[roboth].y].lx = -1; // 使用 -1 表示起点
    path[robot[roboth].x][robot[roboth].y].ly = -1;
    vis[robot[roboth].x][robot[roboth].y] = 1; // 标记起始点为已访问
    path[robot[roboth].x][robot[roboth].y].step = 0;
    path[robot[roboth].x][robot[roboth].y].v = -1.0;
    int gx = -1, gy = -1, depth;
    double min = -1.0;
    int compare = 0;
    while (!q.empty() && depth <= 1000)
    {
        auto [x, y] = q.front(); q.pop();
        depth = path[x][y].step;
        if (gds[x][y].val != 0 && (1000 - id + gds[x][y].put_id) > depth && (gds[x][y].used == 0 || robot[roboth].tx == x && robot[roboth].ty == y))
        {
            if (robot[roboth].density >= 10) 
            {
                int val = gds[x][y].val;
                int step = path[x][y].step;
                if(path[x][y].lx == -1 && path[x][y].ly == -1)
                {
                    path[x][y].v = (double)0x7fffffff;
                }//表明机器人一开始就位于有货物的地方
                else
                {
                    path[x][y].v = double(gds[x][y].val) / step;
                    // fprintf(stderr, "%lf\n", path[x][y].v);
                }
                if(path[x][y].v > min)
                {
                    gx = x; 
                    gy = y;
                    min = path[x][y].v;
                }
                if (double(200) / step < min || compare > 10) 
                {
                    break;
                }
                compare++;
            }
            else {
                gx = x, gy = y;
                break;
            }
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
        gds[robot[roboth].tx][robot[roboth].ty].used = 0;
        gds[gx][gy].used = 1; // 将货物预定给当前机器人
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



int Is_in_berth(int x , int y) 
{
    for (int i = 0; i < 5; i++) 
    {
        if (x - berth[best_berth[i]].x <= 3 && y - berth[best_berth[i]].y <= 3  && ch[x][y] == 'B') {
            return best_berth[i]; // 真正的港口
        }
    }
    return -1;
}


int search_berth_dir(int roboth, int x , int y) 
{
    int h = get_berth_id(x, y);
    if (robot[roboth].reach_port.empty()) {
        robot[roboth].reach_port = group[h]; // Init the robot's reach_port
    }
    int flag = 0;
    for(int i:group[h]) {
        if (berth[i].used == 1) {
            flag = 1;
            break;
        }
    }
    if (flag) {
        if (berth[h].used == 1) { // Now the berth is used by boat
            return h;
        }
        else {
            return -1;
        }
    }
    else { // no berth is used
        for (int i = 0; i < 5; i++) {
            if (h == best_berth[i]) {
                return h;
            }
        }
    }
    return -1;
}


int search_boat_berth(int x , int y) 
{
    int flag = 0;
    for (int i = 0; i < 5; i++) {
        if (berth[best_berth[i]].used == 1) {
            flag = 1;
            break;
        }
    }
    if (flag == 1){
        for (int i = 0; i < 5; i++) 
        {
            if (x - berth[best_berth[i]].x >= 0 && x - berth[best_berth[i]].x <= 3 && y - berth[best_berth[i]].y >= 0 && y - berth[best_berth[i]].y <= 3 && berth[best_berth[i]].used == 1) {
                return best_berth[i]; // 真正的港口
            }
        }

    }
    else {
        for (int i = 0; i < 5; i++) 
        {
            if (x - berth[best_berth[i]].x >= 0 && x - berth[best_berth[i]].x <= 3 && y - berth[best_berth[i]].y >= 0 && y - berth[best_berth[i]].y <= 3) {
                return best_berth[i]; // 真正的港口
            }
        }
    }
    return -1;
}


int search_berth(int roboth, int zhen) 
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
    int depth = 0;
    while (!q.empty() && depth <= 840)
    {
        auto [x, y] = q.front(); q.pop();
        depth = path[x][y].step;
        if (ch[x][y] == 'B' && search_berth_dir(roboth, x, y) != -1)
        {   
            gx = x; gy = y;
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
        return -1;
    }
    if(gx != -1)
    {
        int prex = gx, prey = gy;
        // fprintf(stderr, "robot.goods : %d, gx: %d gy %d robot.x %d robot.y %d\n", robot[roboth].goods, gx, gy, robot[roboth].x, robot[roboth].y);
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

int value_sum(struct Berth k, int m)// m is margin
{
    int ans = 0;
    int l = 0;
    queue<int> tmp = k.values;
    while(!tmp.empty() && l < m)
    {
        l++;
        int d = tmp.front();
        ans += d;
        tmp.pop();
    }
    return ans;
}

double max_money;
void ssort(struct Berth *berth, int margin, int *n, int now_berth)
{
    int max_index = now_berth;
    max_money = 0;
    for(int i = 0; i < 5; i ++)
    {
        double money1 = double(value_sum(berth[best_berth[i]], margin));
        // int money2 = value_sum(berth[max_index], margin);
        if(money1 > max_money && berth[best_berth[i]].used == 0 && berth[best_berth[i]].target == 0 || berth[max_index].used == 1)
        {
            max_money = money1;
            max_index = best_berth[i];
        }
    }
    *n = max_index;
}

void ssort2(struct Berth *berth, int margin, int *n)
{
    int max_index = best_berth[0];
    max_money = value_sum(berth[best_berth[0]], margin);
    for(int i = 1; i < 5; i ++)
    {
        double money1 = double(value_sum(berth[best_berth[i]], margin));
        double max_efficiency = max_money / transport_time[max_index];
        double money1_efficiency = money1 / transport_time[best_berth[i]];
        // int money2 = value_sum(berth[max_index], margin);
        if(money1 > max_money && berth[best_berth[i]].used == 0 && berth[best_berth[i]].target == 0 || berth[max_index].used == 1)
        {
            max_money = money1;
            max_index = best_berth[i];
        }
    }
    *n = max_index;
}


int main()
{  
    Init();
    for(int zhen = 1; zhen <= 15000; zhen ++)
    {
        int id = Input();
        if (15000 - zhen >= transport_time[best_berth[0]]){
            for(int i = 0; i < robot_num; i ++)
            {   
                int mov = -1;
                if (robot[i].goods == 0) 
                { // 此时没有货物，搜索货物
                    if (now_goods_num > 0 || robot[i].tx != 209 && robot[i].ty != 209) 
                    {
                        mov = bfs(i, zhen);
                        if (mov != -1) now_goods_num--;
                    }
                    memset(vis, 0, sizeof(vis));
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
                    mov = search_berth(i, zhen); 
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
                if (mov != -1)
                {
                    printf("move %d %d\n", i, mov);
                }
                if (gds[nx][ny].val != 0 && robot[i].goods == 0) 
                {
                    printf("get %d\n", i);
                    robot[i].tx = robot[i].ty = 209;
                    robot[i].goods = 1; 
                    robot[i].value = gds[nx][ny].val;//表示已经有货物
                    gds[nx][ny].val = 0;
                    gds[nx][ny].used = 0;//表示已经拿走
                }
                else 
                {
                    if (robot[i].goods == 1 && ch[nx][ny] == 'B') 
                    { 
                        // 有货物，有港口5
                        int h = Is_in_berth(nx, ny);
                        if(h >= 0 && h <= 9) {
                            printf("pull %d\n", i);
                            robot[i].goods = 0;
                            berth[Is_in_berth(nx,ny)].values.push(robot[i].value);
                            robot[i].value = 0;
                        }
                    }
                }
            }
            for (int i = 0; i < 5; i++) 
            {
                if(boat[i].status == 1 && boat[i].pos != -1) 
                {
                    berth[boat[i].pos].used = 1;
                    if(boat[i].margin != 0 && !berth[boat[i].pos].values.empty())
                    {
                        int speed = loading_speed[boat[i].pos];
                        // fprintf(stderr, "boat[%d].margin = %d\n", i, boat[i].margin);
                        while(speed > 0 && boat[i].margin > 0 && !berth[boat[i].pos].values.empty())
                        {
                            boat[i].num += berth[boat[i].pos].values.front();
                            berth[boat[i].pos].values.pop();
                            boat[i].margin--;
                            speed--;
                        } 
                    }  
                    if (boat[i].margin > 0 && berth[boat[i].pos].values.empty()) 
                    {
                        int n;
                        berth[boat[i].pos].used = 0;
                        berth[boat[i].pos].target = 0;
                        ssort(berth, boat[i].margin, &n, boat[i].pos);
                        double target_money = double(boat[i].num + max_money) / double(500 + transport_time[n]);
                        double now_money = double(boat[i].num) / double(transport_time[boat[i].pos]);
                        if (target_money > now_money)
                        {
                            berth[boat[i].pos].used = 0;
                            printf("ship %d %d\n",i, n);
                            berth[n].target = 1;
                        }
                        else 
                        {
                            if (boat[i].margin <= (boat_capacity / 5)) 
                            {
                                boat[i].num = 0;
                                berth[boat[i].pos].used = 0;
                                berth[boat[i].pos].target = 0;
                                boat[i].margin = boat_capacity;
                                printf("go %d\n", i);
                            }
                            else 
                            {
                                berth[boat[i].pos].used = 1;
                                berth[boat[i].pos].target = 1;
                            }
                        }
                    }
                    if(boat[i].margin <= (boat_capacity / 10) || 15000 - zhen < transport_time[boat[i].pos] + 10)
                    {
                    berth[boat[i].pos].used = 0;
                    berth[boat[i].pos].target = 0;
                    printf("go %d\n", i);
                    boat[i].num = 0;
                    boat[i].margin = boat_capacity;
                    }
                }

                if(boat[i].status == 1 && boat[i].pos == -1 && zhen > 2) 
                {
                    int n;
                    ssort2(berth, boat[i].margin, &n);
                    printf("ship %d %d\n",i, n);
                    berth[n].target = 1;
                }
        
                if(boat[i].status == 1 && boat[i].pos == -1 && zhen == 2) 
                {
                    for (int i = 0; i < 5; i++) 
                    {
                        printf("ship %d %d\n", i, best_berth[i]);
                        berth[best_berth[i]].target = 1;
                    }
                }
            }
        }
        else {
            for (int i = 0; i < 10; i++){
                printf("mov %d %d\n", i, 1);
            }
        }
        puts("OK");
        fflush(stdout);
    }
    return 0;
}