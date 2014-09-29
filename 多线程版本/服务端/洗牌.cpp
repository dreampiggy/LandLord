#include<iostream>
#include<string>
#include<algorithm>
#include<ctime>
#include<cstdlib>

int NUMBER=17;
#define rep(i,x,y) for(int i=x;i<y;i++)
using namespace std;

string doudizhu[54]=
{"H.A", "H.2", "H.3", "H.4", "H.5", "H.6", "H.7", "H.8", "H.9", "H.10", "H.J", "H.Q", "H.K",
"S.A", "S.2", "S.3", "S.4", "S.5", "S.6", "S.7", "S.8", "S.9", "S.10", "S.J", "S.Q", "S.K",
"C.A", "C.2", "C.3", "C.4", "C.5", "C.6", "C.7", "C.8", "C.9", "C.10", "C.J", "C.Q", "C.K",
"D.A", "D.2", "D.3", "D.4", "D.5", "D.6", "D.7", "D.8", "D.9", "D.10", "D.J", "D.Q", "D.K",
"jkr", "JKR" };

void card(int *value,string *doudizhu)
{
	rep(i,0,54){
		value[i]=i;
	}
}

void Xipai()
{
	rep(i,0,54){
		srand(time(0));
		swap(doudizhu[i],doudizhu[rand()%(i+1)]);
	}
}

void Faipai()
{
	int temp=0;
	rep(m,0,3){
		cout<<"Player"<<m+1<<":\n";
		rep(i,temp,NUMBER){
			cout<<doudizhu[i]<<"  ";
		}
		cout<<endl;
		temp+=17;
		NUMBER+=17;
	}
	cout<<"DiPai:";
	rep(i,temp,54){
		cout<<doudizhu[i]<<"  ";
	}
	cout<<endl;
}

int main()
{
	Xipai();
	Faipai();
	return 0;
}