/*
EntityGroup[0] = min_ten
EntityGroup[1] = min_one
EntityGroup[2] = sec_ten
EntityGroup[3] = sec_one
EntityGroup[4] = ms_hun
EntityGroup[5] = ms_ten
EntityGroup[6] = ms_ten timer
*/

enabled <- false;
ten_enabled <- false;
t_total <- 0;
t_min_ten <- 0;
t_min_one <- 0;
t_sec_ten <- 0;
t_sec_one <- 0;
t_hun <- 0;


DEBUG <- false;


function Think()
{
if(enabled)
{
	EntFireByHandle( EntityGroup[4], "SetTextureIndex", ""+(9-t_hun)+"", 0, null, null);
	EntFireByHandle( EntityGroup[5], "SetTextureIndex", "0", 0, null, null);
	t_hun += 1;
	if(t_hun == 10)
	{
		t_hun = 0;
		t_total += 1;
		if( floor(t_total/600) > t_min_ten )
		{
			t_min_ten += 1;
			t_min_one = 0;
			t_sec_ten = 0;
		}
		if( floor( (t_total/60) - (10*t_min_ten) ) > t_min_one )
		{
			t_min_one += 1;
			t_sec_ten = 0;
		}
		if( floor( (t_total/10) - (60*t_min_ten) - (6*t_min_one)) > t_sec_ten )
			t_sec_ten += 1;
		t_sec_one += 1;
		if(t_sec_one == 10)
			t_sec_one = 0;
		Refresh();

		if(ten_enabled == false)
		{
			ten_enabled = true;
			EntFireByHandle( EntityGroup[6], "Enable", "", 0, null, null);
		}	
	}
	if(DEBUG)
		printl(t_min_ten+""+t_min_one+" : "+t_sec_ten+""+t_sec_one+" : "+t_hun+"0 |||||| "+t_total);
}
}

function SetTime(arg)
{
	if(arg>=0)
	{
		t_total = arg.tointeger();
		t_min_ten = floor(t_total/600);
		t_min_one = floor( (t_total/60) - (10*t_min_ten) );
		t_sec_ten = floor( (t_total/10) - (60*t_min_ten) - (6*t_min_one));
		t_sec_one = floor( (t_total) - (600*t_min_ten) - (60*t_min_one) - (10*t_sec_ten));
		Refresh();
	}
}

function Enable()
{
	enabled = true;
}

function Disable()
{
	enabled = false;
	ten_enabled == false;
	EntFireByHandle( EntityGroup[6], "Disable", "", 0, null, null );
}

function Refresh()
{
	EntFireByHandle( EntityGroup[0], "SetTextureIndex", ""+(9-t_min_ten)+"", 0, null, null);
	EntFireByHandle( EntityGroup[1], "SetTextureIndex", ""+(9-t_min_one)+"", 0, null, null);
	EntFireByHandle( EntityGroup[2], "SetTextureIndex", ""+(9-t_sec_ten)+"", 0, null, null);
	EntFireByHandle( EntityGroup[3], "SetTextureIndex", ""+(9-t_sec_one)+"", 0, null, null);
}

Refresh();
EntFireByHandle( EntityGroup[4], "SetTextureIndex", "9", 0, null, null);
EntFireByHandle( EntityGroup[5], "SetTextureIndex", "9", 0, null, null);