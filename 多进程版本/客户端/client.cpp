#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <netdb.h>
#include <iostream>
#include <string>
#include <sstream>
#include "game.h"

using namespace std;

#define MYPORT  8887
#define MYPORT2 8888
#define BUFFER_SIZE 1024

char sendbuf[BUFFER_SIZE];
char recvbuf[BUFFER_SIZE];
char currentUserName[50];//当前用户名
char playerNum;//玩家编号
int registerFlag=2;//注册标记
int loginFlag=2;//登录标记
int readyFlag=2;//准备标记


//游戏进行中的标记
int gameEndFlag=2;//游戏结束标记
int startGameFlag=2;//游戏开始标记
int game_quit_message=2;//游戏退出标记
int getUserDefaultCardFlag=2;//用户初始手牌标记
int askLandFlag=2;//用户检查是否轮到自己叫地主
int getLandFlag=2;//用户叫地主标记
int currentUserLandScore=0;//本用户叫地主叫分
int currentLandScore=0;//叫地主最高叫分
bool amILandlord=false;//本用户是否为地主
string currenLandLord;//谁是地主
int clientAskLandFlag=-1;
int getLordAndCardFlag=2;
string sendCard;

string mycard;


int clientSendCardFlag=2;//出牌标记
int clientRecvCardFlag=2;//发牌标记

int cardNumber;//手牌个数


void BeginPrompt()
{
    puts("\n输入指令然后回车来选择功能：\n");
    puts("register:注册帐号");
    puts("login:登录帐号");
    puts("help:帮助说明");
    puts("exit:退出程序\n");
}
void ReadyPrompt()
{
	puts("\n现在是准备阶段，你想要干什么?:\n");
	puts("ready:准备游戏");
	puts("user:查看在线用户");
    puts("logout:用户注销");
}
void StartPrompt()
{
    puts("\n现在开始斗地主游戏。显示的牌第一行为花色和牌面，第二行为代号\n牌面说明：H红桃，S黑桃，C梅花，D方片，JKR大王，jkr小王\n");
    puts("具体指令说明:");
    puts("go:通过go 牌号(用空格分隔)来出牌，如想要打出123连子，请输入go 1 2 3然后回车");
    puts("land:叫地主,格式为land 分数(中间空格隔开)，如叫1分，输入land 1，不叫牌为land 0");
    puts("pass:跳过属于你的出牌阶段一次");
    puts("user:查看在线用户");
    puts("exit:强行离开游戏，记录不保留\n");
}
void help()
{
	puts("\n指令说明：\n1、登录：\nregister:注册帐号\nlogin:登录帐号\nhelp:帮助说明\nexit:退出程序\n\n2、准备：\nready:准备游戏\nuser:查看在线用户\nlogout:用户注销\n");
}

int loginUser(int sock_cli)
{
	char loginUserName[50];
	char loginUserPassword[50];
	cout<<"用户名"<<endl;
	cin>>loginUserName;
	cout<<"密码"<<endl;
	cin>>loginUserPassword;
	send(sock_cli, "clientLogin", sizeof("clientLogin"), 0);
	recv(sock_cli,recvbuf,sizeof(recvbuf),0);
	if(strcmp(recvbuf,"startLogin"))
	{
		return -1;
	}
	send(sock_cli, loginUserName, sizeof(loginUserName), 0);
	recv(sock_cli,recvbuf,sizeof(recvbuf),0);
	send(sock_cli, loginUserPassword, sizeof(loginUserPassword), 0);
	recv(sock_cli,recvbuf,sizeof(recvbuf),0);
	if(strcmp(recvbuf,"loginOK"))
	{
		cout<<"登录失败，请检查你的用户名密码!"<<endl;
		return 2;
	}
	else
	{
		cout<<"\n登录成功!"<<endl;
		strcpy(currentUserName,loginUserName);
		cout<<"欢迎："<<currentUserName<<endl;
		return 1;
	}
}

int regiserUser(int sock_cli)
{
    char regiserUserName[50];
    char regiserUserPassword[50];
    char regiserUserPassword2[50];
    cout<<"请输入注册的用户名"<<endl;
    cin>>regiserUserName;
    cout<<"请输入密码"<<endl;
    cin>>regiserUserPassword;
    cout<<"再次输入密码来确认"<<endl;
    cin>>regiserUserPassword2;
    if(strcmp(regiserUserPassword,regiserUserPassword2))
    {
        cout<<"两次输入密码不相同，请重试"<<endl;
        return 2;
    }
    send(sock_cli, "clientRegister", sizeof("clientRegister"), 0);
    recv(sock_cli,recvbuf,sizeof(recvbuf),0);
    {
    	if(strcmp(recvbuf,"startRegister"))
    	{
    		return -1;
    	}
    }
    send(sock_cli, regiserUserName, sizeof(regiserUserName), 0);
    recv(sock_cli,recvbuf,sizeof(recvbuf),0);
    send(sock_cli, regiserUserPassword, sizeof(regiserUserPassword), 0);
    recv(sock_cli,recvbuf,sizeof(recvbuf),0);
    if(strcmp(recvbuf,"registerOK"))
    {
        cout<<"用户名已被注册!"<<endl;
        return 2;
    }
    else
    {
        cout<<"注册成功!"<<endl;
        return 1;
    }
}


int ready(int sock_cli)
{
	send(sock_cli, "clientReady", sizeof("clientReady"), 0);
	recv(sock_cli,recvbuf,sizeof(recvbuf),0);
	if(strcmp(recvbuf,"yourUserName"))
	{
		return -1;
	}
	send(sock_cli, currentUserName, sizeof(currentUserName), 0);
}

int checkReady(int sock_cli)
{
    send(sock_cli, "readyNumber", sizeof("readyNumber"), 0);
    recv(sock_cli,recvbuf,sizeof(recvbuf),0);
    if(strcmp(recvbuf,"3")==0)
    {
        cout<<"三人准备完成，开始游戏"<<endl;
        readyFlag=1;
    }
}


int checkUser(int sock_cli)
{
    cout<<"正在查询在线用户……"<<endl;
	send(sock_cli, "clientOnlineUser", sizeof("clientOnlineUser"), 0);
	recv(sock_cli,recvbuf,sizeof(recvbuf),0);
    cout<<"在线用户:"<<recvbuf<<endl;
}


void logout(int sock_cli)
{
    send(sock_cli, "userLogout", sizeof("userLogout"), 0);
    recv(sock_cli,recvbuf,sizeof(recvbuf),0);
    send(sock_cli,currentUserName,sizeof(currentUserName),0);
    recv(sock_cli,recvbuf,sizeof(recvbuf),0);
}





int startGame(int sock_cli)
{
	string startGameSend="startGame@";
	startGameSend+=currentUserName;
	strcpy(sendbuf,startGameSend.c_str());
	memset(recvbuf,0,sizeof(recvbuf));
    send(sock_cli,sendbuf,sizeof(sendbuf),0);
    recv(sock_cli,recvbuf,sizeof(recvbuf),0);
    playerNum=*recvbuf;
    cout<<"你的顺序编号"<<playerNum<<endl;
    startGameFlag=1;
}

int clientAskLand()
{
	if(currentLandScore==3)
	{
		cout<<"地主已叫到3分，不能叫分"<<endl;
		currentUserLandScore=0;
		clientAskLandFlag=0;
		return 0;
	}
    int userLandScore;
    cout<<"现在轮到你叫地主啦!"<<"地主分已叫到"<<currentLandScore<<"分,你可以出更大的或者0分（表示放弃叫地主）"<<endl;
    cin>>userLandScore;
    cin.ignore(1024, '\n');
    if((userLandScore>currentLandScore)&&(userLandScore<4)&&(userLandScore>0))
    {
        cout<<"成功叫地主叫"<<userLandScore<<"分"<<endl;
        clientAskLandFlag=userLandScore;
        return 0;
    }
    else
    {
        if(userLandScore==0)
        {
            cout<<"不叫地主"<<endl;
            clientAskLandFlag=0;
            return 0;
        }
        else
        {
            cout<<"叫分出错，请重叫"<<endl;
            return -1;
        }
    }
}

int askLand(int sock_cli)
{
    string askLandSend="askLand@";
    askLandSend+=playerNum;
    strcpy(sendbuf,askLandSend.c_str());
    send(sock_cli,sendbuf,sizeof(sendbuf),0);
    recv(sock_cli,recvbuf,sizeof(recvbuf),0);
    char* firstPtr;
    char* secondPtr;
    char* thirdPtr;
    firstPtr=strtok(recvbuf,"@");
    secondPtr=strtok(NULL, "@");
    thirdPtr=strtok(NULL, "@");
    currenLandLord=thirdPtr;
    currentLandScore=atoi(secondPtr);
    if(playerNum!=*firstPtr)
    {
        return 2;
    }
    if(currenLandLord!="#")
    {
    	cout<<"地主已被叫走,地主为"<<currenLandLord<<endl;
    	currentUserLandScore=0;
    	askLandFlag=1;
    	return 1;
    }
    else
    {
        close(sock_cli);
        while(clientAskLandFlag==-1)
        {
            clientAskLand();
        }
        currentUserLandScore=clientAskLandFlag;
        askLandFlag=1;
        return 1;
    }
    return 2;
}


int getLand(int sock_cli)
{
    string getLandSend="";
    ostringstream oss;
    oss<<"getLand@"<<playerNum<<"@"<<currentUserLandScore;
    getLandSend+=oss.str();
    strcpy(sendbuf,getLandSend.c_str());
    send(sock_cli,sendbuf,sizeof(sendbuf),0);
    getLandFlag=1;
}

int getLordAndCard(int sock_cli)
{
    string whoIsLand="whoIsLandLord@";
    whoIsLand+=currentUserName;
    char* firstPtr;
    char* secondPtr;
    strcpy(sendbuf,whoIsLand.c_str());
    send(sock_cli,sendbuf,sizeof(sendbuf),0);
    recv(sock_cli,recvbuf,sizeof(recvbuf),0);
    firstPtr=strtok(recvbuf,"@");
    secondPtr=strtok(NULL,"@");
    if(strcmp(secondPtr,"#")==0)
    {
    	return 2;
    }
    if(strcmp(secondPtr,currentUserName)==0)
    {
    	amILandlord=true;
    	recv(sock_cli,recvbuf,sizeof(recvbuf),0);
    	mycard=recvbuf;
    	cout<<"你是地主！下面补牌并开始出牌"<<endl;
    	cout<<mycard<<endl;
        cardNumber=count(mycard.begin(),mycard.end(),'.');
        for(int i=0;i<cardNumber;i++)
        {
            cout<<i<<"     ";
        }
        cout<<endl;
    	cout<<"已加入底牌，由地主你先出牌"<<endl;
    	getLordAndCardFlag=1;
    	return 1;
    }
    else
    {
    	cout<<"查询地主中……"<<endl;
        cout<<"地主是："<<secondPtr<<endl;
        cout<<"等待地主出牌"<<endl;
        getLordAndCardFlag=1;
        return 1;
    }
}


int getUserDefaultCard(int sock_cli)
{
    string getUserDefaultCardSend="userDefaultCard@";
    getUserDefaultCardSend+=playerNum;
    strcpy(sendbuf,getUserDefaultCardSend.c_str());
    send(sock_cli,sendbuf,sizeof(sendbuf),0);
    recv(sock_cli,recvbuf,sizeof(recvbuf),0);
    mycard=recvbuf;
    cardNumber=count(mycard.begin(),mycard.end(),'.');
    cout<<mycard<<endl;
    for(int i =0;i<cardNumber;i++)
    {
        cout<<i<<"     ";
    }
    cout<<endl;
    getUserDefaultCardFlag=1;
}


int clientJudgeCard(string sendCard)
{
    int cardNumber=count(sendCard.begin(),sendCard.end(),' ');
    cardNumber++;
    cout<<"你选中了"<<cardNumber<<"张牌"<<endl;
	return 1;//以go开头判断满足空格格式
}


int clientSendCard(int sock_cli)
{
    string getPlayerNum="clientSendCard@";
    memset(sendbuf,0,sizeof(sendbuf));
    memset(recvbuf,0,sizeof(recvbuf));
    getPlayerNum+=playerNum;
    strcpy(sendbuf,getPlayerNum.c_str());
    send(sock_cli,sendbuf,sizeof(sendbuf),0);
    recv(sock_cli,recvbuf,sizeof(recvbuf),0);
    if(strcmp(recvbuf,"yourTurnToSend")==0)
    {
        cout<<"该你出牌了，请输入牌号，用空格隔开"<<endl;
        while(getline(cin,sendCard))
        {
        	if (clientJudgeCard(sendCard)==1)
        	{
        		strcpy(sendbuf,sendCard.c_str());
        		send(sock_cli,sendbuf,sizeof(sendbuf),0);
        		recv(sock_cli,recvbuf,sizeof(recvbuf),0);
        		if(strcmp(recvbuf,"sendCardRight")==0)
        		{
        			cout<<"出牌成功，等待下一人"<<endl;
			        clientSendCardFlag=1;
			        clientRecvCardFlag=2;
			        break;
        		}
        		else
        		{
        			cout<<"出牌不符合游戏规则，请重新出牌"<<endl;
        			continue;
        		}
        	}
        	else
        	{
        		cout<<"出牌指令错误,请输入牌号用空格隔开"<<endl;
        		continue;
        	}
        }
    }
    else
    {
    	clientSendCardFlag=1;
    	clientRecvCardFlag=2;
    	return 0;
    }

}


int clientRecvCard(int sock_cli)
{
    string getPlayerNum="clientRecvCard@";
    getPlayerNum+=playerNum;
    strcpy(sendbuf,getPlayerNum.c_str());
    send(sock_cli,sendbuf,sizeof(sendbuf),0);
    recv(sock_cli,recvbuf,sizeof(recvbuf),0);
    char* firstPtr;
    char* secondPtr;
    firstPtr=strtok(recvbuf,"@");
    secondPtr=strtok(NULL,"@");
    if(strcmp(firstPtr,"waitForLand")==0)
    {
    	return 0;
    }
    if(strcmp(firstPtr,"breakAndYourTurn")==0)
    {
    	clientRecvCardFlag=1;
    	clientSendCardFlag=2;
    	return 0;
    }
    else
    {
    	cout<<firstPtr<<"号用户已出牌："<<secondPtr<<endl;
    	clientRecvCardFlag=1;
    	clientSendCardFlag=2;
    	return 0;
    }
}




int landGame(int sock_cli)
{
	if(startGameFlag!=1)
	{
		cout<<"游戏开始啦，准备发牌"<<endl;
		startGame(sock_cli);
		return 2;
	}

	if(getUserDefaultCardFlag!=1&&startGameFlag==1)
	{
        cout<<"首先收到17张牌，准备叫地主"<<endl;
		getUserDefaultCard(sock_cli);
        cout<<"等待叫地主中"<<endl;
		return 2;
	}
    if(askLandFlag!=1&&getUserDefaultCardFlag==1)
    {
        askLand(sock_cli);
        return 2;
    }
	if(getLandFlag!=1&&askLandFlag==1)
	{
		getLand(sock_cli);
		return 2;
	}
    if(getLordAndCardFlag!=1&&getLandFlag==1)
    {
        getLordAndCard(sock_cli);
        return 2;
    }
    if(clientSendCardFlag!=1&&getLordAndCardFlag==1)
    {
        clientSendCard(sock_cli);
        return 2;
    }
    if(clientRecvCardFlag!=1&&getLordAndCardFlag==1)
    {
        clientRecvCard(sock_cli);
        return 2;
    }
	return 2;
}

int main()
{
    //定义sockfd
    int sock_cli = socket(AF_INET,SOCK_STREAM, 0);

    //定义sockaddr_in
    struct sockaddr_in servaddr;
    bzero(&(servaddr.sin_zero),sizeof(servaddr.sin_zero)); //套接字清零
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT);  //服务器端口
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //服务器ip

    //连接服务器，成功返回0，错误返回-1
    if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        exit(1);
    }
	//至此就成功连接服务器了


    puts("欢迎来到欢乐斗地主游戏(字符版)！^^\n");
    string choice;
    while(1)
    {
    	BeginPrompt();
    	cin>>choice;
		if(choice=="register")
		{
			registerFlag=regiserUser(sock_cli);
            if(registerFlag==1)
            {
            	BeginPrompt();
                continue;
            }
            if(registerFlag==2)
            {
                continue;
            }
            if(registerFlag==-1)
            {
                exit(1);
            }
		}
		if(choice=="login")
		{
			loginFlag=loginUser(sock_cli);
            if(loginFlag==1)
            {
                break;
            }
            if(loginFlag==2)
            {
                continue;
            }
            if(loginFlag==-1)
            {
                exit(1);
            }
		}
		if(choice=="help")
		{
			help();
			continue;
		}
		if(choice=="exit")
		{
			close(sock_cli);
			exit(0);
		}
		else
		{
			cout<<"指令错误，请重新输入"<<endl;
			BeginPrompt();
			continue;
		}
    }
    puts("你已经登录啦，现在准备进行游戏!-0-\n");
    while(1)
    {
    	ReadyPrompt();
    	string choice2;
    	cin>>choice2;
		if(choice2=="ready")
		{
            cout<<"你已准备游戏，正在等待其他人准备"<<endl;
            ready(sock_cli);
            while(readyFlag!=1)
            {
                checkReady(sock_cli);
                usleep(100000);
            }
            break;
		}
		if(choice2=="user")
		{
			checkUser(sock_cli);
			continue;
		}
        if(choice2=="logout")
        {
            logout(sock_cli);
            close(sock_cli);
            exit(0);
        }
		else
		{
			cout<<"指令错误，请重新输入"<<endl;
			continue;
		}
    }
    close(sock_cli);
    StartPrompt();
    sock_cli = socket(AF_INET,SOCK_STREAM, 0);
    bzero(&(servaddr.sin_zero),sizeof(servaddr.sin_zero));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT2);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    while(game_quit_message!=99)
    {
    	sock_cli = socket(AF_INET,SOCK_STREAM, 0);
	    connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr));
	    usleep(100000);
	    game_quit_message=landGame(sock_cli);
	    close(sock_cli);
	}
}