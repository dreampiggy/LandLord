#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <cctype>
#include <sys/shm.h>
#include <iostream>
using namespace std;

#define MYPORT  8887
#define MYPORT2 8888
#define BUFFER_SIZE 150
#define ERR_EXIT(m) \
    do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while (0)


int sock_cli;
struct sockaddr_in servaddr;
char username[BUFFER_SIZE];
char landlord[BUFFER_SIZE];


void BeginPrompt(){
    puts("您的选择是：");
    puts("1.我没注册");
    puts("2.我想登录");
    puts("3.我要匿名登录");
    puts("4.我要退出");
}

/*
    when BWR:
    return 0, it means BWR will be restart 
    return -1 means BWR broke down with error
    return 1 indicates that everything is ok
*/
int BeginWithRegis(int sock_cli){
    char sendbuf[50], recvbuf[50], sendbuf2[50];
    puts("报上名来:");
    
    fscanf(stdin,"%s",sendbuf);
    while(strlen(sendbuf)==0){
        puts("输入不能为空");
        fscanf(stdin,"%s",sendbuf);
    }

    puts(sendbuf);

    send(sock_cli, sendbuf, sizeof(sendbuf), 0);
    recv(sock_cli, recvbuf, sizeof(recvbuf), 0);

    if(strcmp(recvbuf,"usernameexist")==0){
        puts("帐号已被注册");
        return 2;
    }
    else if(strcmp(recvbuf,"usernamenoexist")==0){
        puts("恭喜你，帐号未被注册");
        while(1){
            puts("请输入密码:");
                    
            fscanf(stdin,"%s",sendbuf);
            while(strlen(sendbuf)==0){
                puts("输入不能为空");
                fscanf(stdin,"%s",sendbuf);
            }

            puts("请再次输入密码");
            fscanf(stdin,"%s",sendbuf2);
            while(strlen(sendbuf2)==0){
                puts("输入不能为空");
                fscanf(stdin,"%s",sendbuf2);
            }

            if(strcmp(sendbuf2, sendbuf)==0){
                send(sock_cli, sendbuf, sizeof(sendbuf), 0);
                recv(sock_cli, recvbuf, sizeof(recvbuf), 0);
                if(strcmp(recvbuf, "successregister")==0)
                    return 1;
                else
                    return -1;
            }
            else{
                puts("两次密码不同，请重新输入");
            }
        }
    }
    else{
        puts("Fuck! System Error!!!");
        return -1;
    }
}


int BeginWithLogin(int sock_cli){
    char sendbuf[50], recvbuf[50];
    puts("请输入用户名");
    
    fscanf(stdin,"%s",sendbuf);
    while(strlen(sendbuf)==0){
        puts("输入不能为空");
        fscanf(stdin,"%s",sendbuf);
    }
    strcpy(username, sendbuf);

    send(sock_cli, sendbuf, sizeof(sendbuf), 0);
    recv(sock_cli, recvbuf, sizeof(recvbuf), 0);
    if(strcmp(recvbuf,"inputpassword")==0){

        puts("请输入密码");
        fscanf(stdin,"%s",sendbuf);
        while(strlen(sendbuf)==0){
            puts("输入不能为空");
            fscanf(stdin,"%s",sendbuf);
        }

        send(sock_cli, sendbuf, sizeof(sendbuf), 0);
        recv(sock_cli, recvbuf, sizeof(recvbuf), 0);
        if(strcmp(recvbuf,"successlogin")==0){
            puts("可以进去了");
            return 1;
        }
        else if(strcmp(recvbuf, "passwordincorrect")==0){
            puts("密码错误！");
            return 2;
        }
        else{
            puts("login error");
            return -1;
        }
    }
    else if(strcmp(recvbuf,"usernameonline")==0){
        puts("该用户已经登录");
        return 2;
    }    
    else if(strcmp(recvbuf,"usernameexist")==0){
        puts("用户名不存在");
        return 2;
    }
    else{
        return -1;
    }
}

void UserBegin(int sock_cli){ 
    char sendbuf[BUFFER_SIZE];
    char recvbuf[BUFFER_SIZE];  
    //至此就成功连接服务器了
    int flag;
    while(1){
        BeginPrompt();
        fgets(sendbuf, sizeof(sendbuf), stdin);
        send(sock_cli, sendbuf, sizeof(sendbuf),0); ///发送
        recv(sock_cli, recvbuf, sizeof(recvbuf),0); ///接收

        if(strcmp(recvbuf,"register")==0){
            int flag=-1;
            while(1){
                flag=BeginWithRegis(sock_cli);
                if(flag==2) continue;
                else    break;
            }
            if(flag==0)
                continue;
            else if(flag==-1)
                exit(1);
            else if(flag==1)
                break;
        }
        else if(strcmp(recvbuf,"login")==0){
            int flag=-1;
            while(1){
                flag=BeginWithLogin(sock_cli);
                if(flag==2)
                    continue;
                else
                    break;
            }
            if(flag==0)
                continue;
            else if(flag==1)
                break;
            else
                exit(1);
        }
        else    exit(0);
    }
}


void ReadyPrompt()
{
    puts("\n现在是准备阶段，你想要干什么?:\n");
    puts("ready:准备游戏");
    puts("logout:用户注销");
}

void logout(int sock_cli)
{
    char recvbuf[BUFFER_SIZE];
    send(sock_cli, "userlogout", sizeof("userlogout"), 0);
    recv(sock_cli, recvbuf, sizeof(recvbuf),0);
    send(sock_cli, username, sizeof(username),0);
    recv(sock_cli, recvbuf, sizeof(recvbuf),0);
    if(strcmp(recvbuf, "successlogout")!=0)
        ERR_EXIT("logout error"); 
}

int ready(int sock_cli){
    char recvbuf[BUFFER_SIZE];
    send(sock_cli, "clientready", sizeof("clientready"), 0);
    recv(sock_cli, recvbuf, sizeof(recvbuf),0);
    if(strcmp(recvbuf, "yourusername")==0 && strlen(username)>0){ //usrname should not be empty
        send(sock_cli, username, sizeof(username), 0);
        recv(sock_cli, recvbuf, sizeof(recvbuf), 0);
        if(strcmp(recvbuf, "successready")==0)
            return 1;
        else    ERR_EXIT("get ready error");
    }
    else    ERR_EXIT("username error");
}

void UserGetReady(int sock_cli){ 
    puts("你已经登录啦，现在准备进行游戏!-0-\n");
    char choice2[30];
    while(1)
    {
        ReadyPrompt();
        fscanf(stdin, "%s", choice2);
        if(strcmp(choice2, "ready")==0){
            int readyFlag=ready(sock_cli);
            if(readyFlag==1){
                break;
            }
            else ERR_EXIT("ready error");
        }
        if(strcmp(choice2, "logout")==0){
            logout(sock_cli);
            puts("you have logout");
            close(sock_cli);
            exit(0);
        }
        else{
            puts("指令错误，请重新输入");
            continue;
        }
    }
}

void WaitForOthers(int sock_cli){
    char recvbuf[BUFFER_SIZE];
    puts("wait for others....");
    recv(sock_cli, recvbuf, sizeof(recvbuf),0);
    puts("allready");
    memset(recvbuf, 0, sizeof(recvbuf));
    // if(strcmp(recvbuf, "allready")!=0)
    //     ERR_EXIT("ready error");
    // 
}

void UserCallLandlord(int sock_cli){
    char recvbuf[BUFFER_SIZE], sendbuf[BUFFER_SIZE];
    puts("call land lord now");
    recv(sock_cli, recvbuf, sizeof(recvbuf), 0);
    if(strcmp(recvbuf, "yourusername")!=0)
        ERR_EXIT("call username error");
    send(sock_cli, username, sizeof(username), 0);
    puts("wait for others to CallLandlord");
    recv(sock_cli, recvbuf, sizeof(recvbuf), 0);

    if(strcmp(recvbuf, "calllandlord")==0){  
        while(1){      
            puts("call landlord or not ? (y/n)");
            fscanf(stdin, "%s", sendbuf);       
            if(strcmp(sendbuf, "y")==0 || strcmp(sendbuf, "n")==0){
                send(sock_cli, sendbuf, sizeof(sendbuf), 0);
                break;
            }
            else{
                puts(sendbuf);
                puts("input again");
                continue;
            }
        }
        puts("wait for others to CallLandlord");
        recv(sock_cli, landlord, sizeof(landlord), 0);
        printf("the landlord is %s\n", landlord);
    }
}

void ConnectServer(const int port){
    puts("please wait 3 senconds");
    for(int i=3;i>0;i--){
        printf("%ds\n",i);
        sleep(1);
    }

    sock_cli = socket(AF_INET,SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);  ///服务器端口
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  ///服务器ip
    int on = 1;
    if (setsockopt(sock_cli, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        ERR_EXIT("setsockopt error");

    ///连接服务器，成功返回0，错误返回-1
    if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("connect error");
}

void PlayGame(int sock_cli){
    char recvbuf[BUFFER_SIZE], sendbuf[BUFFER_SIZE];
    puts("game start");
    
    recv(sock_cli, recvbuf, sizeof(recvbuf), 0);
    if(strcmp(recvbuf, "yourusername")!=0)
        ERR_EXIT("call username error");
    send(sock_cli, username, sizeof(username), 0);
    recv(sock_cli, recvbuf, sizeof(recvbuf), 0);
    puts(recvbuf);
    puts("wait for others...");

    fgets(sendbuf, sizeof(sendbuf), stdin);
    send(sendbuf, )

}

int main()
{
    ConnectServer(MYPORT); 
    UserBegin(sock_cli);
    UserGetReady(sock_cli);
    WaitForOthers(sock_cli);
/*-----------------------------*/
    ConnectServer(MYPORT2);
    UserCallLandlord(sock_cli);
/*-----------------------------*/
    ConnectServer(MYPORT3);
    PlayGame(sock_cli);
    close(sock_cli);
    return 0;
}