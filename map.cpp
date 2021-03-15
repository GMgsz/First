#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX 100

int STATION_NUM;//站点总数
char *STATION[MAX];//存储站点名称

int ROUTE_NUM;//路段总数
int ROUTES[MAX][4];//存储路段信息

int BUS_NUM;//公交线路总数
char *BUS_NAME[MAX];//存储公交线路名称
int BUSES[MAX][3];//存储公交线路信息

void Read1(){//从文件中读取路段信息保存到ROUTES数组
    FILE *fp;
    fp = fopen("routes.txt","r+");
    if(fp == NULL){
        printf("打开文件失败!\n");
        return;
    }
    fscanf(fp,"%d",&ROUTE_NUM);
    int i;
    for(i = 0;i < ROUTE_NUM;i++)
        fscanf(fp,"%d%d%d%d",&ROUTES[i][0],&ROUTES[i][1],&ROUTES[i][2],&ROUTES[i][3]);
    fclose(fp);
}

void Read2(){//从文件中读取站点名称保存到STATION数组
    FILE *fp;
    fp = fopen("stations.txt","r+");
    if(fp == NULL){
        printf("打开文件失败!\n");
        return;
    }
    char str[30];
    int i = 0,len;
    while(fgets(str,30,fp) != NULL){
        len = strlen(str);
        if(str[len-1] == '\n')
            str[len-1] = '\0';
        char *pstr = (char*)malloc((len+1)*sizeof(char));
        strcpy(pstr,str);
        STATION[i] = pstr;//直接等于str为什么不行
        i++;
    }
    STATION_NUM = i;
    fclose(fp);
}

void Read3(){//从文件中读取公交信息保存到BUSES数组
    FILE *fp;
    fp = fopen("buses.txt","r+");
    if(fp == NULL){
        printf("打开文件失败!\n");
        return;
    }
    fscanf(fp,"%d",&BUS_NUM);

    int i;
    for(i = 0;i < BUS_NUM;i++){
        fscanf(fp,"%d%d%d",&BUSES[i][0],&BUSES[i][1],&BUSES[i][2]);
    }
    fclose(fp);
}

void Read4(){//从文件中读取公交名称保存到BUS_NAME数组
    FILE *fp;
    fp = fopen("bus_name.txt","r+");
    if(fp == NULL){
        printf("打开文件失败!\n");
        return;
    }
    char str[30];
    int i = 0,len;
    while(fgets(str,30,fp) != NULL){
        len = strlen(str);
        if(str[len-1] == '\n')
            str[len-1] = '\0';
        char *pstr = (char*)malloc((len+1)*sizeof(char));
        strcpy(pstr,str);
        BUS_NAME[i] = pstr;
        i++;
    }
    fclose(fp);
    //for(i=0;i<BUS_NUM;i++)
    //    printf("%s\n",BUS_NAME[i]);
}

void ReadFile(){//读取文件内容保存在数组
    Read1();
    Read2();
    Read3();
    Read4();
}

typedef struct Bus{
    char *name;//公交名
    int start; //起点
    int end;   //终点
}Bus;

typedef struct Route{
    int station; //指向的站点索引号
    int bus;     //公交索引号
    int distance;//两站之间公路的距离
    struct Route*next;//起始站点相同的，下一条下行路线
}Route;

typedef struct Station{
    char *station;//站点名
    struct Route*routes;//从该站点出发的所有下行路线的链域
}Station;

typedef struct BusMap{
    Bus *buses;//公交线路数组
    Station *stations;//站点数组
    int station_num;//站点数
    int bus_num;//公交线路数
}BusMap;

BusMap g_BusMap;//全局变量,保存公交地图信息

void CreateMap(){//创建公交线路图
    int i;
    g_BusMap.bus_num = BUS_NUM;
    g_BusMap.buses = (Bus*)malloc(sizeof(Bus)*BUS_NUM);
    for(i = 0;i < BUS_NUM;i++){
        g_BusMap.buses[i].name = BUS_NAME[i];
        g_BusMap.buses[i].start = -1;
        g_BusMap.buses[i].end = -1;
    }
    g_BusMap.station_num = STATION_NUM;
    g_BusMap.stations = (Station*)malloc(sizeof(Station)*STATION_NUM);
    for(i = 0;i < STATION_NUM;i++){
        g_BusMap.stations[i].station = STATION[i];
        g_BusMap.stations[i].routes = NULL;
    }
}

int FindBus(char *bus){//查找公交是否存在，有则返回索引号
    for(int i = 0;i < g_BusMap.bus_num;i++){
        if(strcmp(g_BusMap.buses[i].name,bus) == 0){
            return i;
        }
    }
    return -1;
}

int GetBus(char *bus){//若无此公交则创建后返回索引号
    int nBus = FindBus(bus);
    if(nBus == -1){
        g_BusMap.buses = (Bus*)realloc(g_BusMap.buses,sizeof(Bus)*(g_BusMap.bus_num+1));
        Bus *pBus = g_BusMap.buses+g_BusMap.bus_num;
        pBus->name = bus;
        pBus->start = -1;
        pBus->end = -1;
        nBus = g_BusMap.bus_num;
        g_BusMap.bus_num++;//BUS_NUM没加，未同步处理
    }
    return nBus;
}

int FindStation(char *station){//查找站点是否存在，有则返回索引号
    for(int i = 0;i < g_BusMap.station_num;i++){
        if(strcmp(g_BusMap.stations[i].station,station) == 0)
            return i;
    }
    return -1;
}

int GetStation(char *station)//若无此站点则创建后返回索引号
{
    int nStation = FindStation(station);
    if(nStation == -1){
        g_BusMap.stations = (Station*)realloc(g_BusMap.stations,sizeof(Station)*(g_BusMap.station_num+1));
        Station *pStation = g_BusMap.stations+g_BusMap.station_num;
        pStation->station = station;
        pStation->routes = NULL;
        nStation = g_BusMap.station_num;
        g_BusMap.station_num++;
    }
    return nStation;
}

void AddBus(int bus,int pStart,int pEnd){//添加公交
    int nBus = GetBus(BUS_NAME[bus]);
    int nStart = GetStation(STATION[pStart]);
    int nEnd = GetStation(STATION[pEnd]);
    Bus *pBus = g_BusMap.buses+nBus;//&
    pBus->start = nStart;
    pBus->end = nEnd;
}

void AddRoute(int pBus,int pStart,int pEnd,int distance){//添加路段
    Station *pStStation = &g_BusMap.stations[pStart];
    Route *pStRoute = pStStation->routes;

    while(pStRoute != NULL&&pStRoute->next != NULL){
        //判断该边是否已经存在，如果已经存在，则不插入
        if(pStRoute->bus == pBus&&pStRoute->station == pEnd)
            return;
        pStRoute = pStRoute->next;
    }
    //创建新的路线
    Route *pNewRoute = (Route*)malloc(sizeof(Route));
    pNewRoute->bus = pBus;
    pNewRoute->station = pEnd;
    pNewRoute->distance = distance;
    pNewRoute->next = NULL;
    //若是起始顶点的第一条边
    if(pStRoute == NULL)
        pStStation->routes = pNewRoute;
    else
        pStRoute->next = pNewRoute;
}

void LoadMapDate(){//加载公交线路信息
    int i;
    for(i = 0;i < BUS_NUM;i++)
        AddBus(BUSES[i][0],BUSES[i][1],BUSES[i][2]);
    for(i = 0;i < ROUTE_NUM;i++)
        AddRoute(ROUTES[i][0],ROUTES[i][1],ROUTES[i][2],ROUTES[i][3]);
}

int CheckSame(int *buses,int name,int n){//是否与前面的数组元素相同
    int i;
    for(i = 0;i < n;i++)
        if(buses[i] == name)
            break;
    if(i == n)
        return 0;
    else
        return 1;
}

void QueryBus(char *pBus){//查询公交
    int nBus = FindBus(pBus);
    int nStart = g_BusMap.buses[nBus].start;
    int nEnd = g_BusMap.buses[nBus].end;

    int route[MAX];
    route[0] = nStart;
    Station *pStation = &g_BusMap.stations[nStart];
    Route *pRoute = pStation->routes;//选择其下行路线和站点是否有问题？需检查
    int nStation = nStart;
    int num = 1;
    while(nStation != nEnd){
        //代码可优化
        while(pRoute->bus != nBus){//不是该公交线路，则继续寻找
            pRoute = pRoute->next;
        }
        nStation = pRoute->station;
        //printf("%d\n",nStation);
        route[num] = nStation;
        num++;
        pStation = &g_BusMap.stations[nStation];
        pRoute = pStation->routes;
    }
    //输出各站点
    printf("#%s# 从 *%s* 开往 *%s* 共经过%d个站点:\n",pBus,g_BusMap.stations[nStart].station,g_BusMap.stations[nEnd].station,num);
    printf("\n");
    printf("%s",g_BusMap.stations[route[0]].station);
    for(int i = 1;i < num;i++){
        printf("-->%s",g_BusMap.stations[route[i]].station);
    }
    printf("\n\n");
}

int QueryStation(char *station){//查询站点
    int nSta = FindStation(station);
    int nBus,i;
    int bus[MAX];
    Station *pstation = &g_BusMap.stations[nSta];
    Route *pRoute = pstation->routes;
    int num = 0;

    //遍历该站点邻接表找到所有从该站点驶出的车
    while(pRoute != NULL){
        nBus = pRoute->bus;     //找到公交索引号
        pRoute = pRoute->next;
        bus[num] = nBus;
        num++;
    }

    //遍历所有邻接点找到所有驶入该站点的车
    for(i = 0;i < g_BusMap.station_num;i++){
        Station *qstation = &g_BusMap.stations[i];
        Route *qRoute = qstation->routes;
        while(qRoute != NULL){
            if(qRoute->station == nSta){
                nBus = qRoute->bus;   //找到公交索引号
                if(CheckSame(bus,nBus,num) == 0){//检查公交索引号是否已经被记录
                    *(bus+num) = nBus;
                    num++;
                }
            }
            qRoute = qRoute->next;
        }
    }
    printf("[%s] 共有%d辆车经过:\n",station,num);
    for(i = 0;i < num;i++){
        printf("%s\n",g_BusMap.buses[bus[i]].name);
    }
    printf("\n");
    return num;
}

typedef struct Path  //只有一个path结点
{
    Route route[MAX];
    int transfer;    //换乘次数
    int alldist;    //路线总路程
    int top;
}Path;

bool visited[MAX]; //记录站点是否被访问过
int sum = 0; //记录符合要求的路线数,初始为0

void ClearVisited(){   //将visited数组初始化为false
    for(int i = 0;i < STATION_NUM;i++)
        visited[i] = false;
}

void Init(Path *&p,char *s){  //先初始化
    p = (Path*)malloc(sizeof(Path));
    p->route[0].bus = -1;
    p->route[0].distance = 0;
    p->route[0].next = NULL;
    p->route[0].station = FindStation(s);
    p->transfer = -1;
    visited[FindStation(s)] = true;
    p->top = 0;
    p->alldist = 0;
}

void Push(Path *&p,Route *s){ //将路段压入栈,记录相关信息
    if(p->route[p->top].bus != s->bus)
        p->transfer++;
    p->top++;
    p->route[p->top].bus = s->bus;
    p->route[p->top].distance = s->distance;
    p->route[p->top].next = s->next;
    p->route[p->top].station = s->station;
    p->alldist += s->distance;
}

void Pop(Path *&p,Route *&s){ //将路段从栈弹出
    s = (Route*)malloc(sizeof(Route));
    s->bus = p->route[p->top].bus;
    s->distance = p->route[p->top].distance;
    s->next = p->route[p->top].next;
    s->station = p->route[p->top].station;
    p->alldist -= s->distance;     //减掉此段路线的路程
    p->top--;
    if(p->route[p->top].bus!=s->bus)
        p->transfer--;
}

void PrintPath(Path *p){  //输出可行线路
    if(p->transfer <= 1){
        printf("%s",g_BusMap.stations[p->route[0].station].station);
        for(int i = 1;i <= p->top;i++){
            printf("--[%s%dm]-->%s",g_BusMap.buses[p->route[i].bus].name,p->route[i].distance,g_BusMap.stations[p->route[i].station].station);
        }
        sum++;
        printf("\n");
        printf("该路线总路程:%dm 换乘次数:%d\n\n",p->alldist,p->transfer);
    }
}

Path *path;

void QueryPath(char *pStart,char *pEnd)
{
    int nStart = GetStation(pStart);
    int nEnd = GetStation(pEnd);
    int n = nStart;
    Station *pStation = &g_BusMap.stations[nStart];
    Route *pRoute = pStation->routes;
    if(n == nEnd)
        PrintPath(path);
    else{
        while(pRoute){//关键代码
            if(!visited[pRoute->station]){
                visited[pRoute->station] = true;
                Push(path,pRoute);
                QueryPath(g_BusMap.stations[pRoute->station].station,pEnd);//递归函数，向前继续搜索
            }
            pRoute = pRoute->next;//其他公交线路
        }
    }
    Pop(path,pRoute);
    visited[pRoute->station] = false;
}

void Function1(){
    char bus[20];
    printf("------查询公交线路------\n");
    printf("请输入要查询的公交名:");
    scanf("%s",bus);
    if(FindBus(bus) == -1)
        printf("输入的公交名有错误!\n\n");
    else{
        printf("------------------------\n");
        QueryBus(bus);
    }
}

void Function2(){
    char station[20];
    printf("------查询站点信息------\n");
    printf("请输入要查询的站点名:");
    scanf("%s",station);
    if(FindStation(station) == -1)
        printf("输入的站点名有错误!\n\n");
    else{
        printf("------------------------\n");
        QueryStation(station);
    }
}

void Function3(){
    char start[20];
    char end[20];
    printf("\n------查询两个站点公交线路<最多换乘1次>------\n");

    printf("请输入要查询的起点:");
    scanf("%s",start);
    printf("请输入要查询的终点:");
    scanf("%s",end);
    if(FindStation(start) == -1||FindStation(end) == -1){
        printf("\n输入的起始站点有错误!\n\n");
        return;
    }
    ClearVisited();
    Init(path,start);
    printf("从 *%s* 开往 *%s* 找到以下路线:\n\n",start,end);
    QueryPath(start,end);

    if(sum != 0){
        printf("总共有 %d 条路线符合要求!\n\n",sum);
        sum = 0;
    }
    else
        printf("未找到符合要求的路线!\n\n");
    free(path);
}

void CreateAndLoad(){
    CreateMap();
    LoadMapDate();
}

void AddStart(char *bus){
    int distance;
    char start[30];
    int pBus = FindBus(bus);//公交线路的索引号
    int pstart = g_BusMap.buses[pBus].start;//原来公交线路的起点索引号
    printf("请输入新起点:");
    scanf("%s",start);
    printf("\n请输入新起点到原来起点的距离:");
    scanf("%d",&distance);
    int newStart = GetStation(start);
    printf("%d\n",newStart);
    printf("%s\n",g_BusMap.stations[newStart].station);
    AddRoute(pBus,newStart,pstart,distance);
    printf("%d\n",g_BusMap.buses[pBus].start);
    g_BusMap.buses[pBus].start = newStart;//原公交线路起点更改
    g_BusMap.stations[newStart].station = start;
}

void AddEnd(char *bus){
    int distance;
    char end[30];
    int pBus = FindBus(bus);//公交线路的索引号
    int pStart = g_BusMap.buses[pBus].start;
    int pEnd = g_BusMap.buses[pBus].end;//原来公交线路的终点索引号
    printf("请输入新终点:");
    scanf("%s",end);
    printf("\n请输入新终点到原来终点的距离:");
    scanf("%d",&distance);
    int newEnd = GetStation(end);

    int i;
    STATION[STATION_NUM] = end;
    STATION_NUM++;//STATION数组变化
    printf("\n");
    for(i = 0;i < STATION_NUM;i++){
        printf("%s\n",STATION[i]);
    }

    int j;
    for(i = 0;i < ROUTE_NUM;i++)
        if(ROUTES[i][0]==pBus&&ROUTES[i+1][0]!=pBus){
            j = i+1;
            break;
        }

    for(i = ROUTE_NUM;i > j;i--){
        ROUTES[i][0] = ROUTES[i-1][0];
        ROUTES[i][1] = ROUTES[i-1][1];
        ROUTES[i][2] = ROUTES[i-1][2];
        ROUTES[i][3] = ROUTES[i-1][3];
    }
    ROUTES[j][0] = pBus;
    ROUTES[j][1] = pEnd;
    ROUTES[j][2] = newEnd;
    ROUTES[j][3] = distance;
    ROUTE_NUM++;  //ROUTES数组变化
    for(i = 0;i < ROUTE_NUM;i++){
        printf("%d %d %d %d\n",ROUTES[i][0],ROUTES[i][1],ROUTES[i][2],ROUTES[i][3]);
    }

    BUSES[pBus][0] = pBus;
    BUSES[pBus][1] = pStart;
    BUSES[pBus][2] = newEnd;
    //BUSES数组变化
    for(i = 0;i < BUS_NUM;i++){
        printf("%d %d %d\n",BUSES[i][0],BUSES[i][1],BUSES[i][2]);
    }

    CreateAndLoad();
}

void Menu(){
    printf("======公交管理系统======\n");
    printf("1.查询公交线路\n");
    printf("2.查询站点信息\n");
    printf("3.查询两个站点公交线路\n");
    printf("0.退出\n");
    printf("请输入操作编号<0-3>:\n");
}

int main(){
    ReadFile();
    CreateAndLoad();
    int select;
    do{
        Menu();
        scanf("%d",&select);
        switch(select)
        {
            case 0:
                printf("感谢使用！\n");
                break;
        	case 1:
            	Function1();
            	break;
        	case 2:
           		Function2();
            	break;
        	case 3:
            	Function3();
            	break;
        	default:
            	printf("输入有误，请重新输入\n");
            	break;
        }
    }while(select != 0);

    return 0;
}
