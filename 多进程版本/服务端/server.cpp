#include <cstring>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <sys/wait.h>
#include <sstream>
#include "easyMysql.h"
#include "game.h"

using namespace std;

#define MYPORT  8887
#define MYPORT2 8888
#define QUEUE   20
#define BUFFER_SIZE 1024
#define MAX_USER 3
#define SHARE_SIZE 1024
#define MYKEY 25


char buffer[BUFFER_SIZE];
int shmid;  
char *shmptr;  
int counter=0;//多线程计数器
int clientReadyNumber=0;


vector<poke> player[3];//poke数组，每个人的牌对应一个名字叫做player的poke结构体
int userCounter=0;//用户编号标记
int startflag=2;//初始化牌局
int endGame=2;//游戏结束标记

int landCounter=0;//叫地主序号
int landScore=0;//当前叫地主得分
int updateLandToSQL=1;
int landLordNumber;//地主序号
string landLordName="#";//地主姓名

int landHaveSaid=0;
int currentOrder=0;//当前轮到谁出牌

string clientSendCardList;//对客户端发来的牌进行判断





string userNum[3];

int checkRegister(int conn)
{
    char buffer[BUFFER_SIZE];
    char registerUserName[50];
    char registerUserPassword[50];
    send(conn,"startRegister",sizeof("startRegister"),0);
    recv(conn, buffer, sizeof(buffer), 0);
    strcpy(registerUserName,buffer);
    send(conn,"password?",sizeof("password?"),0);
    recv(conn, buffer, sizeof(buffer), 0);
    strcpy(registerUserPassword,buffer);
    sqlConnect();
    char query[100];
    sprintf(query,"insert into users (username,password,status) values ('%s','%s','offline')",registerUserName,registerUserPassword);
    if(sqlInsert(query))
    {
        send(conn,"registerOK",sizeof("registerOK"),0);
    }
    else
    {
        send(conn,"registerWrong",sizeof("registerWrong"),0);
    }
}

int checkLogin(int conn)
{
    char buffer[BUFFER_SIZE];
    char loginUserName[50];
    char loginUserPassword[50];
    send(conn,"startLogin",sizeof("startLogin"),0);
    recv(conn, buffer, sizeof(buffer), 0);
    strcpy(loginUserName,buffer);
    send(conn,"password?",sizeof("password?"),0);
    recv(conn, buffer, sizeof(buffer), 0);
    strcpy(loginUserPassword,buffer);
    sqlConnect();
    char query[100];
    sprintf(query,"update users set status = 'online' where binary username='%s' and password='%s' and status = 'offline'",loginUserName,loginUserPassword);
    if(sqlUpdate(query)==1)
    {
        send(conn,"loginOK",sizeof("loginOK"),0);
    }
    else
    {
        send(conn,"loginWrong",sizeof("loginWrong"),0);
    }
}

int checkReady(int conn)
{
    char buffer[BUFFER_SIZE];
    char readyUserName[50];
    char readyNum[50];
    char query[100];
    sqlConnect();
    send(conn,"yourUserName",sizeof("yourUserName"),0);
    recv(conn,buffer,sizeof(buffer),0);
    strcpy(readyUserName,buffer);
    sprintf(query,"update users set status = 'ready' where binary username='%s'",readyUserName);
    sqlUpdate(query);
}

int readyNumber(int conn)
{
    char readyNum[50];
    MYSQL_ROW sql1=sqlSelect("select count(*) from users where status = 'ready'");
    strcpy(readyNum,sql1[0]);
    send(conn,readyNum,sizeof(readyNum),0);
    if(atoi(sql1[0])==3)
    {
        cout<<"三人已准备，游戏开始"<<endl;
        return 1;
    }
    else
    {
        return 2;
    }
}


int userLogout(int conn)
{
    char buffer[BUFFER_SIZE];
    char logoutUserName[50];
    send(conn,"yourUserName",sizeof("yourUserName"),0);
    recv(conn,buffer,sizeof(buffer),0);
    strcpy(logoutUserName,buffer);
    sqlConnect();
    char query[100];
    sprintf(query,"update users set status = 'offline' where binary username='%s'",logoutUserName);
    sqlUpdate(query);
    send(conn,"logoutOK",sizeof("logoutOK"),0);
}

int onlineUser(int conn)
{
	string onlineUserList="";
	sqlConnect();
    MYSQL_ROW sql1=sqlSelect("select count(*) from users where (status = 'online' or status = 'ready')");
    mysql_query(mysql,"select username from users where (status = 'online' or status = 'ready')");
    result=mysql_store_result(mysql);

	ostringstream oss;

	for(int i=0;i<atoi(sql1[0]);i++)
	{
        row=mysql_fetch_row(result);
        oss<<row[0]<<"  ";
	}
    onlineUserList+=oss.str();

	strcpy(buffer,onlineUserList.c_str());
    send(conn,buffer,sizeof(buffer),0);
}



int checkUserName(int conn,char* ptr)
{
    char* clientUserName=ptr;
    userNum[userCounter]=ptr;
    cout<<"用户进入游戏："<<userNum[userCounter]<<endl;
    sprintf(buffer,"%d",userCounter);
    send(conn,buffer,sizeof(buffer),0);
    char query[100];
    sprintf(query,"insert into game set username='%s',landlord='no',landscore='0',gameid='1001',playernumber='%d'",clientUserName,userCounter);
    sqlInsert(query);    
    userCounter++;
}


int checkLand(int conn,char* ptr)//进行叫地主检查
{
    char askLandUserNum=*ptr;
    string sendLandUserNum="";
    ostringstream oss;
    oss<<landCounter<<"@"<<landScore<<"@"<<landLordName;
    sendLandUserNum+=oss.str();
    strcpy(buffer,sendLandUserNum.c_str());
    send(conn,buffer,sizeof(buffer),0);
}

int getLandScore(int conn,char* ptr1,char* ptr2)//来自用户的叫分
{
    int user=atoi(ptr1);
    int score=atoi(ptr2);
    cout<<user<<"号叫地主："<<score<<"分"<<endl;
    sqlConnect();
    char query[100];
    landScore=(landScore>=score?landScore:score);
    landCounter++;
    sprintf(query,"update game set landscore = '%d' where playernumber='%d'",score,user);
    sqlUpdate(query);
}


int serverSendDefaultCard(int conn,char* ptr)
{
	int userNumInt=atoi(ptr);
    sortCards(player[userNumInt]);
    string userCardList=getCardList(player[userNumInt]);
    strcpy(buffer,userCardList.c_str());
    send(conn,buffer,sizeof(buffer),0);
    cout<<userNumInt<<"号用户："<<userCardList<<endl;
}

int getLordAndCard(int conn,char* ptr)
{
    string identifyUser=ptr;
    string atLandLordName="areYouLandLord@";
    if(landCounter==3&&updateLandToSQL==1)
    {
        cout<<"查询最高叫分并给地主身份"<<endl;
        MYSQL_ROW sql1=sqlSelect("SELECT * FROM game where gameid = 1001 order by landscore DESC");
        landLordName=sql1[0];
        cout<<"地主名称"<<landLordName<<endl;
        landLordNumber=atoi(sql1[1]);
        cout<<"地主序号"<<landLordNumber<<endl;
        char query[100];
        sprintf(query,"update game set landlord = 'yes' where username = '%s'",landLordName.c_str());
        sqlUpdate(query);
        currentOrder=landLordNumber;
        updateLandToSQL=1;
    }
    atLandLordName+=landLordName;
    strcpy(buffer,atLandLordName.c_str());
    send(conn,buffer,sizeof(buffer),0);
    if(identifyUser==landLordName)
    {
        for(int i=52;i<55;i++)
        player[landLordNumber].push_back(p[a[i]]);//补上底牌
        sortCards(player[landLordNumber]);
        string userCardList=getCardList(player[landLordNumber]);
        strcpy(buffer,userCardList.c_str());
        cout<<"加入底牌后地主的牌:\n"<<buffer<<endl;
        send(conn,buffer,sizeof(buffer),0);
    }
    else
    {
        return 0;
    }
}


int serverJudgeCard(char * ptr)
{
    clientSendCardList=ptr;
    cout<<"用户出牌判断："<<clientSendCardList<<endl;
    return 1;
}

//接受客户端出牌信息
int serverRecvCard(int conn,char* ptr)
{
    int clientPlayerNum=atoi(ptr);
    if(clientPlayerNum==currentOrder)
    {
        send(conn,"yourTurnToSend",sizeof("yourTurnToSend"),0);
        recv(conn,buffer,sizeof(buffer),0);
        if(serverJudgeCard(buffer)==1)
        {
            send(conn,"sendCardRight",sizeof("sendCardRight"),0);
            currentOrder++;
            landHaveSaid=1;
            if(currentOrder==3)
            {
                currentOrder=0;
            }
        }
        else
        {
            send(conn,"sendCardWrong",sizeof("sendCardWrong"),0);
        }
    }
    else
    {
        send(conn,"NotYourTurn",sizeof("NotYourTurn"),0);
    }
}

//给客户端显示发牌信息
int serverSendCard(int conn,char* ptr)
{
    int clientPlayerNum=atoi(ptr);

    if(landHaveSaid==0)
    {
        send(conn,"waitForLand",sizeof("waitForLand"),0);
        return 1;
    }
    if(clientPlayerNum==currentOrder)
    {
        send(conn,"breakAndYourTurn@0",sizeof("breakAndYourTurn@0"),0);
        return 1;
    }
    else
    {
        string sendUserNumAndList="";
        int lastUserNumber=currentOrder-1;
        if(lastUserNumber==-1)
        {
            lastUserNumber=2;
        }
        ostringstream oss;
        oss<<lastUserNumber<<"@"<<clientSendCardList;
        sendUserNumAndList+=oss.str();
        strcpy(buffer,sendUserNumAndList.c_str());
        send(conn,buffer,sizeof(buffer),0);
        return 1;
    }
}


int server (int conn)
{
    recv(conn,buffer,sizeof(buffer),0);
    char *firstPtr;
    char *secondPtr;
    char *thirdPrt;
    firstPtr=strtok(buffer,"@");
    secondPtr=strtok(NULL, "@");

    if(startflag!=1)
    {
    	InitCards();//牌局为一个poke结构体(flw花色，num数字)的数组
	    ShuffleCards();//洗牌，一次行洗完三个用户的牌
	    dealCards(player);//发牌，一次性发完三个用户的牌
	    startflag=1;
	    cout<<"牌局已初始化完毕"<<endl;
    }


    if(strcmp(firstPtr,"startGame")==0)
    {
        checkUserName(conn,secondPtr);
        return 2;
    }
    if(strcmp(firstPtr,"userDefaultCard")==0)
    {
        serverSendDefaultCard(conn,secondPtr);
        return 2;
    }
    if(strcmp(firstPtr,"askLand")==0)
    {
        checkLand(conn,secondPtr);
        return 2;
    }
    if(strcmp(firstPtr,"getLand")==0)
    {
        thirdPrt=strtok(NULL, "@");
        getLandScore(conn,secondPtr,thirdPrt);
        return 2;
    }
    if(strcmp(firstPtr,"whoIsLandLord")==0)
    {
        getLordAndCard(conn,secondPtr);
        return 2;
    }
    if(strcmp(firstPtr,"clientSendCard")==0)//clientSendCard@玩家序号
    {
        serverRecvCard(conn,secondPtr);
        return 2;
    }
    if(strcmp(firstPtr,"clientRecvCard")==0)
    {
        serverSendCard(conn,secondPtr);
        return 2;
    }
    return 2;

}

























//线程1
void* serverThread1(void* ptr)
{
	int conn = *(int*)ptr;
    while(1)
    {
        recv(conn, buffer, sizeof(buffer), 0);
        if(strcmp(buffer,"clientRegister")==0)
        {
            checkRegister(conn);
        }
        if(strcmp(buffer,"clientLogin")==0)
        {
            checkLogin(conn);
        }
        if(strcmp(buffer,"clientReady")==0)
        {
            checkReady(conn);
        }
        if(strcmp(buffer,"readyNumber")==0)
        {
            int readyNumberFlag=2;
            readyNumberFlag=readyNumber(conn);
            if(readyNumberFlag!=1)
            {
                continue;
            }
            if(readyNumberFlag==1)
            {
                break;
            }
        }
        if(strcmp(buffer,"userLogout")==0)
        {
            userLogout(conn);
        }
        if(strcmp(buffer,"clientOnlineUser")==0)
        {
            onlineUser(conn);
        }
    }
    close(conn);
    cout<<"线程1关闭"<<endl;
    return 0;
}
//线程2
void* serverThread2(void* ptr)
{
    int conn = *(int*)ptr;
    while(1)
    {
        recv(conn, buffer, sizeof(buffer), 0);
        if(strcmp(buffer,"clientRegister")==0)
        {
            checkRegister(conn);
        }
        if(strcmp(buffer,"clientLogin")==0)
        {
            checkLogin(conn);
        }
        if(strcmp(buffer,"clientReady")==0)
        {
            checkReady(conn);
        }
        if(strcmp(buffer,"readyNumber")==0)
        {
            int readyNumberFlag=2;
            readyNumberFlag=readyNumber(conn);
            if(readyNumberFlag!=1)
            {
                continue;
            }
            if(readyNumberFlag==1)
            {
                break;
            }
        }
        if(strcmp(buffer,"userLogout")==0)
        {
            userLogout(conn);
        }
        if(strcmp(buffer,"clientOnlineUser")==0)
        {
            onlineUser(conn);
        }
    }
    close(conn);
    cout<<"线程2关闭"<<endl;
    return 0;
}
//线程3
void* serverThread3(void* ptr)
{
    int conn = *(int*)ptr;
    while(1)
    {
        recv(conn, buffer, sizeof(buffer), 0);
        if(strcmp(buffer,"clientRegister")==0)
        {
            checkRegister(conn);
        }
        if(strcmp(buffer,"clientLogin")==0)
        {
            checkLogin(conn);
        }
        if(strcmp(buffer,"clientReady")==0)
        {
            checkReady(conn);
        }
        if(strcmp(buffer,"readyNumber")==0)
        {
            int readyNumberFlag=2;
            readyNumberFlag=readyNumber(conn);
            if(readyNumberFlag!=1)
            {
                continue;
            }
            if(readyNumberFlag==1)
            {
                break;
            }
        }
        if(strcmp(buffer,"userLogout")==0)
        {
            userLogout(conn);
        }
        if(strcmp(buffer,"clientOnlineUser")==0)
        {
            onlineUser(conn);
        }
    }
    close(conn);
    cout<<"线程3关闭"<<endl;
    return 0;
}



int main(){
    cout<<"服务端已运行，等待客户端连接……"<<endl;
    sqlConnect();
    sqlUpdate("update users set status = 'offline'");
    sqlDelete("delete from game where gameid = 1001");
    int server_sockfd = socket(AF_INET,SOCK_STREAM, 0);//类型：Internet套接字, 格式：流数据 ,协议：自动

    ///定义sockaddr_in结构体
    struct sockaddr_in server_sockaddr;//定义结构体
    bzero(&(server_sockaddr.sin_zero),sizeof(server_sockaddr.sin_zero)); //套接字清零
    server_sockaddr.sin_family = AF_INET;//通讯类型：Internet
    server_sockaddr.sin_port = htons(MYPORT);//端口（转换到网络字节）
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);//地址（转换到网络字节）


    int client_sent_quit_message;

    ///bind，成功返回0，出错返回-1
    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)//socket()返回的文件描述符,指向sockaddr_in结构体的指针,长度
    {
        perror("bind");
        exit(1);
    }

    ///listen，成功返回0，出错返回-1
    if(listen(server_sockfd,QUEUE) == -1)//socket()返回的文件描述符,队列长度
    {
        perror("listen");
        exit(1);
    }

    ///客户端套接字
    char buffer[BUFFER_SIZE];//流数据的长度
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);

    //多线程部分
    int ret=0;
    pthread_t id1,id2,id3;
    while(counter<3)
    {
    	int conn = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);//socket()返回的文件描述符，指向客户端sockaddr_in的指针,长度
        if(conn<0)
        {
            perror("connect");
            exit(1);
        }
    	if(counter==0)
    	{
    		ret = pthread_create(&id1, NULL, serverThread1, &conn);
    		if (ret)
       		{
        	    printf("Create pthread error!\n");
        	    return 1;
       		}
    		cout<<"线程1创建"<<endl;
    		counter++;
    		continue;
		}
    	if(counter==1)
    	{
    		ret = pthread_create(&id2, NULL, serverThread2, &conn);
    		if (ret)
       		{
        	    printf("Create pthread error!\n");
        	    return 1;
       		}
    		cout<<"线程2创建"<<endl;
    		counter++;
    		continue;
    	}
    	if(counter==2)
    	{
    		ret = pthread_create(&id3, NULL, serverThread3, &conn);
   			if (ret)
       		{
        	    printf("Create pthread error!\n");
        	    return 1;
       		}
    		cout<<"线程3创建"<<endl;
    		counter++;
    		break;
    	}
    }

    //主进程会在玩家准备之后开始
    cout<<"主进程开始"<<endl;

	server_sockfd = socket(AF_INET,SOCK_STREAM, 0);//类型：Internet套接字, 格式：流数据 ,协议：自动

    bzero(&(server_sockaddr.sin_zero),sizeof(server_sockaddr.sin_zero)); //套接字清零
    server_sockaddr.sin_family = AF_INET;//通讯类型：Internet
    server_sockaddr.sin_port = htons(MYPORT2);//端口（转换到网络字节）
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);//地址（转换到网络字节）

    int opt=1;
    setsockopt(server_sockfd, SOL_SOCKET,SO_REUSEADDR, (const void *) &opt, sizeof(opt));

    ///bind，成功返回0，出错返回-1
    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)//socket()返回的文件描述符,指向sockaddr_in结构体的指针,长度
    {
        perror("bind");
        exit(1);
    }

    ///listen，成功返回0，出错返回-1
    if(listen(server_sockfd,QUEUE) == -1)//socket()返回的文件描述符,队列长度
    {
        perror("listen");
        exit(1);
    }

    while(1)
    {
        int conn = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);//socket()返回的文件描述符，指向客户端sockaddr_in的指针,长度
        if(conn<0)
        {
            perror("connect");
            exit(1);
        }
        client_sent_quit_message = server (conn);
        close (conn);
    }
	close(server_sockfd);
}