#include <iostream>
#include <cmath>
using namespace std;

class scoreCount
{
public:
	scoreCount(int bomb ,int landlord, int base)
	{
		bombnum=bomb;
		landlordnum=landlord;
		basescore=base;
	}
	int getscore()
	{
		return pow(2,bombnum+landlordnum)*basescore;
	}
	void display_Score()
	{
		cout<<getscore();
	}
private:
	int bombnum;
	int landlordnum;
	int basescore;
};

int main()
{
	scoreCount a(2,1,10);
	a.display_Score();
}