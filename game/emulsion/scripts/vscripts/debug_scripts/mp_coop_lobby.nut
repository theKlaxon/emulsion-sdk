TauntList<- [
//0 
"smallWave",
"highFive",
"not_used",
"rps",
//4 
"laugh",
"robotDance",
//6 
"teamtease",
"not_used",
"teamhug",
"trickfire",
]


function OpenHUBAreas()
{
	local numHighestActiveBranch = GetHighestActiveBranch();
	local numReturnToHub = GetGladosSpokenFlags(2);

	EntFire( "counter_screen_flip", "SetMaxValueNoFire", numHighestActiveBranch, 0 )

	if ( numHighestActiveBranch == 1 )
	{
		if ( numReturnToHub & (1 << 0) )
		{
			//quick way
			EntFire( "counter_return_to_hub", "SetValue", "1", 0 )
		}
		else
		{
			//slow way
			EntFire( "counter_choose_course", "SetValue", "1", 0 )
			// remember we saw it
			AddGladosSpokenFlags( 2, 1 << 0 )
		}

		EntFire( "counter_music", "SetValue", "1", 0 )
	}
	else if ( numHighestActiveBranch == 2 )
	{
		if ( numReturnToHub & (1 << 1) )
		{
			//quick way
			EntFire( "counter_return_to_hub", "SetValue", "2", 0 )
		}
		else
		{
			//slow way
			EntFire( "counter_choose_course", "SetValue", "2", 0 )
			// remember we saw it
			AddGladosSpokenFlags( 2, 1 << 1 )
		}

		EntFire( "texture_course_01", "SetTextureIndex", "1", 3 )
		EntFire( "counter_music", "SetValue", "2", 0 )
	}
	else if ( numHighestActiveBranch == 3 )
	{
		if ( numReturnToHub & (1 << 2) )
		{
			//quick way
			EntFire( "counter_return_to_hub", "SetValue", "3", 0 )
		}
		else
		{
			//slow way
			EntFire( "counter_choose_course", "SetValue", "3", 0 )
			// remember we saw it
			AddGladosSpokenFlags( 2, 1 << 2 )
		}

		EntFire( "texture_course_01", "SetTextureIndex", "2", 3 )
		EntFire( "texture_course_02", "SetTextureIndex", "1", 4 )
		EntFire( "counter_music", "SetValue", "3", 0 )
	}
	else if ( numHighestActiveBranch == 4 )
	{
		if ( numReturnToHub & (1 << 3) )
		{
			//quick way
			EntFire( "counter_return_to_hub", "SetValue", "4", 0 )
		}
		else
		{
			//slow way
			EntFire( "counter_choose_course", "SetValue", "4", 0 )
			// remember we saw it
			AddGladosSpokenFlags( 2, 1 << 3 )
		}

		EntFire( "texture_course_01", "SetTextureIndex", "2", 3 )
		EntFire( "texture_course_02", "SetTextureIndex", "2", 4 )
		EntFire( "texture_course_03", "SetTextureIndex", "1", 5 )
		EntFire( "counter_music", "SetValue", "4", 0 )
	}
	else if ( numHighestActiveBranch == 5 )
	{
		if ( numReturnToHub & (1 << 4) )
		{
			//quick way
			EntFire( "counter_return_to_hub", "SetValue", "5", 0 )
		}
		else
		{
			//slow way
			EntFire( "counter_choose_course", "SetValue", "5", 0 )
			// remember we saw it
			AddGladosSpokenFlags( 2, 1 << 4 )
		}

		EntFire( "texture_course_01", "SetTextureIndex", "2", 3 )
		EntFire( "texture_course_02", "SetTextureIndex", "2", 4 )
		EntFire( "texture_course_03", "SetTextureIndex", "2", 5 )
		EntFire( "texture_course_04", "SetTextureIndex", "1", 6 )
		EntFire( "counter_music", "SetValue", "5", 0 )
	}
	else if ( numHighestActiveBranch >= 6 )
	{
		EntFire( "counter_return_to_hub", "SetValue", "6", 0 )
		EntFire( "texture_course_01", "SetTextureIndex", "2", 3 )
		EntFire( "texture_course_02", "SetTextureIndex", "2", 4 )
		EntFire( "texture_course_03", "SetTextureIndex", "2", 5 )
		EntFire( "texture_course_04", "SetTextureIndex", "2", 6 )

		if ( numHighestActiveBranch == 6 )
		{
			EntFire( "counter_music", "SetValue", "6", 0 )
		}
		else
		{
			EntFire( "counter_music", "SetValue", "7", 0 )
		}
	}
	
	// now grant taunts to players if they are playing with a partner who has unocked more courses than they have
	// they would have gotten the taunt when the course was unlocked for the first time, 
	// but since it's skipped when playing with a more experienced partner, we need to give them silently here
	// check for the teamtease first
	if ( numHighestActiveBranch >= 3 )
	{
		// check if mp_coop_fan is NOT complete in course 2 by either player
		if ( IsPlayerLevelComplete( 0, 1, 7 ) == false || IsPlayerLevelComplete( 1, 1, 7 )  == false )
		{
			EntFire( "@command", "command", "mp_earn_taunt teamtease 1", 1.0 )
			//printl("!!=== SCRIPT trying to grant teamtease silently" )
		}
	}
	// now check the same thing for the laugh taunt
	if ( numHighestActiveBranch >= 2 )
	{
		// check if mp_coop_teambts is NOT complete in course 1
		if ( IsPlayerLevelComplete( 0, 0, 5 ) == false || IsPlayerLevelComplete( 1, 0, 5 ) == false )
		{
			EntFire( "@command", "command", "mp_earn_taunt laugh 1", 1.0 )
			//printl("!!=== SCRIPT trying to grant laugh silently" )
		}
	}

	EntFire( "counter_music", "GetValue", "", 1 )
	
	if ( numHighestActiveBranch >= 2 )
	{
		EntFire( "hint_zoom", "Enable", "", 0 )
	}

	// give each player an achievement if they finished the prior branch
	//TEAM_BUILDING
	if ( IsPlayerBranchComplete( 0, 0 ) )
	{
		RecordAchievementEvent( "ACH.TEAM_BUILDING", GetBluePlayerIndex() )
	}
	if ( IsPlayerBranchComplete( 1, 0 ) )
	{
		RecordAchievementEvent( "ACH.TEAM_BUILDING", GetOrangePlayerIndex() )	
	}
	//MASS_AND_VELOCITY
	if ( IsPlayerBranchComplete( 0, 1 ) )
	{
		RecordAchievementEvent( "ACH.MASS_AND_VELOCITY", GetBluePlayerIndex() )
	}
	if ( IsPlayerBranchComplete( 1, 1 ) )
	{
		RecordAchievementEvent( "ACH.MASS_AND_VELOCITY", GetOrangePlayerIndex() )	
	}
	//LIGHT BRIDGES
	if ( IsPlayerBranchComplete( 0, 2 ) )
	{
		RecordAchievementEvent( "ACH.HUG_NAME", GetBluePlayerIndex() )
	}
	if ( IsPlayerBranchComplete( 1, 2 ) )
	{
		RecordAchievementEvent( "ACH.HUG_NAME", GetOrangePlayerIndex() )	
	}
	//EXCURSION_FUNNELS
	if ( IsPlayerBranchComplete( 0, 3 ) )
	{
		RecordAchievementEvent( "ACH.EXCURSION_FUNNELS", GetBluePlayerIndex() )
	}
	if ( IsPlayerBranchComplete( 1, 3 ) )
	{
		RecordAchievementEvent( "ACH.EXCURSION_FUNNELS", GetOrangePlayerIndex() )	
	}
	
	for ( local i = 0; i < 5; i++ )
	{	
		for ( local j = 0; j < 16; j++ )
		{
			if ( IsLevelComplete( i, j ) )
			{
				EntFire( "texture_level_complete" + i + "" + j, "SetTextureIndex", "1", 0 )
			}
		}
		
		local branch = i + 1
		
		if ( IsBranchComplete( i ) )
		{
			EntFire( "track" + branch + "-texture_toggle_door", "SetTextureIndex", "1", i + 3 )
		}
	}

	GrantGameCompleteAchievementHUB()
}

function CompleteFinalMap()
{
	MarkMapComplete( GetMapName() )
}

function GrantGameCompleteAchievementHUB()
{
	local bGameCompleteB = true
	local bGameCompleteO = true
	for ( local i = 0; i < 5; i++ )
	{	
		if ( IsPlayerBranchComplete( 0, i ) == false )
		{
			bGameCompleteB = false
			//printl("!!=== bGameCompleteB = false --- not completed branch " + i )
		}
		if ( IsPlayerBranchComplete( 1, i ) == false )
		{
			bGameCompleteO = false
		}
	}
	
	if ( bGameCompleteB )
	{
		RecordAchievementEvent( "ACH.NEW_BLOOD", GetBluePlayerIndex() )	
		RecordAchievementEvent( "AV_SHIRT1", GetBluePlayerIndex() )	
		//printl("!!!!!!!!=== AWARDING END GAME ACHIEVEMENT!!!" )
	}
	if ( bGameCompleteO )
	{
		RecordAchievementEvent( "ACH.NEW_BLOOD", GetOrangePlayerIndex() )	
		RecordAchievementEvent( "AV_SHIRT1", GetOrangePlayerIndex() )	
	}
}

function GrantGameCompleteAchievement()
{
	// this was moved from the last map because the toast caused the bink movie's sound to echo on the 360!
	// it now lives in credits_coop.nut in CreditsGrantGameCompleteAchievement
}

function EarnTaunt( nTaunt )
{
	local TauntName = TauntList[nTaunt];
	if ( TauntName == "not_used" )
		return;
	
	local bGrantTauntB = true
	local bGrantTauntO = true
		
	if ( TauntName == "laugh" || TauntName == "teamtease" )
	{
		local nCourse = 0
		
		if ( TauntName == "laugh" )
		{
			nCourse = 1
		}
		else if ( TauntName == "teamtease" )
		{
			nCourse = 2
		}
	
		for ( local j = 0; j < 16; j++ )
		{
			if ( IsPlayerLevelComplete( 0, nCourse, j ) )
			{
				bGrantTauntB = false
				//printl("bGrantTauntB = false")
			}
			if ( IsPlayerLevelComplete( 1, nCourse, j ) )
			{
				bGrantTauntO = false
				//printl("bGrantTauntB = false")
			}
		}
	}
	
	if ( bGrantTauntB == false && bGrantTauntO == false )
	{
		//printl("Not granting " + TauntName + " because both players already have it")
		return
	}
	
	//printl("Earning " + TauntName)
	EntFire( "@command", "command", "mp_earn_taunt " + TauntName, 0.0 )
}

function TransitionToCredits()
{
	if ( IsLocalSplitScreen() )
	{
		EntFire( "@command", "Command", "changelevel mp_coop_credits", 0.1 )
	}
	else
	{
		EntFire( "@command", "Command", "changelevel mp_coop_credits", 0.0 )
	}
}

function CheckForNewTaunts()
{
}
