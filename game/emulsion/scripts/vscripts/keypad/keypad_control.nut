//Keypad controller script, by Collin Eddings

/*
ENTITYGROUP REFERENCE SHEET:

0 = keypad
1 = button 1
2 = button 2
3 = button 3
4 = button 4
5 = button 5
6 = button 6
7 = button 7
8 = button 8
9 = button 9
10 = button 0
11 = screen number 1
12 = screen number 2
13 = screen number 3
14 = screen number 4
Sequences: FireUser1 = Button Press; FireUser2 = Incorrect Number; FireUser3 = Correct Number; FireUser4 = Inactivity, keypad resetted
*/

//Setting variables to the entities listed in the logic_script entity (reference listed above)
ent_keypad <- EntityGroup[0].GetName();
ent_b0 <- EntityGroup[10].GetName();
ent_b1 <- EntityGroup[1].GetName();
ent_b2 <- EntityGroup[2].GetName();
ent_b3 <- EntityGroup[3].GetName();
ent_b4 <- EntityGroup[4].GetName();
ent_b5 <- EntityGroup[5].GetName();
ent_b6 <- EntityGroup[6].GetName();
ent_b7 <- EntityGroup[7].GetName();
ent_b8 <- EntityGroup[8].GetName();
ent_b9 <- EntityGroup[9].GetName();
ent_n1 <- EntityGroup[11].GetName();
ent_n2 <- EntityGroup[12].GetName();
ent_n3 <- EntityGroup[13].GetName();
ent_n4 <- EntityGroup[14].GetName();

//Other variables needed
var_curnum <- 1;	//Current number out of four that it's using. Runs comparison when it reaches 5.
var_num <- 0;		//Number that has been input to the keypad
var_num1 <- 0;		//Individual digits of input number
var_num2 <- 0;
var_num3 <- 0;
var_num4 <- 0;
var_finnum <- 2845;	//The correct number.
var_calc <- 0;		//Garbage variable used in calculating
var_activated <- 0;	//Should we ignore keypad presses or not?

//Attaching buttons to their position on the model
//EntFire(ent_b0,"SetParentAttachment","b0",0);
//EntFire(ent_b1,"SetParentAttachment","b1",0);
//EntFire(ent_b2,"SetParentAttachment","b2",0);
//EntFire(ent_b3,"SetParentAttachment","b3",0);
//EntFire(ent_b4,"SetParentAttachment","b4",0);
//EntFire(ent_b5,"SetParentAttachment","b5",0);
//EntFire(ent_b6,"SetParentAttachment","b6",0);
//EntFire(ent_b7,"SetParentAttachment","b7",0);
//EntFire(ent_b8,"SetParentAttachment","b8",0);
//EntFire(ent_b9,"SetParentAttachment","b9",0);
//...wait...
//Just kidding. Thanks Valve. You broke func_button's so they don't work on model attachments. You totally ruined my weekend there. I'm leaving this
//script in here for the whole world to see! Or, rather, a very very small fraction of it who actually hunts around in VScript files. Why are you even
//looking at this anyway? Go play outside. I've never been, but I've heard most of the buttons out there actually work when they're attached to things.

//Alright, alright, you came here for the vscript. There's no stopping you, is there? WELL JUST GO AHEAD THEN. SEE IF I CARE.

//Attaching screen numbers to their position on the model
EntFire(ent_n1,"SetParentAttachment","n1",0);
EntFire(ent_n2,"SetParentAttachment","n2",0);
EntFire(ent_n3,"SetParentAttachment","n3",0);
EntFire(ent_n4,"SetParentAttachment","n4",0);
//Oh-hoh. Look at that. At least the numbers still work. So I've got that going for me.

function TurnOn()
{	
//	EntFire(ent_b0,"Unlock","",0);
//	EntFire(ent_b1,"Unlock","",0);
//	EntFire(ent_b2,"Unlock","",0);
//	EntFire(ent_b3,"Unlock","",0);
//	EntFire(ent_b4,"Unlock","",0);
//	EntFire(ent_b5,"Unlock","",0);
//	EntFire(ent_b6,"Unlock","",0);
//	EntFire(ent_b7,"Unlock","",0);
//	EntFire(ent_b8,"Unlock","",0);
//	EntFire(ent_b9,"Unlock","",0);
//	Oh yeah, and heaven forbid I try to lock a button while it's pressed down! That was a fun one to work around, too.

	var_activated = 1;
	var_num = 0;
	
	var_num1 = 0;
	var_num2 = 0;
	var_num3 = 0;
	var_num4 = 0;

	//Setting screen numbers to the 'dash' skin
	EntFire(ent_n1,"Skin",10,0);
	EntFire(ent_n2,"Skin",10,0);
	EntFire(ent_n3,"Skin",10,0);
	EntFire(ent_n4,"Skin",10,0);
	
	//Setting keypad model to default on skin
	EntFire(ent_keypad,"Skin",1,0);
}

function Input(arg)
{
if(var_activated==1)
{
	//Starts the animation on the keypad, fires User1 on keypad script entity, changes skin of screen number, etc.
	EntFire(ent_keypad,"SetAnimation","press"+arg,0);
	if(var_curnum==1)
	{
		EntFire(ent_n1,"Skin",""+arg+"",0);
		var_num1=arg;
		var_calc=1000;
	}
	if(var_curnum==2)
	{
		EntFire(ent_n2,"Skin",""+arg+"",0);
		var_num2=arg;
		var_calc=100;
	}
	if(var_curnum==3)
	{
		EntFire(ent_n3,"Skin",""+arg+"",0);
		var_num3=arg;
		var_calc=10;
	}
	if(var_curnum==4)
	{
		EntFire(ent_n4,"Skin",""+arg+"",0);
		var_num4=arg;
		var_calc=1;
	}
	EntFire(self.GetName(),"FireUser1","",0);

	//Deactivate the keys so the keypad has enough time to play the animation. It'll be reactivated a few lines down below
	var_activated = 0;

	//Adds the new number to the value stored, if var_curnum reaches 5 compare the value.
	var_num += arg*var_calc;
	var_curnum += 1;
	if(var_curnum==5)
	{
		EntFire(self.GetName(),"RunScriptCode","BeginCompare();",0.02);
		var_curnum = 1;
	}
	else
		EntFire(self.GetName(),"RunScriptCode","var_activated = 1;",0.5);
}
}

//Little easter egg sequence I made just for fun. Scrolls '427' across the screen and flashes colors. You can still access it
//in-game with some console commands (ent_fire <keypad controller entity> runscriptcode "StanleySeq();").
function StanleySeq()
{
	EntFire(self.GetName(),"FireUser2","",0);
	EntFire(ent_keypad,"Skin",3,0);
	EntFire(ent_n1,"Skin",11,0);
	EntFire(ent_n2,"Skin",11,0);
	EntFire(ent_n3,"Skin",11,0);
	EntFire(ent_n4,"Skin",10,0);
	EntFire(ent_n1,"Skin",11,0.1);
	EntFire(ent_n2,"Skin",11,0.1);
	EntFire(ent_n3,"Skin",10,0.1);
	EntFire(ent_n4,"Skin",4,0.1);
	EntFire(ent_keypad,"Skin",2,0.2);
	EntFire(ent_n1,"Skin",11,0.2);
	EntFire(ent_n2,"Skin",10,0.2);
	EntFire(ent_n3,"Skin",4,0.2);
	EntFire(ent_n4,"Skin",2,0.2);
	EntFire(ent_n1,"Skin",10,0.3);
	EntFire(ent_n2,"Skin",4,0.3);
	EntFire(ent_n3,"Skin",2,0.3);
	EntFire(ent_n4,"Skin",7,0.3);
	EntFire(ent_keypad,"Skin",1,0.4);
	EntFire(ent_n1,"Skin",10,0.4);
	EntFire(ent_n2,"Skin",4,0.4);
	EntFire(ent_n3,"Skin",2,0.4);
	EntFire(ent_n4,"Skin",7,0.4);
	EntFire(ent_n1,"Skin",4,0.5);
	EntFire(ent_n2,"Skin",2,0.5);
	EntFire(ent_n3,"Skin",7,0.5);
	EntFire(ent_n4,"Skin",10,0.5);
	EntFire(ent_keypad,"Skin",4,0.6);
	EntFire(ent_n1,"Skin",2,0.6);
	EntFire(ent_n2,"Skin",7,0.6);
	EntFire(ent_n3,"Skin",10,0.6);
	EntFire(ent_n4,"Skin",11,0.6);
	EntFire(ent_n1,"Skin",7,0.7);
	EntFire(ent_n2,"Skin",10,0.7);
	EntFire(ent_n3,"Skin",11,0.7);
	EntFire(ent_n4,"Skin",11,0.7);
	EntFire(ent_keypad,"Skin",1,0.8);
	EntFire(ent_n1,"Skin",10,0.8);
	EntFire(ent_n2,"Skin",11,0.8);
	EntFire(ent_n3,"Skin",11,0.8);
	EntFire(ent_n4,"Skin",11,0.8);
	EntFire(ent_n1,"Skin",11,0.9);
	EntFire(ent_n2,"Skin",11,0.9);
	EntFire(ent_n3,"Skin",11,0.9);
	EntFire(ent_n4,"Skin",11,0.9);
	EntFire(ent_keypad,"Skin",2,0);
	EntFire(ent_n1,"Skin",11,1);
	EntFire(ent_n2,"Skin",11,1);
	EntFire(ent_n3,"Skin",11,1);
	EntFire(ent_n4,"Skin",10,1);
	EntFire(ent_n1,"Skin",11,1.1);
	EntFire(ent_n2,"Skin",11,1.1);
	EntFire(ent_n3,"Skin",10,1.1);
	EntFire(ent_n4,"Skin",4,1.1);
	EntFire(ent_keypad,"Skin",3,1.2);
	EntFire(ent_n1,"Skin",11,1.2);
	EntFire(ent_n2,"Skin",10,1.2);
	EntFire(ent_n3,"Skin",4,1.2);
	EntFire(ent_n4,"Skin",2,1.2);
	EntFire(ent_n1,"Skin",10,1.3);
	EntFire(ent_n2,"Skin",4,1.3);
	EntFire(ent_n3,"Skin",2,1.3);
	EntFire(ent_n4,"Skin",7,1.3);
	EntFire(ent_keypad,"Skin",2,1.4);
	EntFire(ent_n1,"Skin",10,1.4);
	EntFire(ent_n2,"Skin",4,1.4);
	EntFire(ent_n3,"Skin",2,1.4);
	EntFire(ent_n4,"Skin",7,1.4);
	EntFire(ent_n1,"Skin",4,1.5);
	EntFire(ent_n2,"Skin",2,1.5);
	EntFire(ent_n3,"Skin",7,1.5);
	EntFire(ent_n4,"Skin",10,1.5);
	EntFire(ent_keypad,"Skin",1,1.6);
	EntFire(ent_n1,"Skin",2,1.6);
	EntFire(ent_n2,"Skin",7,1.6);
	EntFire(ent_n3,"Skin",10,1.6);
	EntFire(ent_n4,"Skin",11,1.6);
	EntFire(ent_n1,"Skin",7,1.7);
	EntFire(ent_n2,"Skin",10,1.7);
	EntFire(ent_n3,"Skin",11,1.7);
	EntFire(ent_n4,"Skin",11,1.7);
	EntFire(ent_keypad,"Skin",4,1.8);
	EntFire(ent_n1,"Skin",10,1.8);
	EntFire(ent_n2,"Skin",11,1.8);
	EntFire(ent_n3,"Skin",11,1.8);
	EntFire(ent_n4,"Skin",11,1.8);
	EntFire(ent_n1,"Skin",11,1.9);
	EntFire(ent_n2,"Skin",11,1.9);
	EntFire(ent_n3,"Skin",11,1.9);
	EntFire(ent_n4,"Skin",11,1.9);
	EntFire(self.GetName(),"RunScriptCode","TurnOn();",2);
}

function BeginCompare()
{
	//Blinking sequence
	EntFire(ent_keypad,"Skin",2,0);
	EntFire(ent_n1,"Skin",10,0.2);
	EntFire(ent_n2,"Skin",10,0.2);
	EntFire(ent_n3,"Skin",10,0.2);
	EntFire(ent_n4,"Skin",10,0.2);
	EntFire(ent_n1,"Skin",""+var_num1+"",0.4);
	EntFire(ent_n2,"Skin",""+var_num2+"",0.4);
	EntFire(ent_n3,"Skin",""+var_num3+"",0.4);
	EntFire(ent_n4,"Skin",""+var_num4+"",0.4);
	EntFire(ent_n1,"Skin",10,0.6);
	EntFire(ent_n2,"Skin",10,0.6);
	EntFire(ent_n3,"Skin",10,0.6);
	EntFire(ent_n4,"Skin",10,0.6);
	EntFire(ent_n1,"Skin",""+var_num1+"",0.8);
	EntFire(ent_n2,"Skin",""+var_num2+"",0.8);
	EntFire(ent_n3,"Skin",""+var_num3+"",0.8);
	EntFire(ent_n4,"Skin",""+var_num4+"",0.8);
	//Finally, we can compare! *drumroll*
	EntFire(self.GetName(),"RunScriptCode","Compare();",1);
}

function Compare()
{
	//You did it! Good for you. You deserve a treat.
	if(var_num==var_finnum)
	{
		EntFire(ent_keypad,"Skin",4,0);
		EntFire(self.GetName(),"FireUser3","",0);
	}
	//You did i...wait, really? You didnt'? The narrator DID tell you the combination, right? 2845?
	else
	{
//		I left this so we can add an easter egg here easily. The script called for one, but I'm not sure if we're keeping it.
		if(var_num==1947)
			EntFire(self.GetName(),"FireUser4","",0);
		EntFire(ent_keypad,"Skin",3,0);
		EntFire(self.GetName(),"FireUser2","",0);
		EntFire(self.GetName(),"RunScriptCode","TurnOn();",1);
	}
}
//Sorry for getting all fussy up there. I love you Valve.