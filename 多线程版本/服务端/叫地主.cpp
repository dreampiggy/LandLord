#include <time.h>
#include <stdlib.h>
using namespace std;
int menu()
{
	int k;
	cout<<"type 1 to start game,type 0 to quit"<<endl;
	cin>>k;
	switch(k)
	{
		case 0:
			cout<<"quit\n";
			return 0;
		case 1:
			cout<<"start\n";
			return 1;
	}
	return k;
}

void initialize(int brand[]) //initialize
{
	int i=0;
	while(i<54)
	{
		if(i==0||i==1||i=2||i=3)
		{
			brand[i++]=52+i;
		}
		else if(i==4||i==5||i==6)
		{
			brand[i++]=56+i;
		}
		else if(i==52||i==53)
		{
			brand[i++]=12+i;
		}
		else
		{
			brand[i++]=i;
		}
	}
}

void output(int brand[])
{
	int i=0;
	for(;i<54;++i)
	{
		cout<<brand[i]<<endl;
	}
}

void output1(int *plate,int *fig,int k)
{
	int i;
	cout<<endl;
	for(i=0;i<k;++i)
	{
		if(plate[i]>1&&plate[i]<11)
		{
			cout<<static_cast<char>(fig[i])<<plate[i];
		}
		else
		{
			switch(plate[i])
			{
				case 11:
					cout<<static_cast<char>(fig[i])<<'J';
					break;
				case 12:
					cout<<static_cast<char>(fig[i])<<'Q';
					break;
				case 13:
					cout<<static_cast<char>(fig[i])<<'K';
					break;
				case 14:
					cout<<static_cast<char>(fig[i])<<'A';
					break;
				case 16:
					cout<<static_cast<char>(fig[i])<<2;
					break;
				case 17:
					cout<<static_cast<char>(fig[i])<<'jk';
					break;
				case 18:
					cout<<static_cast<char>(fig[i])<<'JK';
					break;
			}
		}
	}
}

void output2(list *enjoy)
{
	cout<<"player1 card: ";
	output1(enjoy->platex,enjoy->figx,enjoy->openx);
	cout<<endl;
	cout<<"player2 card: ";
	output1(enjoy->platey,enjoy->figy,enjoy->openy);
	cout<<endl;
	cout<<"player3 card: ";
	output1(enjoy->platez,enjoy->figz,enjoy->openz);
	cout<<endl;
}

void optout(list *enjoy,int j)
{
	cout<<j;
	if(j==0)
	{
		enjoy->openx=20,enjoy->openy=17,enjoy->openz=17;
	}
	else if(j==1)
	{
		enjoy->openx=17,enjoy->openy=20,enjoy->openz=17;
	}
	else
	{
		enjoy->openx=17,enjoy->openy=17,enjoy->openz=20;
	}
	output2(enjoy);
}

void riffle(int brand[])
{
	int i,j,k=0;
	int commute;
	while(k<100)
	{
		i=rand()%54;
		j=rand()%54;
		commute=brand[i];
		brand[i]=brand[j];
		brand[j]=commute;
		++k;
	}
}

void deal(int brand[],list *enjoy)
{
	int i;
	for(i=0;i<17;++i)
	{
		enjoy->x[i]=brand[0+i*3];
		enjoy->y[i]=brand[1+i*3];
		enjoy->z[i]=brand[2+i*3];
	}
]

list *initlish()
{
	lisht *enjoy;
	enjoy=(list*)malloc(sizeof(list));
	enjoy->x=(int*)malloc(sizeof(int)*20);
	enjoy->figx=(int*)malloc(sizeof(int)*20);
	enjoy->platex=(int*)malloc(sizeof(int)*20);
	enjoy->y=(int*)malloc(sizeof(int)*20);
	enjoy->figy=(int*)malloc(sizeof(int)*20);
	enjoy->platey=(int*)malloc(sizeof(int)*20);
	enjoy->z=(int*)malloc(sizeof(int)*20);
	enjoy->figz=(int*)malloc(sizeof(int)*20);
	enjoy->platez=(int*)malloc(sizeof(int)*20);
	return enjoy;
}

int taxis1(int *address,int low ,int high)
{
	int work;
	work=address[low];
	while(low<high)
	{
		while(low<high&&address[high]<=work)
		{
			--high;
		}
		address[low]=address[high];
		while(low<high&&address[low]>=work)
		{
			++low;
		}
		address[high]=address[low];
	}
	address[low]=work;
	return low;
}

void taxis(int *address,int low,int high)
{
	int work;
	if(low<high)
	{
		work=taxis1(address,low,high);
		taxis(address,low,work-1);
		taxis(address,work+1,high);
	}
}

void output_x(int x)
{
	cout<<x<<endl;
}

void output_y(int y)
{
	cout<<y<<endl;
}

void output_z(int z)
{
	cout<<z<<endl;
}

int compare(int x,int y,int z)
{
	if(x>y)
	{
		if(x>z)
		{
			return 0;
		}
		else
		{
			return 2;
		}
	}
	else
	{
		if(y>z)
		{
			return 1;
		}
		else
		{
			return 2;
		}
	}
}

int callcent1(int *x)
{
	*x=rand()%4;
	if(*x==3)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}
int callent2(int x,int *y)
{
	*y=rand%(4-x);
	if(*y!=0)
	{
		*y=(*y)+x;
	}
	if(*y==3)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

int callent3(int x,int y)
{
	int maxdata;
	if(x>y)
	{
		maxdata=x;
	}
	else
	{
		maxdata=y;
	}
	return maxdata;
}

int callcent4(int maxdata,int *z)
{
	*z=rand()%(4-maxdata);
	if(*z!=0)
	{
		*z=(*z)+maxdata;
	}
	if(*z==3)
	{
		return 2;
	}
	else
	{
		return -1;
	}
}

int callcent5(int x,int y,int z)
{
	if(x==0&&y==0&&z==0)
	{
		cout<<"x=="<<x<<endl;
		cout<<"y=="<<y<<endl;
		cout<<"z=="<<z<<endl;
		cout<<"restart\n";
		return -1;
	}
	else
	{
		return compare(x,y,z);
	}
}

int callcent6(int *x,int maxdata)
{
	while(*x<0||*x>3-maxdata)
	{
		cout<<"