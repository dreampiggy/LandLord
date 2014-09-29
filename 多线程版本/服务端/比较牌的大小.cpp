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
