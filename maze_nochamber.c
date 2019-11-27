#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define WALL '#'
#define ROAD ' '
#define DOOR ' '
#define PATH '.'
#define POSITION 'X'
#define SPEED 50
#define PERCENT 10
#define Entrance 0
#define Exit 1
#define Continue 1
#define Break 0
#define OK 1
#define NOT_OK 0

int **creat_area(int, int);                                      //創建迷宮空間
int **creat_direction();                                         //創建方向陣列
int build_maze(int **, int, int);                                //創建迷宮
int build_maze_wall(int **, int, int);                           //蓋出牆
int build_maze_road(int **, int, int);                           //破牆造路
int rand_wall(int *, int, int);                                  //在迷宮內隨機選擇一面牆
int cover_road(int **, int, int, int, int, int);                 //覆蓋編號，造路
int max_num_around_point(int **, int, int);                      //取得指定點四周最大編號
int min_num_around_point(int **, int, int, int **);              //取得指定點四周最小編號
int check_maze(int **, int, int);                                //檢查迷宮是否完成(所有編號只剩最大值編號)
int check_wall(int **, int, int, int, int);                      //檢查牆是否符合標準
int check_wall_around(int **, int, int, int, int);               //判斷柱子四周是否符合標準(破壞欲破壞之牆後不可只剩自己)
int break_chamber(int **, int, int, int);                        //循序打破所有密室
int rand_wall_destructible(int **, int *, int, int, int, int);   //尋找可破壞之牆
int print_maze(int **, int, int);                                //顯示迷宮(圖形)
int rand_door(int **, int, int);                                 //隨機創建出入口
int print_maze_number(int **, int, int);                         //顯示陣列(數字顯示，找bug用)
int find_road(int **, int, int);                                 //尋找出路
int walk_wall_follower(int **, int, int, int, int, int **, int); //走迷宮-跟蹤牆面法
int degule(int **, int, int, int **);                            //洪水演算法
int find_door(int **, int, int, int *, int);                     //尋找門口
int initialization_maze(int **, int, int);                       //將所有編號歸0
int flood(int **, int, int, int, int, int);                      //放水流
int get_direction(int **, int, int, int, int, int **, int); //取得周圍最小編號中的亂數方向
int walk_degule(int **, int, int, int, int, int, int **);        //走迷宮-洪水演算法
int direction_caculate(int, int);                                //計算方向轉動，1~3循環
int free_array(int **, int, int);                                //釋放maze陣列
void gotoxy(int, int);

int main()
{
    srand(time(NULL));
    int x_length, y_length; //x軸走道數, y軸走道數
    printf("Please enter the x and y length: ");
    scanf("%d %d", &x_length, &y_length);
    getchar();
    int array_x_length = x_length * 2 + 1, array_y_length = y_length * 2 + 1; //二維陣列長寬
    int **maze = creat_area(array_x_length, array_y_length);                  //創建maze二維陣列
    build_maze(maze, array_x_length, array_y_length);                         //建造迷宮
    rand_door(maze, array_x_length, array_y_length);                          //隨機選擇出入口位置(入口在左，出口在右)
    gotoxy(0, 1);
    print_maze(maze, array_x_length, array_y_length); //顯示迷宮
    find_road(maze, array_x_length, array_y_length);  //找到走出去的路
    free_array(maze, array_x_length, array_y_length); //釋放malloc的陣列
    gotoxy(0, array_y_length + 1);
    getchar();
}
int find_road(int **maze, int array_x_length, int array_y_length) //尋找出路
{
    int **direction_array = creat_direction();
    //walk_wall_follower(maze, array_x_length, array_y_length, 0, y, direction, 0); //走迷宮-跟蹤牆面
    degule(maze, array_x_length, array_y_length, direction_array); //走迷宮-洪水演算法
    for (int i = 0; i < 4; i++)                                    //釋放malloc的陣列
    {
        free(direction_array[i]);
    }
    free(direction_array);
}

int walk_wall_follower(int **maze, int array_x_length, int array_y_length,      //走迷宮-跟蹤牆面法
                       int x, int y, int **direction_coordinate, int direction) //x:走路者的x座標, y:走路者的y座標, direction[][]:各方向的座標, direction:0~3各自代表右下左上
{
    while (x != array_x_length - 1) //走到最右邊結束
    {
        int next_direction = direction_caculate(direction, 1);    //方向轉動一個順位(順時針轉90度)
        int next_x = x + direction_coordinate[next_direction][0]; //設定下一個位置
        int next_y = y + direction_coordinate[next_direction][1];
        if (maze[next_x][next_y] != -1) //若行走者右邊沒有牆壁則移動
        {
            direction = next_direction;
        }
        else //若右邊有牆
        {
            do //逆時針旋轉直到面對方向沒牆
            {
                next_direction = direction_caculate(next_direction, 3); //逆時針旋轉
                next_x = x + direction_coordinate[next_direction][0];
                next_y = y + direction_coordinate[next_direction][1];
            } while (maze[next_x][next_y] == -1);
            direction = next_direction;
        }
        gotoxy(x * 2, y + 1);                    //移動游標至原行走者的座標
        printf(".");                             //顯示為'.'
        maze[x][y] = -2;                         //迷宮矩陣中 該位置改為-2(-2代表路徑)
        x += direction_coordinate[direction][0]; //行走者座標移動
        y += direction_coordinate[direction][1];
        maze[x][y] = -3;      //迷宮矩陣中 該位置改為-3(-3代表行走者位置)
        gotoxy(x * 2, y + 1); //移動游標至新的行走者的座標
        printf("X");          //顯示為'X'
        Sleep(SPEED);         //停止SPEED*0.001秒
    }
}

int degule(int **maze, int array_x_length, int array_y_length, int **direction_array) //洪水演算法
{
    int exit[2];                                                                                     //出口座標陣列
    find_door(maze, array_x_length, array_y_length, exit, Exit);                                     //找出口座標
    initialization_maze(maze, array_x_length, array_y_length);                                       //初始化整個迷宮(所有編號歸0)
    flood(maze, array_x_length, array_y_length, exit[0], exit[1], 1);                                //放水流(開始編新編號)
    int entrance[2];                                                                                 //入口座標陣列
    find_door(maze, array_x_length, array_y_length, entrance, Entrance);                             //找入口座標
    walk_degule(maze, array_x_length, array_y_length, entrance[0], entrance[1], 1, direction_array); //開始行走
}

int walk_degule(int **maze, int array_x_length, int array_y_length, int x, int y, int count, int **direction_array)//走迷宮-洪水演算法
{
    for (int i = maze[x][y]; i > 1; i--)//從編號最大的倒數行走，直到走到0
    {
        int direction = get_direction(maze, array_x_length, array_y_length, x, y, direction_array, i - 1);//取得下一個行走方向(附近編號為i-1的位置)
        Sleep(SPEED);//停止SPEED*0.001秒
        gotoxy(0 ,array_y_length+1);
        printf("(%d, %d)     ", x, y);
        gotoxy(x * 2, y + 1);//移動至畫面中(x, y)的位置
        printf(".");//輸出'.'(路徑)
        x += direction_array[direction][0];//移動至下一個x, y
        y += direction_array[direction][1];
        gotoxy(x * 2, y + 1);//移動至畫面中(x, y)的位置
        printf("X");//輸出'X'(行走者)
    }
}

int get_direction(int **maze, int array_x_length, int array_y_length, int x, int y, int **direction_array, int count)//取得行走方向
{
    int i;
    for (i = 0; i < 4; i++)//四個方向依序查找
    {
        if (maze[x + direction_array[i][0]][y + direction_array[i][1]] == count)//若方向編號等於欲行走之編號則跳出迴圈
        {
            break;
        }
    }
    return i;//回傳方向
}

int initialization_maze(int **maze, int array_x_length, int array_y_length)//初始化整個迷宮(所有編號歸0)
{
    for (int i = 0; i < array_x_length; i++)
    {
        for (int j = 0; j < array_y_length; j++)
        {
            if (maze[i][j] > 0)//非牆面皆歸0
                maze[i][j] = 0;
        }
    }
}

int find_door(int **maze, int array_x_length, int array_y_length, int *door, int exit_entrance)//尋找門口
{
    if (exit_entrance == Exit)
    {
        door[0] = array_x_length - 1;//若為出口則x座標在最右邊
    }
    else if (exit_entrance == Entrance)
    {
        door[0] = 0;//若為入口則x座標在最左邊
    }
    for (int i = 0; i < array_y_length && maze[door[0]][i] == -1; i++)//尋找直到非牆
    {
        door[1]++;
    }
}

int flood(int **maze, int array_x_length, int array_y_length,//放水流
          int x, int y, int count)
{
    if (x > array_x_length - 1 || x < 0 ||//若超出範圍則引爆
        y > array_y_length - 1 || y < 0)
    {
        return 0;
    }
    if ((count >= maze[x][y] && maze[x][y] != 0) || maze[x][y] == -1)//(若位置上的編號比水流的編號小，且不是牆壁)(或是遇到牆壁)引爆
    {
        return 0;
    }
    maze[x][y] = count;//將水流編號指派到位置上
    flood(maze, array_x_length, array_y_length, x + 1, y, count + 1);//往右流
    flood(maze, array_x_length, array_y_length, x - 1, y, count + 1);//往左流
    flood(maze, array_x_length, array_y_length, x, y + 1, count + 1);//往下流
    flood(maze, array_x_length, array_y_length, x, y - 1, count + 1);//往上游
    return 0;//結束
}

int **creat_direction()
{
    int **direction = malloc(sizeof(int *) * 4); //指方向的二維陣列
    for (int i = 0; i < 4; i++)
    {
        direction[i] = malloc(sizeof(int) * 2);
    }
    direction[0][0] = 1; //依照順序(右 下 左 上)
    direction[0][1] = 0;
    direction[1][0] = 0;
    direction[1][1] = 1;
    direction[2][0] = -1;
    direction[2][1] = 0;
    direction[3][0] = 0;
    direction[3][1] = -1;
    return direction;
}

int direction_caculate(int direction, int i) //計算方向轉動，1~3循環
{
    return (direction + i) % 4;
}

int **creat_area(int array_x_length, int array_y_length) //創建迷宮空間
{
    int **maze = malloc(sizeof(int *) * array_x_length);
    for (int i = 0; i < array_x_length; i++)
    {
        maze[i] = malloc(sizeof(int) * array_y_length);
    }
    return maze;
}

int build_maze(int **maze, int array_x_length, int array_y_length) //創建迷宮
{
    build_maze_wall(maze, array_x_length, array_y_length); //蓋出牆，放置編號
    build_maze_road(maze, array_x_length, array_y_length); //破壞牆壁，建造道路
}

int build_maze_wall(int **maze, int array_x_length, int array_y_length) //蓋出牆
{
    int count = 1; //編號
    for (int i = 0; i < array_y_length; i++)
    {
        for (int j = 0; j < array_x_length; j++)
        {
            if (i % 2 == 0) //偶數行每個都是牆
            {
                maze[j][i] = -1;
            }
            else //奇數行中
            {
                if (j % 2 == 0) //偶數個是牆
                {
                    maze[j][i] = -1;
                }
                else //奇數個為編號
                {
                    maze[j][i] = count++; //指派完數字，count+1
                }
            }
        }
    }
    return 0;
}

int build_maze_road(int **maze, int array_x_length, int array_y_length) //破牆造路
{
    int total = (array_x_length - 2) * (array_y_length / 2) - (array_x_length / 2); //計算總體牆數(格子間)
    for (int i = 0; i < total * PERCENT / 100 && check_maze(maze, array_x_length, array_y_length) == Continue; i++)
    {                //破牆至整體牆數量的PERXENT%，或是整個迷宮已經完成
        int wall[2]; //宣告wall陣列，wall[0]:破壞目標的x座標，wall[1]:破壞目標的y座標
        do
        {
            rand_wall(wall, array_x_length, array_y_length);                                      //隨機選擇牆壁
        } while (check_wall(maze, wall[0], wall[1], array_x_length, array_y_length) == Continue); //若選擇的牆壁未符合條件則繼續
        int max = max_num_around_point(maze, wall[0], wall[1]);                                   //max:欲破壞之牆四周最大編號
        maze[wall[0]][wall[1]] = 0;                                                               //將欲破壞之牆的位置設定為0，方便之後遞迴造路
        cover_road(maze, array_x_length, array_y_length, max, wall[0], wall[1]);                  //造路(將牆打通後，區域內編號改為最大編號)
    }
    for (int i = 0; check_maze(maze, array_x_length, array_y_length) == Continue; i++) //直到所有方塊編號都為最大值為止
    {
        break_chamber(maze, array_x_length, array_y_length, i % 2); //循序打破所有密室
    }
}

int break_chamber(int **maze, int array_x_length, int array_y_length, int i) //循序打破所有密室
{
    int num = (array_x_length / 2) * (array_y_length / 2), wall[2]; //num:整個迷宮最大編號，wall:欲破壞之牆ㄉ座標
    switch (i)
    {
    case 0: //偶數次橫向循序破壞
        for (int i = 1; i < array_y_length; i += 2)
        {
            int num = maze[1][i]; //num:上一格的編號，每排首位設為初始值
            for (int j = 1; j < array_x_length; j += 2)
            {
                if (maze[j][i] != num) //若此格與上一格編號不同，則破壞此格四周隨機一個牆壁，並將區間內所有編號變為區間內最大值
                {
                    rand_wall_destructible(maze, wall, array_x_length, array_y_length, i, j); //找到附近可破壞的牆壁
                    int max = max_num_around_point(maze, wall[0], wall[1]);                   //取得欲破壞之牆四周最大編號
                    maze[wall[0]][wall[1]] = 0;                                               //將欲破壞之牆的位置設為0，方便遞迴取代編號
                    cover_road(maze, array_x_length, array_y_length, max, wall[0], wall[1]);  //覆蓋編號，建造道路
                }
                num = maze[j][i]; //每排首位以自己最基準值
            }
        }
        break;
    case 1: //奇數次縱向循序破壞
        for (int i = 1; i < array_x_length; i += 2)
        {
            int num = maze[i][1];
            for (int j = 1; j < array_y_length; j += 2)
            {
                if (maze[i][j] != num)
                {
                    rand_wall_destructible(maze, wall, array_x_length, array_y_length, j, i);
                    int max = max_num_around_point(maze, wall[0], wall[1]);
                    maze[wall[0]][wall[1]] = 0;
                    cover_road(maze, array_x_length, array_y_length, max, wall[0], wall[1]);
                }
                num = maze[i][j];
            }
        }
        break;
    }
}

int rand_wall_destructible(int **maze, int *wall, int array_x_length, int array_y_length, int x, int y) ////尋找可破壞之牆
{
    do
    {
        wall[0] = rand() % 3 + y - 1; //隨機選擇牆壁四個方向的位置
        if (wall[0] == y)
        {
            wall[1] = (rand() % 2) * 2 + x - 1;
        }
        else
        {
            wall[1] = x;
        }
    } while (check_wall(maze, wall[0], wall[1], array_x_length, array_y_length) == Continue); //若隨機的牆不符合標準，繼續尋找
}

int check_wall(int **maze, int x, int y, int array_x_length, int array_y_length) //檢查牆是否符合標準
{
    if (x <= 0 || x >= array_x_length - 1 || y <= 0 || y >= array_y_length - 1) //若位置超過範圍，繼續尋找
    {
        return Continue;
    }
    if (maze[x][y] != -1) //若位置非牆，繼續尋找
    {
        return Continue;
    }
    maze[x][y] = 0; //將牆設為0方便之後判斷
    if (y % 2 == 0) //若為偶數行
    {
        if (check_wall_around(maze, x + 1, y, array_x_length, array_y_length) == OK &&
            check_wall_around(maze, x - 1, y, array_x_length, array_y_length) == OK) //左右兩個柱子符合標準
        {
            maze[x][y] = -1; //還原牆壁
            return Break;    //破掉尋找的迴圈
        }
    }
    else
    {
        if (check_wall_around(maze, x, y + 1, array_x_length, array_y_length) == OK &&
            check_wall_around(maze, x, y - 1, array_x_length, array_y_length) == OK) //上下兩個柱子符合標準
        {
            maze[x][y] = -1; //還原牆壁
            return Break;    //破掉尋找的迴圈
        }
    }
    maze[x][y] = -1; //若都沒達標準，還原牆壁
    return Continue; //繼續尋找
}

int check_wall_around(int **maze, int x, int y, int array_x_length, int array_y_length) //判斷柱子四周是否符合標準(破壞欲破壞之牆後不可只剩自己)
{
    if (x + 1 >= array_x_length - 1 || x - 1 <= 0 || y + 1 >= array_y_length - 1 || y - 1 <= 0) //若在邊界則可以破壞
    {
        return OK;
    }
    else //若不在邊界
    {
        if (maze[x + 1][y] == -1 || maze[x - 1][y] == -1 || maze[x][y + 1] == -1 || maze[x][y - 1] == -1) //若四周有一面牆以上則可以破壞
        {
            return OK;
        }
        else //反之不可破壞
        {
            return NOT_OK;
        }
    }
}

int check_maze(int **maze, int array_x_length, int array_y_length) //檢查迷宮是否完成(所有編號只剩最大值編號)
{
    int num = maze[1][1]; //設第一格編號為num
    for (int i = 1; i < array_x_length; i += 2)
    {
        for (int j = 1; j < array_y_length; j += 2)
        {
            if (maze[i][j] != num) //只要有一數字不同於num，迷宮未完成
            {
                return Continue; //繼續迴圈
            }
        }
    }
    return Break; //若都沒數字不同於num，迷宮已完成，破壞迴圈
}

int cover_road(int **maze, int array_x_length, int array_y_length, int num, int x, int y) //覆蓋編號，造路
{
    if (maze[x][y] == -1 || maze[x][y] == num) //若遇牆或遇到同編號
    {
        return 0; //結束函數
    }
    maze[x][y] = num;                                                //將(x,y)設為最大值編號
    cover_road(maze, array_x_length, array_y_length, num, x + 1, y); //往右遞迴
    cover_road(maze, array_x_length, array_y_length, num, x - 1, y); //往左遞迴
    cover_road(maze, array_x_length, array_y_length, num, x, y + 1); //往上遞迴
    cover_road(maze, array_x_length, array_y_length, num, x, y - 1); //往下遞迴
    return 0;                                                        //結束函數
}

int max_num_around_point(int **maze, int x, int y) //取得指定點四周最大編號
{
    int max = maze[x + 1][y];
    if (maze[x - 1][y] > max)
    {
        max = maze[x - 1][y];
    }
    if (maze[x][y + 1] > max)
    {
        max = maze[x][y + 1];
    }
    if (maze[x][y - 1] > max)
    {
        max = maze[x][y - 1];
    }
    return max; //反傳最大值
}

int min_num_around_point(int **maze, int x, int y, int **direction_array) //取得指定點四周最小編號
{
    int min, i, next_x, next_y;
    for (i = 0; i < 4; i++)//先找到非牆面的編號，並將其值指派給min
    {
        next_x = x + direction_array[i][0];
        next_y = y + direction_array[i][1];
        if (maze[next_x][next_y] != -1)
        {
            min = maze[next_x][next_y];
            break;
        }
    }
    for (; i < 4; i++)//接續查找，若有比min更小的數字，則指派給min
    {

        next_x = x + direction_array[i][0];
        next_y = y + direction_array[i][1];
        if (maze[next_x][next_y] < min && maze[next_x][next_y] != -1)
        {
            min = maze[next_x][next_y];
        }
    }
    return min; //反傳最小值
}

int rand_wall(int *wall, int array_x_length, int array_y_length) //在迷宮內隨機選擇一面牆
{
    wall[0] = rand() % (array_x_length - 2) + 1;
    if (wall[0] % 2 == 1)
    {
        wall[1] = rand() % (array_y_length / 2 - 1) * 2 + 2;
    }
    else
    {
        wall[1] = rand() % (array_y_length / 2) * 2 + 1;
    }
}

int print_maze(int **maze, int array_x_length, int array_y_length) //顯示迷宮(圖形)
{
    for (int i = 0; i < array_y_length; i++)
    {
        for (int j = 0; j < array_x_length; j++)
        {
            switch (maze[j][i])
            {
            case -3:
                printf("%c ", POSITION);
                break;
            case -2:
                printf("%c ", PATH);
                break;
            case -1:
                printf("%c ", WALL);
                break;
            case 0:
                printf("%c ", DOOR);
                break;
            default:
                printf("%c ", ROAD);
            }
        }
        printf("\n");
    }
}

int rand_door(int **maze, int array_x_length, int array_y_length) //隨機創建出入口
{
    int x, y;
    switch (0)
    {
    case 0://創建在左右
        do//隨機選擇最左邊的一個位置，右邊有路的(避免之後改成同個牆面進出，多增加一個判斷：不能同個位置取門)
        {
            y = rand() % (array_y_length - 2) + 1;
        } while (maze[1][y] == -1 || maze[0][y] == 0);
        maze[0][y] = 0;
        do//隨機選擇最右邊的一個位置
        {
            y = rand() % (array_y_length - 2) + 1;
        } while (maze[array_x_length - 2][y] == -1 || maze[array_x_length - 1][y] == 0);
        maze[array_x_length - 1][y] = 0;
        break;
    case 1://創建在上下
        do//隨機選擇最下面的位置
        {
            x = rand() % (array_x_length - 2) + 1;
        } while (maze[x][array_y_length - 2] == -1 || maze[x][array_y_length - 1] == 0);
        maze[x][array_y_length - 1] = 0;
        do//隨機選擇最上面的位置
        {
            x = rand() % (array_x_length - 2) + 1;
        } while (maze[x][1] == -1 || maze[x][0] == 0);
        maze[x][0] = 0;
        break;
    }
}

int print_maze_number(int **maze, int array_x_length, int array_y_length) //顯示陣列(數字顯示，找bug用)
{
    printf("\n");
    for (int i = 0; i < array_y_length; i++)
    {
        for (int j = 0; j < array_x_length; j++)
        {
            switch (maze[j][i])
            {
            case -1://牆面
                printf(" %c ", WALL);
                break;
            case 0://門口
                printf(" %c ", DOOR);
                break;
            default://編號
                printf("%-3d", maze[j][i]);
            }
        }
        printf("\n");
    }
}

int free_array(int **maze, int array_x_length, int array_y_length) //釋放maze陣列
{
    for (int i = 0; i < array_x_length; i++)
    {
        free(maze[i]);
    }
    free(maze);
}

void gotoxy(int xpos, int ypos)
{
    COORD scrn;
    HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
    scrn.X = xpos;
    scrn.Y = ypos;
    SetConsoleCursorPosition(hOuput, scrn);
}
