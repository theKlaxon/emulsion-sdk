btn <- 0
col <- 0
col_r <- 0
col_g <- 0
col_b <- 0
randint <- 7
num <- 1
a <- 0
b <- 0
c <- 0
btn_a <- [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
allenabled <- 0
btn_push <- 0
mon_d <- 0
mon <- 0
mon_c <- 0
mon_lg <- 0
i <- 0

function Think()
{
	if(allenabled==1)
	i+=1;
	if(i==5)
	{
	i=0;
	MonitorEnableAll();
	}
}
	

function BeginSequence()
{
	EntFire( "sound_fs-button02", "Playsound", "", 0 );
	a = 0;
	for(a==0;a<num;a+=1)
	{
		PickRandomButton();
		PickNewColor();
		Execute();
	}
}

function PickRandomButton()
{
	btn = RandomInt( 0, randint );
	btn_a[btn] = 1;
	if(btn<10)
	btn = "0"+btn;
}

function PickNewColor()
{
	col = RandomInt( 0, 3 );
	if(col==0)
	{
		col_r = 255;
		col_g = 0;
		col_b = 0;
	}
	if(col==1)
	{
		col_r = 0;
		col_g = 0;
		col_b = 255;
	}
	if(col==2)
	{
		col_r = 0;
		col_g = 255;
		col_b = 0;
	}
	if(col==3)
	{
		col_r = 255;
		col_g = 255;
		col_b = 0;
	}
}

function Execute()
{
	EntFire( "btn_prop-"+btn, "Skin", ""+col+"", 0 );
	EntFire( "btn_prop-"+btn, "SetAnimation", "ascend", 0 );
	EntFire( "btn_sprite-"+btn, "ColorRedValue", ""+col_r+"", 0 );
	EntFire( "btn_sprite-"+btn, "ColorGreenValue", ""+col_g+"", 0 );
	EntFire( "btn_sprite-"+btn, "ColorBlueValue", ""+col_b+"", 0 );
	EntFire( "btn_sprite-"+btn, "ShowSprite", "", 0 );
	EntFire( "btn_button-"+btn, "Unlock", "", 0.2 );
}

function ButtonPush(arg)
{
	btn_push = arg;
	if(btn_push<10)
	btn_push = "0"+btn_push;
	if(allenabled==0)
	{
		EntFire( "btn_prop-"+btn_push, "SetAnimation", "press", 0.00 );
		EntFire( "btn_button*", "Lock", "", 0.01 );
		EntFire( "sound_fs-button01", "Playsound", "", 0.1 );
		EntFire( "btn_sprite*", "HideSprite", "", 0.4 );
		EntFire( "btn_script", "RunScriptCode", "LowerRemaining()", 0.4 );
		MonitorSequence();
		EntFire( "btn_script", "RunScriptCode", "BeginSequence()", 2.4 );
	}
	if(allenabled==1)
	{
		EntFire( "btn_prop-"+btn_push, "SetAnimation", "press", 0.00 );
		EntFire( "btn_button-"+btn_push, "Lock", "", 0.01 );
		EntFire( "sound_fs-button01", "Playsound", "", 0.1 );
		EntFire( "btn_sprite-"+btn_push, "HideSprite", "", 0.4 );
		EntFire( "btn_prop-"+btn_push, "SetAnimation", "descend", 0.40 );
		PickNewColor();
		EntFire( "btn_prop-"+btn_push, "Skin", ""+col+"", 1.4 );
		EntFire( "btn_prop-"+btn_push, "SetAnimation", "ascend", 1.4 );
		EntFire( "btn_sprite-"+btn_push, "ColorRedValue", ""+col_r+"", 1.4 );
		EntFire( "btn_sprite-"+btn_push, "ColorGreenValue", ""+col_g+"", 1.4 );
		EntFire( "btn_sprite-"+btn_push, "ColorBlueValue", ""+col_b+"", 1.4 );
		EntFire( "btn_sprite-"+btn_push, "ShowSprite", "", 1.4 );
		EntFire( "btn_button-"+btn_push, "Unlock", "", 1.6 );
	}
}

function LowerRemaining()
{
	b = 0;
	for(b==0;b<=15;b+=1)
	{
		if(btn_a[b]==1)
		{
			btn_a[b] = 0;
			c = b;
			if(c<10)
			c="0"+c;
			EntFire( "btn_prop-"+c, "SetAnimation", "descend", 0 );
		}
	}
}

function EnableInner()
{
	randint = 15;
}

function IncrementButton()
{
	num+=1;
}

function EnableAll()
{
	EntFire( "btn_button*", "Lock", "", 0 );
	EntFire( "btn_sprite*", "HideSprite", "", 0 );
	LowerRemaining();
	EntFire( "btn_script", "RunScriptCode", "EnableAll_exec()", 1 );
}

function EnableAll_exec()
{
	allenabled = 1
	btn=0;
	c = 0;
	for(c==0;c<=15;c+=1)
	{
		btn = c;
		if(btn<10)
		btn ="0"+btn;
		PickNewColor();
		Execute();
	}
}

function MonitorSequence()
{	
	mon_d = btn_push;
	EntFire( "btn_mon_lg-"+btn_push+"", "SetMovie", "media/controlrm/monitor_lg-"+btn_push+"", 0.1 );
	EntFire( "btn_mon_lg-"+btn_push+"", "Enable", "", 0.2 );
	EntFire( "btn_script", "RunScriptCode", "MonitorExecute()", 0 );
	EntFire( "btn_script", "RunScriptCode", "MonitorExecute()", 0.5 );
	EntFire( "btn_script", "RunScriptCode", "MonitorExecute()", 1 );
	EntFire( "btn_script", "RunScriptCode", "MonitorExecute()", 1.5 );
	EntFire( "btn_mon_lg-"+btn_push+"", "Disable", "", 2 );
}

function MonitorExecute()
{
	mon = RandomInt(1,6);
	mon_c = RandomInt(1,4);
	EntFire( "btn_mon-"+mon_d+"-"+mon+"", "SetMovie", "media/controlrm/monitor0"+mon_c+"", 0.1 );
	EntFire( "btn_mon-"+mon_d+"-"+mon+"", "Enable", "", 0.2 );
	EntFire( "btn_mon-"+mon_d+"-"+mon+"", "Disable", "", 0.5 );
}

function MonitorEnableAll()
{
	mon_d = RandomInt(0,16);
	if(mon_d<10)
	mon_d = "0"+mon_d;
	MonitorExecute();
	mon_d = RandomInt(0,16);
	if(mon_d<10)
	mon_d = "0"+mon_d;
	MonitorExecute();
	mon_d = RandomInt(0,16);
	if(mon_d<10)
	mon_d = "0"+mon_d;
	MonitorExecute();
	mon_d = RandomInt(0,16);
	if(mon_d<10)
	mon_d = "0"+mon_d;
	MonitorExecute();
	mon_d = RandomInt(0,16);
	if(mon_d<10)
	mon_d = "0"+mon_d;
	MonitorExecute();
	mon_d = RandomInt(0,16);
	if(mon_d<10)
	mon_d = "0"+mon_d;
	MonitorExecute();
	mon_d = RandomInt(0,16);
	if(mon_d<10)
	mon_d = "0"+mon_d;
	MonitorExecute();
	mon_d = RandomInt(0,16);
	if(mon_d<10)
	mon_d = "0"+mon_d;
	MonitorExecute();
	mon_d = RandomInt(0,16);
	if(mon_d<10)
	mon_d = "0"+mon_d;
	MonitorExecute();
	mon_d = RandomInt(0,16);
	if(mon_d<10)
	mon_d = "0"+mon_d;
	MonitorExecute();
	mon_d = RandomInt(0,16);
	if(mon_d<10)
	mon_d = "0"+mon_d;
	EntFire( "btn_mon_lg-"+mon_d+"", "SetMovie", "media/controlrm/monitor_lg-"+mon_d+"", 0.1 );
	EntFire( "btn_mon_lg-"+mon_d+"", "Enable", "", 0.2 );
	EntFire( "btn_mon_lg-"+mon_d+"", "Disable", "", 2 );
	mon_d = RandomInt(0,16);
	if(mon_d<10)
	mon_d = "0"+mon_d;
	EntFire( "btn_mon_lg-"+mon_d+"", "SetMovie", "media/controlrm/monitor_lg-"+mon_d+"", 0.1 );
	EntFire( "btn_mon_lg-"+mon_d+"", "Enable", "", 0.2 );
	EntFire( "btn_mon_lg-"+mon_d+"", "Disable", "", 2 );
}