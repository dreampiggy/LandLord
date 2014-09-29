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
