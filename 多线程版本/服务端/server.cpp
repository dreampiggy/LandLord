#include <cstring>
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <ctime>
#include <cctype>
#include "usermysql.h"
#include "poke.h"
using namespace std;

#define MYPORT  8887
#define MYPORT2 8888
#define MYPORT3 8889
#define QUEUE   20
#define BUFFER_SIZE 150
#define ERR_EXIT(m) \
    do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while (0)

typedef map< string,vector<poke> > PLAYERLIST;

int server_sockfd;
struct sockaddr_in server_sockaddr;
int ready_num;
queue<string> UserSequence;
PLAYERLIST player;
string first, landlord;
string landlordcards[3];
int times;
bool yes=0;

const int gameID = 1001;

/*
    RegisterUser return 3 kinds of values
    1: success
    2: username duplicated
    -1: error
*/
int RegisterUser(int conn){
    char username[50], pwd[20];
    recv(conn, username, sizeof(username), 0);
    int sta = UsernameExist(username);
    if(sta==1){
        send(conn, "usernameexist", sizeof("usernameexist"), 0);
        return 2;
    }          
    else if(sta==0){
        send(conn, "usernamenoexist", sizeof("usernamenoexist"), 0);
        recv(conn, pwd, sizeof(pwd), 0);
        if(UserInsert(username, pwd)==1){
            send(conn, "successregister", sizeof("successregister"), 0);
            return 1;
        }
        else{
            send(conn, "failregister", sizeof("failregister"), 0);
            return -1;
        }
    }  
    else{
        return -1;
    }
}

int login(int conn){
    char username[50], password[20];
    recv(conn, username, sizeof(username), 0);
    if(strcmp(username, "--exit")==0)
        return 0;
    int sta0 = UsernameExist(username);
    if(sta0==1){
        int sta = UserOnline(username);
        if(sta==0){
            send(conn, "inputpassword", sizeof("inputpassword"), 0);
            recv(conn, password, sizeof(password), 0);
            int sta2=PasswordCorrect(username, password);
            if(sta2==1){
                int sta3=StatusTurnOn(username);
                if(sta3==1){
                    send(conn, "successlogin", sizeof("successlogin"), 0);
                    return 1;
                }
                else{
                    send(conn, "faillogin", sizeof("faillogin"), 0);
                    return -1;
                }
            }
            else if(sta==0){
                send(conn, "passwordincorrect", sizeof("passwordincorrect"), 0);
                return 2;
            }
            else{
                send(conn, "fail", sizeof("fail"), 0);
                return -1;
            }
        }
        else if(sta==1){
            send(conn, "usernameonline", sizeof("usernameonline"), 0);
            return 2;
        }
        else{
            send(conn, "fail", sizeof("fail"), 0);
            return -1;
        }
    }
    else if(sta0==0){
        send(conn, "usernameexist", sizeof("usernameexist"), 0);
        return 2;
    }
    else{
        send(conn, "fail", sizeof("fail"), 0);
        return -1;
    }
}

void BeforeEnter(int conn){
    char buffer[BUFFER_SIZE];
    //客户端发回去的信息不带回车
    //一开始询问用户要注册登录，已有账户登录，匿名登录还是退出
    int flag=-1;    
    while(1){
        recv(conn, buffer, sizeof(buffer), 0);
        if(strcmp(buffer,"1\n")==0){
            flag=-1;
            send(conn, "register", sizeof("register"), 0);
            while(1){
                flag = RegisterUser(conn);
                if(flag==2)
                    continue;
                else
                    break;
            }
            if(flag==0)
                continue;
            else if(flag==-1)
                exit(1);
            else
                break;
        }
        else if(strcmp(buffer,"2\n")==0){
            send(conn, "login", sizeof("login"), 0);
            int flag=-1;
            while(1){
                flag=login(conn);
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
        else{
            send(conn, "quit", sizeof("quit"), 0);
            break;
        }
    }
}

void DuringReady(int conn){
    char buffer[BUFFER_SIZE];
    while(1){
        recv(conn, buffer, sizeof(buffer), 0);
        while(!isalnum(buffer[0]))
            recv(conn, buffer, sizeof(buffer), 0);
        if(strcmp(buffer, "clientready") == 0){
            send(conn, "yourusername", sizeof("yourusername"), 0);
            recv(conn, buffer, sizeof(buffer), 0);
            if(StatusTurnReady(buffer)==1){
                send(conn, "successready", sizeof("successready"), 0);
                break;
            }
            else{
                send(conn, "fail", sizeof("fail"), 0);
                ERR_EXIT("status turn ready error");
            }
        }
        else if(strcmp(buffer, "userlogout")==0){
            send(conn, "yourusername", sizeof("yourusername"), 0);
            recv(conn, buffer, sizeof(buffer), 0);
            if(StatusTurnOff(buffer)==1){
                send(conn, "successlogout", sizeof("successlogout"), 0);
                continue;
            }
            else{
                send(conn, "fail", sizeof("fail"), 0);
                ERR_EXIT("status turn off error");
            }
        }
        else    ERR_EXIT("command error");
    }
}

void *BigProcess(void *arg){
    int conn;
    ///客户端套接字
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);
    ///成功返回非负描述字，出错返回-1
    conn = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);
    if(conn<0)    ERR_EXIT("connect error");
    BeforeEnter(conn);
    DuringReady(conn);
    ready_num++;
    while(ready_num!=3){
        sleep(2);
    }
    send(conn, "allready", sizeof("allready"), 0);
    close(conn);
    pthread_exit(NULL);
    return NULL;
}

void *CallLandlord(void *arg){
    int conn;
    ///客户端套接字
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);
    ///成功返回非负描述字，出错返回-1
    conn = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);
    if(conn<0)    ERR_EXIT("connect error");

    char buffer[BUFFER_SIZE];
    send(conn, "yourusername", sizeof("yourusername"), 0);
    recv(conn, buffer, sizeof(buffer), 0);
    string usernow = buffer;

    while(UserSequence.front() != usernow)    sleep(3);
    times=1;

    send(conn, "calllandlord", sizeof("calllandlord"), 0);
    recv(conn, buffer, sizeof(buffer), 0);
    if(strcmp(buffer, "y")==0){
        times*=2;
        if(usernow == first){
            yes=1;
            landlord = first;
        }
        else if(!yes)
            landlord = usernow;
    }
    else if(strcmp(buffer, "n")==0);//do nothing
    else ERR_EXIT("calllandlord error");
    UserSequence.pop();

    ready_num++;
    while(ready_num!=3)    sleep(2);

    strcpy(buffer, landlord.c_str()); //send landlord name to user
    send(conn, buffer, sizeof(buffer), 0);
    close(conn);
    pthread_exit(NULL);
    return NULL;
}

void SocketConn(int port){
    server_sockfd = socket(AF_INET,SOCK_STREAM, 0);
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(port);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1;
    if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        ERR_EXIT("setsockopt error");
    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)
        ERR_EXIT("bind error");
    if(listen(server_sockfd,QUEUE) == -1)
        ERR_EXIT("listen error");
}

void *InGame(void *arg){
    int conn;
    ///客户端套接字
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);
    ///成功返回非负描述字，出错返回-1
    conn = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);
    if(conn<0)    ERR_EXIT("connect error");

    char buffer[BUFFER_SIZE];
    send(conn, "yourusername", sizeof("yourusername"), 0);
    recv(conn, buffer, sizeof(buffer), 0);

    char cards[BUFFER_SIZE];
    string usernow = buffer;
    strcpy(cards ,CardsToString(player[usernow]).c_str()); 
    send(conn, cards, sizeof(cards), 0);

    while(UserSequence.front() != buffer)    sleep(3);    
    

    close(conn);
    pthread_exit(NULL);
    return NULL;
}

int main(){

//     SocketConn(MYPORT);

    int rc;
//     ready_num=0;
//     pthread_t thread[3];
//     for(int i=0; i<3; i++){
//         rc = pthread_create(&thread[i], NULL, BigProcess, NULL);
//         if(rc)  ERR_EXIT("pthread create error");
//     }
//     for(int i=0; i<3; i++)   pthread_join(thread[i], NULL); 


// /*-------------------jiao di zhu--------------------------------------*/

//     SocketConn(MYPORT2);
//     while(UserSequence.size())  UserSequence.pop();
//     InitUsers(gameID, UserSequence);
//     ready_num=0;
//     first = UserSequence.front();
//     pthread_t thread2[3];
//     for(int i=0; i<3; i++){
//         rc = pthread_create(&thread2[i], NULL, CallLandlord, NULL);
//         if(rc)  ERR_EXIT("pthread create error");
//     }
//     for(int i=0; i<3; i++)   pthread_join(thread2[i], NULL);
//     SetLandlord(gameID, landlord.c_str());

// /*------------------dou di zhu-------------------------------------------------*/
    
    SocketConn(MYPORT3);

    InitCards();
    ShuffleCards();
    setLandlordCards(landlordcards);
    landlord="wutao";
    while(UserSequence.size())  UserSequence.pop();
    UserSequence.push(landlord);//landlord must be the first
    InitPlayer(gameID, UserSequence);

    player.clear();
    dealCards(player, UserSequence);

    pthread_t thread3[3];
    for(int i=0; i<3; i++){
        rc = pthread_create(&thread3[i], NULL, InGame, NULL);
        if(rc)  ERR_EXIT("pthread create error");
    }

    close(server_sockfd);
    return 0;
}