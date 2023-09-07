timer <- 60;
i <- 10;
start <- false;

function Think()
{
	if(start==true && timer!=0)
	{
		i-=1;
		printl(timer+"."+i);
		if(i==0)
		{
			i=10;
			timer -= 1;
			DispMsg();
		}
	}
}

function DispMsg()
{
	EntFireByHandle(EntityGroup[0],"AddOutput","message TSPT_"+timer+"",0,null,null);
	EntFireByHandle(EntityGroup[0],"ShowMessage","",0,null,null);
}

function Start()
{
	start = true;
	DispMsg();
}