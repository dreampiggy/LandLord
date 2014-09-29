#include <iostream>
#include <algorithm>
#include <ctime>
#include <string>
#include <vector>
#include <stdio.h>
using namespace std;
#define rep(i,x,y) for(int i=x;i<=y;i++)
#define DEAL_NUM 17
//发牌的时候每个人的牌数限定是17张

struct poke{
	int flw, num;
}p[55];
int a[55];//hash
string s[55]={
"",
"H.A", "H.2", "H.3", "H.4", "H.5", "H.6", "H.7", "H.8", "H.9", "H.10", "H.J", "H.Q", "H.K",
"S.A", "S.2", "S.3", "S.4", "S.5", "S.6", "S.7", "S.8", "S.9", "S.10", "S.J", "S.Q", "S.K",
"C.A", "C.2", "C.3", "C.4", "C.5", "C.6", "C.7", "C.8", "C.9", "C.10", "C.J", "C.Q", "C.K",
"D.A", "D.2", "D.3", "D.4", "D.5", "D.6", "D.7", "D.8", "D.9", "D.10", "D.J", "D.Q", "D.K",
"jkr", "JKR" };
//牌号下标从1开始，花色从0开始
//为了统一长度，我把jocker写成jkr
int index(int flw, int num){
	if(flw==4)	return 52+num;
	if(num>=3)
		return (num-3)*4+flw+1;
	if(num<3)//可省
		return 11*4+flw+1;
}

bool cmp(const poke& a, const poke& b){
	return index(a.flw, a.num) < index(b.flw, b.num);
}

void InitCards(){
	rep(i,0,3){
		rep(j,1,13){
			int id=i*13+j;
			p[id].flw = i;
			p[id].num = j;
		}	
	}
	p[53].flw=4;p[53].num=1;//小王
	p[54].flw=4;p[54].num=2;//大王
}

void ShuffleCards(){
	srand(time(0));
	rep(i,1,54)	a[i] = i;
	for(int i=54;i>=1;i--)
		swap(a[i],a[rand()%i+1]);
}//洗牌的复杂度为O(n)

string getCards(int flw, int num){//H.7的flw=0, num=7
	return s[flw*13+num];
}

void dealCards(vector<poke>* player){//发牌
	rep(id, 0, 2){
		player[id].clear();
		rep(i, DEAL_NUM*id+1, DEAL_NUM*(id+1))
			player[id].push_back(p[a[i]]);
	}
}

void sortCards(vector<poke>& player){
	sort( player.begin(), player.end(), cmp );//给p1的牌排序
}

void printCards(vector<poke>& player){
	for(int i=0; i<player.size(); i++){
		cout<<getCards( player[i].flw, player[i].num );
		if(i==player.size()-1)
			continue;
		if(player[i].num==10)//牌的间距是3格，有10的话间距会变成2格
			printf("  ");
		else printf("   ");
	}
	puts("");
	for(int i=0; i<player.size(); i++){
		printf(" %d",i+1);
		if(i==player.size()-1)	continue;
		if(i>=9)
			printf("   ");
		else
			printf("    ");
	}
	//按道理，下面要有1,2,3,4...n来提示用户，暂时不写
}

int main(){
	vector<poke> player[3];//玩家一定是作为一个数组
	InitCards();
	ShuffleCards();
	dealCards(player);
	sortCards(player[0]);
	printCards(player[0]);

	return 0;
}

/*
DESCRIPTION:
洗牌+发牌
用c++封装的时候main里的都是public接口，其他都放private
进一步的，vector<poke>可以用typedef改掉
3 3 3 3
4 4 4 4
.......
K K K K
A A A A
2 2 2 2
j j     
*/ 