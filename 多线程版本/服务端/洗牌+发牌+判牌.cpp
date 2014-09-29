#include <iostream>
#include <algorithm>
#include <ctime>
#include <string>
#include <vector>
#include <stdio.h>
using namespace std;
#define rep(i,x,y) for(int i=x;i<=y;i++)
#define DEAL_NUM 17 //���Ƶ�ʱ��ÿ���˵������޶���17��
#define MAXLEN  1024 //�ͻ��˽��ܵ��ַ�����Ϣ�ĳ���

struct poke{
	int flw, num;
}p[55];
int index(int flw, int num){
	if(flw==4)	return 52+num;
	if(num>=3)
		return (num-3)*4+flw+1;
	if(num<3)//��ʡ
		return (10+num)*4+flw+1;
}
bool cmp(const poke& a, const poke& b){
	return index(a.flw, a.num) < index(b.flw, b.num);
}
/*index �� cmp��������*/

string s[55]={
"",
"H.A", "H.2", "H.3", "H.4", "H.5", "H.6", "H.7", "H.8", "H.9", "H.10", "H.J", "H.Q", "H.K",
"S.A", "S.2", "S.3", "S.4", "S.5", "S.6", "S.7", "S.8", "S.9", "S.10", "S.J", "S.Q", "S.K",
"C.A", "C.2", "C.3", "C.4", "C.5", "C.6", "C.7", "C.8", "C.9", "C.10", "C.J", "C.Q", "C.K",
"D.A", "D.2", "D.3", "D.4", "D.5", "D.6", "D.7", "D.8", "D.9", "D.10", "D.J", "D.Q", "D.K",
"jkr", "JKR" };
//�ƺ��±��1��ʼ����ɫ��0��ʼ
//Ϊ��ͳһ���ȣ��Ұ�jockerд��jkr
/*s�������*/

void InitCards(){
	rep(i,0,3){
		rep(j,1,13){
			int id=i*13+j;
			p[id].flw = i;
			p[id].num = j;
		}
	}
	p[53].flw=4;p[53].num=1;//С��
	p[54].flw=4;p[54].num=2;//����
}


int a[55];//hash
void ShuffleCards(){
	srand(time(0));
	rep(i,1,54)	a[i] = i;
	for(int i=54;i>=1;i--)
		swap(a[i],a[rand()%i+1]);
}//ϴ�Ƶĸ��Ӷ�ΪO(n)

string getCards(int flw, int num){//H.7��flw=0, num=7
	return s[flw*13+num];
}

void dealCards(vector<poke>* player){//����
	rep(id, 0, 2){
		player[id].clear();
		rep(i, DEAL_NUM*id+1, DEAL_NUM*(id+1))
			player[id].push_back(p[a[i]]);
	}
}

void sortCards(vector<poke>& player){
	sort( player.begin(), player.end(), cmp );//��p1��������
}

string getCardList(vector<poke>& player)
{
    string cardList="";
    for(int i=0;i<player.size();i++)
    {
        cardList+=getCards( player[i].flw, player[i].num );
        if(i==player.size()-1)
            continue;
        if(player[i].num==10)//�Ƶļ����3����10�Ļ�������2��
            cardList+="  ";
        else cardList+="   ";
    }

}





void printCards(vector<poke>& player){
	for(int i=0; i<player.size(); i++){
		cout<<getCards( player[i].flw, player[i].num );
		if(i==player.size()-1)
			continue;
		if(player[i].num==10)//�Ƶļ����3����10�Ļ�������2��
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
	//����������Ҫ��1,2,3,4...n����ʾ�û�����ʱ��д
}


// /*���������Ʋ���*/
bool isNumeric(char x){
	return x<='9' && x>='0';
}
void readGo(char* buf, vector<int>& v){//��ָ��ת��Ϊ����
	v.clear();//�����
	int i=0;
	while(buf[i]!='\0'){
		if(!isNumeric(buf[i])){
			i++;
			continue;
		}
		int x=buf[i]-'0';
		if(isNumeric(buf[i+1])){
			x = x*10+buf[i+1]-'0';
			v.push_back(x);
			i+=2;
		}
		else{
			v.push_back(x);
			i++;
		}
	}
}
void getGo(const vector<int>& v, const vector<poke>& player, vector<poke>& GoCards){//ȡ����ҵ����ƣ�����GoCards������
	for(int i=0; i<v.size(); i++){
		GoCards.push_back(player[v[i]-1]);
	}
}
/*��ȡָ��*/


bool isSingle(vector<poke>& Go){  //����
	return Go.size()==1;
}

bool isDouble(vector<poke>& Go)   //һ��
{
    if(Go.size()!=2)
        return 0;
    return Go[0].num==Go[1].num;
}

int isTriple(vector<poke>& Go){   //���Ļ�����1������һ�ͷ���2��ʲô�����ǵķ���0  (�ܲ��ܳ����Ŷ���)
 	if(Go.size()==4)
    {
        int same=0,i=1;
        while( i<Go.size() && Go[i].num==Go[i-1].num)
        {
            same++;
            i++;
        }
        if(same!=0&&same!=2)
            return 0;
        if(same==0)
        {
            if(Go[1].num==2)   //���Ŵ�һ�Ų��ܳ�������2
                return 0;
            if(Go[1].num==Go[2].num && Go[2].num==Go[3].num)
                return 2;
            else
                return 0;
        }
        if(Go[1].num!=2)  //���Ŵ�һ�Ų��ܳ�������2
            return 2;
        else
            return 0;

    }
    else
        return  Go[0].num==Go[1].num && Go[1].num==Go[2].num;
 }//Ϊ�˾�����룬�Ҳ�׼������������һ�ֳ���������д

 bool isTriple_Two(vector<poke>& Go) //���Ŵ�һ��
 {
     if(Go.size()!=5)
        return 0;
     int i=1;
     int same=0;
     while(i<Go.size() && Go[i].num==Go[i-1].num)
     {
         same++;
         i++;
     }
     if(same!=1 && same!=2)
         return 0;
     if((same==2 && Go[3].num==Go[4].num && Go[2].num!=2)||(same==1 && Go[2].num==Go[3].num && Go[3].num==Go[4].num && Go[2].num!=2))
         return 1;
     else
         return 0;
 }

bool isSequence(vector<poke>& Go) //˳��
{
	for(int i=1; i<Go.size(); i++)
    {
		if((Go[i].num-Go[i-1].num)!=1 ||Go[i-1].num==2||Go[i-1].flw==4) //˳���ﲻ����2����
			return 0;
	}
	return 1;
}

bool isDsequence(vector<poke>& Go) //����
{
    if(Go.size()<6||(Go.size()%2)!=0)
        return 0;
    for(int i=0,same=0;i<Go.size();i++)
    {
        if(Go[i].flw==4||Go[i].num==2) //�����в������2����
            return 0;
        same=0;
        while(i<Go.size()-1 && Go[i].num==Go[i+1].num )
        {
            same++;
            i++;
        }
        if(same!=1)
            return 0;
        if(i!=0&& i!=1 &&(Go[i].num-Go[i-2].num)!=1)
            return 0;
    }
    return 1;
}

bool isTriple_Ssequence(vector<poke>& Go) //���Ŵ�һ�ŵ�˳��
{
    if(Go.size()<=4||Go.size()%4!=0)
        return 0;
    int same=0;
    int single=0;
    int triple=0;
    for(int i=0;i<Go.size();i++)
    {
        same=0;
        while(i<Go.size()-1 && Go[i].num==Go[i+1].num)
        {
            same++;
            i++;
        }
        if(same!=0&&same!=1&&same!=2)
            return 0;
        if(same==0)
            single++;
        if(same==1)
            single+=2;
        if(same==2)
        {
            if(Go[i].num==2)  //���Ŵ�һ�Ų��ܳ�������2
                return 0;
            else
                triple++;
        }

    }
    return single==triple;
}

bool isTriple_Dsequence(vector<poke>& Go) // ���Ŵ�һ�Ե�˳��
{
    if(Go.size()<=5||Go.size()%5!=0)
        return 0;
    int same=0;
    int couple=0;
    int triple=0;
    for(int i=0;i<Go.size();i++)
    {
        same=0;
        while(i<Go.size()-1 && Go[i].num==Go[i+1].num)
        {
            same++;
            i++;
        }
        if(same!=1&&same!=2)
            return 0;
        if(same==1)
            couple++;
        if(same==2)
            triple++;
    }
    return couple==triple;
}

bool isFour_Two_Differ(vector<poke>& Go) //���Ŵ�����
{
    if(Go.size()!=6)
        return 0;

    int same=0;
    for(int i=0;i<Go.size();i++)
    {
        same=0;
        while( i<Go.size()-1 && Go[i].num==Go[i+1].num)
        {
            same++;
            i++;
        }
        if(same==3)
            return true;
    }
    return false;
}

bool isFour_Two_Same(vector<poke>& Go) //���Ŵ�����
{
    if(Go.size()!=8)
        return 0;
    int couple=0;
    bool flag=false;
    for(int i=0,same=0;i<Go.size();i++)
    {
        same=0;
        while(i<Go.size()-1 && Go[i].num==Go[i+1].num )
        {
            same++;
            i++;
        }
        if(same!=1 && same!=3)
            return 0;
        if(same==3)
            flag=true;
        if(same==1)
            couple++;
    }
    return couple==2 && flag;
}

bool isBomb(vector<poke>& Go) //ը��
{
    if(Go.size()!=4)
        return 0;
    return Go[0].num==Go[1].num && Go[1].num==Go[2].num && Go[2].num==Go[3].num;
}

bool isRocket(vector<poke>& Go) //���
{
    if(Go.size()!=2)
        return 0;
    return Go[0].flw==4 && Go[1].flw==4;
}

// bool judgeGo(vector<poke>& GoCards){
// 	sort(GoCards.begin(), GoCards.end());
// }

int main(){
	freopen("a.txt","r",stdin);

	vector<poke> player[3];//���һ������Ϊһ������
	InitCards();//
	ShuffleCards();
	dealCards(player);
	sortCards(player[0]);
    sortCards(player[1]);
	printCards(player[0]);
	cout<<endl;
    printCards(player[1]);
    cout<<endl;

	char buff[MAXLEN];
	vector<int> inputArr;
	vector<poke> GoCards;
	GoCards.clear();

	GoCards.push_back((poke){0,1});
	GoCards.push_back((poke){0,2});
	GoCards.push_back((poke){0,3});
	GoCards.push_back((poke){0,4});

	sortCards(GoCards);
	cout << isBomb(GoCards);

	//gets(buff);
	//���м�Ҫ��һ���жϵ�ǰ�û��ܷ����
	//readGo(buff, inputArr);
	//�����һ���ж����Ƿ�Ϊ��

	//getGo(inputArr, player[0], GoCards);//�õ���Ҵ��������
	//for(int i=0; i<GoCards.size(); i++)
		//cout<<getCards(GoCards[i].flw, GoCards[i].num)<<" ";
	// //judgeGo(GoCards);

	return 0;
}

/*
DESCRIPTION:
ϴ��+����
��c++��װ��ʱ��main��Ķ���public�ӿڣ���������private
��һ���ģ�vector<poke>������typedef�ĵ�
*/
