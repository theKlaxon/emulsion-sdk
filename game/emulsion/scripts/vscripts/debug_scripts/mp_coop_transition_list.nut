DBG <- 0

m_nLocalBranch <- 0

HUB_MAP <- "mp_coop_lobby_2"

DAY_ONE_START_MAP <- "mp_coop_doors"
DAY_ONE_END_MAP <- "mp_coop_teambts"
DAY_ONE_LENGTH <- 1

DAY_TWO_START_MAP <- "mp_coop_fling_3"
DAY_TWO_END_MAP <- "mp_coop_fan"
DAY_TWO_LENGTH <- 1

DAY_THREE_START_MAP <- "mp_coop_wall_intro"
DAY_THREE_END_MAP <- "mp_coop_wall_5"
DAY_THREE_LENGTH <- 1

DAY_FOUR_START_MAP <- "mp_coop_tbeam_redirect"
DAY_FOUR_END_MAP <- "mp_coop_tbeam_end"
DAY_FOUR_LENGTH <- 1

DAY_FIVE_START_MAP <- "mp_coop_paint_come_along"
DAY_FIVE_END_MAP <- "mp_coop_paint_longjump_intro"
DAY_FIVE_LENGTH <- 1

DAY_SIX_START_MAP <- "mp_coop_credits"	// We need this so that the length of day five is counted properly
DAY_SIX_END_MAP <- "mp_coop_credits"
DAY_SIX_LENGTH <- 1

MapPlayOrder<- [
// this is the order to play the multiplayer maps

//"mp_coop_mic_calibration",

// INTRO
"mp_coop_start",	// small wave
"mp_coop_lobby_2",	
//HUB - high-5

// DAY 1 Start
//----TEAMBUILDING----
"mp_coop_doors",
"mp_coop_race_2",
"mp_coop_laser_2",
"mp_coop_rat_maze", //- RPS
"mp_coop_laser_crusher",
"mp_coop_teambts",
//HUB - laugh

// DAY 2 Start
//----FLINGING----
// "Mass and Velocity"
"mp_coop_fling_3",
"mp_coop_infinifling_train",
"mp_coop_come_along",
"mp_coop_fling_1", 
"mp_coop_catapult_1", //- robotDance
"mp_coop_multifling_1",
"mp_coop_fling_crushers",
"mp_coop_fan",
//HUB - teamtease

// DAY 3 Start
//----LIGHTBRIDGES----
// "Hard-Light Surfaces"
"mp_coop_wall_intro", 
"mp_coop_wall_2",
"mp_coop_catapult_wall_intro",
"mp_coop_wall_block",
"mp_coop_catapult_2",
"mp_coop_turret_walls",
"mp_coop_turret_ball",
"mp_coop_wall_5", //- teamhug

// DAY 4 Start
//----TBEAM----
// "Excursion Funnels"
"mp_coop_tbeam_redirect",
"mp_coop_tbeam_drill",
"mp_coop_tbeam_catch_grind_1", //- trickfire
"mp_coop_tbeam_laser_1",
"mp_coop_tbeam_polarity",
"mp_coop_tbeam_polarity2",
"mp_coop_tbeam_polarity3",
"mp_coop_tbeam_maze",
"mp_coop_tbeam_end",

// DAY 5 Start
//----PAINT----
// "Mobility Gels"
"mp_coop_paint_come_along",
"mp_coop_paint_redirect",
"mp_coop_paint_bridge",
"mp_coop_paint_walljumps",
"mp_coop_paint_speed_fling",
"mp_coop_paint_red_racer",
"mp_coop_paint_speed_catch",
"mp_coop_paint_longjump_intro",

// END OF LINE
//----NOTHING----
"mp_coop_credits"	// We need this so that the length of day five is counted properly
]

// --------------------------------------------------------
// OnPostSpawn
// --------------------------------------------------------
function OnPostSpawn()
{
	local nBranch = 0
	local daymapcount = 0
	local mapcount = 0
	
	if ( DBG )
	{
		printl("================DUMPING MAP PLAY ORDER")
	}
	
	AddBranchLevelName( 0, "CLEAR ALL" )
	
	foreach( index, map in MapPlayOrder )
	{	
		if( MapPlayOrder[index] == DAY_ONE_START_MAP )
		{
			nBranch++
			daymapcount = 0
			
			if ( DBG )
			{
				printl( "=== Start of Track 1" )
			}
		}
		else if( MapPlayOrder[index] == DAY_TWO_START_MAP )
		{
			nBranch++
			DAY_ONE_LENGTH = daymapcount
			daymapcount = 0
			
			if ( DBG )
			{
				printl( "< " + DAY_ONE_LENGTH + " levels in Track 1 >" )
				printl( "=== Start of Track 2" )
			}
		}
		else if( MapPlayOrder[index] == DAY_THREE_START_MAP )
		{
			nBranch++
			DAY_TWO_LENGTH = daymapcount
			daymapcount = 0
			
			if ( DBG )
			{
				printl( "< " + DAY_TWO_LENGTH + " levels in Track 2 >" )
				printl( "=== Start of Track 3" )
			}
		}
		else if( MapPlayOrder[index] == DAY_FOUR_START_MAP )
		{
			nBranch++
			DAY_THREE_LENGTH = daymapcount
			daymapcount = 0
			
			if ( DBG )
			{
				printl( "< " + DAY_THREE_LENGTH + " levels in Track 3 >" )
				printl( "=== Start of Track 4" )
			}
		}
		else if( MapPlayOrder[index] == DAY_FIVE_START_MAP )
		{
			nBranch++
			DAY_FOUR_LENGTH = daymapcount
			daymapcount = 0
			
			if ( DBG )
			{
				printl( "< " + DAY_FOUR_LENGTH + " levels in Track 4 >" )
				printl( "=== Start of Track 5" )
			}
		}
		else if( MapPlayOrder[index] == DAY_SIX_START_MAP )
		{
			nBranch++
			DAY_FIVE_LENGTH = daymapcount
			daymapcount = 0
			
			if ( DBG )
			{
				printl( "< " + DAY_FIVE_LENGTH + " levels in Track 5 >" )
				printl( "=== Start of Track 6" )
			}
		}
	
		if ( DBG )
		{
			if( GetMapName() == MapPlayOrder[index] )
			{
				printl( index + " " + MapPlayOrder[index] + " <--- You Are Here" )
			}
			else
			{
				printl( index + " " + MapPlayOrder[index] )
			}
		}
					
		AddBranchLevelName( nBranch, MapPlayOrder[index] )
		
		if( MapPlayOrder[index] == DAY_SIX_END_MAP )
		{
			nBranch++
			DAY_SIX_LENGTH = daymapcount+1
			daymapcount = 0
			
			if ( DBG )
			{
				printl( "< " + DAY_SIX_LENGTH + " levels in Track 6 >" )
				printl( "=== PLEASE DO NOT TEST ANY MAP BELOW THIS LINE ===" )
			}
		}
		
		mapcount++
		daymapcount++	
	}
	
	if ( DBG )
	{
		printl( mapcount + " maps total." )
	
		printl("================END DUMP")
	}
}

function MapPostLoaded()
{
}

function SetMapBranchAndLevel()
{
	local nBranch = 0
	local daymapcount = 1

	foreach( index, map in MapPlayOrder )
	{
		local bResetDayMapCount = false
		//printl( "= Branch: " + nBranch + ", Level: " + daymapcount )
		
		if( MapPlayOrder[index] == DAY_ONE_START_MAP )
		{
			nBranch++
			bResetDayMapCount = true
		}
		else if( MapPlayOrder[index] == DAY_TWO_START_MAP )
		{
			nBranch++
			bResetDayMapCount = true
		}
		else if( MapPlayOrder[index] == DAY_THREE_START_MAP )
		{
			nBranch++
			bResetDayMapCount = true
		}
		else if( MapPlayOrder[index] == DAY_FOUR_START_MAP )
		{
			nBranch++
			bResetDayMapCount = true
		}
		else if( MapPlayOrder[index] == DAY_FIVE_START_MAP )
		{
			nBranch++
			bResetDayMapCount = true
		}
		else if( MapPlayOrder[index] == DAY_SIX_START_MAP )
		{
			nBranch++
			bResetDayMapCount = true
		}
		
		// reset the daymap count
		if ( bResetDayMapCount == true )
		{
			daymapcount = 1
		}
		
		if( GetMapName() == MapPlayOrder[index] )
		{
			//printl( "################=== This is TRACK #" + nBranch + ", level " + daymapcount )
			EntFire( "@command", "command", "lobby_select_day " + nBranch, 0 )
			EntFire( "@command", "Command", "coop_lobby_select_level " + nBranch + " " + daymapcount + " 1", 0)	
			return
		}
				
		daymapcount++
	}
}

// --------------------------------------------------------
// TransitionFromMap
// TAUNTS - 	highFive and smallWave already unlocked at the start in code
//		robotDance and thumbsUp already unlocked in mp_coop_lobby.nut file
// --------------------------------------------------------
function TransitionFromMap()
{
	SaveMPStatsData()

	MarkMapComplete( GetMapName() )
		
	if( GetMapName() == HUB_MAP )
	{
		TransitionToSection()
	}
	else if( GetMapName() == DAY_ONE_END_MAP || 
			 GetMapName() == DAY_TWO_END_MAP || 
			 GetMapName() == DAY_THREE_END_MAP || 
			 GetMapName() == DAY_FOUR_END_MAP || 
			 GetMapName() == DAY_FIVE_END_MAP || 
			 GetMapName() == DAY_SIX_END_MAP )
	{
		EntFire( "@command", "command", "changelevel " + HUB_MAP, 1.0 )
	}
	else
	{
		local bInBranches = 0
		foreach( index, map in MapPlayOrder )
		{	
			if( GetMapName() == MapPlayOrder[index] )
			{				
				if( index + 1 >= MapPlayOrder.len() )
				{
					if(DBG) printl( "Map " + GetMapName() + " is the last map" )
					EntFire( "end_of_playtest_text", "display", 0 )
					EntFire( "@command", "command", "disconnect", 2.6 )
				}
				else
				{
					if(DBG) printl( "Map " + GetMapName() + " connects to " + MapPlayOrder[ index + 1 ] )
					EntFire( "@command", "command", "changelevel " + MapPlayOrder[index + 1], 1.3 )
				}
				return
			}
		}
	}
}

// --------------------------------------------------------
// TransitionToLevelFromHub
// --------------------------------------------------------
function TransitionToLevelFromHub( nBranch )
{
	SaveMPStatsData()

	EntFire( "@command", "command", "lobby_select_day " + nBranch, 0 )
	
	local nStartLevel = 0	
	foreach( index, map in MapPlayOrder )
	{
		if( GetMapName() == MapPlayOrder[index] )
		{
			break
		}
		nStartLevel++
	}
	
	if ( nBranch > 1 )
	{
		nStartLevel = nStartLevel + DAY_ONE_LENGTH
	}
	
	if ( nBranch > 2 )
	{
		nStartLevel = nStartLevel + DAY_TWO_LENGTH
	}

	if ( nBranch > 3 )
	{
		nStartLevel = nStartLevel + DAY_THREE_LENGTH
	}

	if ( nBranch > 4 )
	{
		nStartLevel = nStartLevel + DAY_FOUR_LENGTH
	}

	if ( nBranch > 5 )
	{
		nStartLevel = nStartLevel + DAY_FIVE_LENGTH
	}
	
	if ( nBranch > 6 )
	{
		printl( "=== Branch is out of range!!!!!!!!!" )
		return
	}

	local nCurrentLevel = GetCoopBranchLevelIndex( nBranch )
	local index = nCurrentLevel + nStartLevel
	if ( DBG )
	{
		printl( "=== Branch = " + nBranch + ", named " + MapPlayOrder[index] )
		printl( "=== Going to Level #" + index + ", named " + MapPlayOrder[index] )
	}
	EntFire( "@command", "command", "changelevel " + MapPlayOrder[index], 0.5 )
}


// --------------------------------------------------------
// ReturnToHubFromLevel
// --------------------------------------------------------
function ReturnToHubFromLevel()
{
	if ( DBG )
	{
		printl( "=== RETURNING TO HUB" )
	}

	SaveMPStatsData()

	EntFire( "@command", "command", "changelevel " + HUB_MAP, 0.2 )
}

function CheckDayLights()
{	
	local section = GetCoopSectionIndex()
	
	if( section == 1 )
	{
		EntFire( "light_day1", "TurnOn", "", 0.0 )
	}
	else
	{
		EntFire( "light_day1", "TurnOff", "", 0.0 )	
	}
	
	if( section == 2 ) 
	{
		EntFire( "light_day2", "TurnOn", "", 0.0 )
	}
	else
	{
		EntFire( "light_day2", "TurnOff", "", 0.0 )	
	}
	
	if( section == 3 )
	{
		EntFire( "light_day3", "TurnOn", "", 0.0 )
	}
	else
	{
		EntFire( "light_day3", "TurnOff", "", 0.0 )	
	}
	
	if( section != 0 )
	{
		EntFire( "@exit_door_1", "Open", "", 0.0 )
	}
}

function TransitionToSection()
{	
	local section = GetCoopSectionIndex()
	
	if ( DBG )
	{
		printl("Transitioning " + section )
	}
	
	if( section == 0 )
	{
		EntFire( "@command", "command", "changelevel mp_coop_lobby_2", 1.3 )
	}
	else if( section == 1 )
	{
		TransitionToDayOne()
	}
	else if( section == 2 )
	{
		TransitionToDayTwo()
	}
	else if( section == 3 )
	{
		TransitionToDayThree()
	}	
	else if( section == 4 )
	{
		TransitionToDayFour()
	}	
	else if( section == 5 )
	{
		TransitionToDayFive()
	}	
	else if( section == 6 )
	{
		TransitionToDaySix()
	}	
}


function TransitionToDayOne()
{
	EntFire( "@command", "command", "changelevel " + DAY_ONE_START_MAP, 0.1 )
}

function TransitionToDayTwo()
{
	EntFire( "@command", "command", "changelevel " + DAY_TWO_START_MAP, 0.1 )
}

function TransitionToDayThree()
{
	EntFire( "@command", "command", "changelevel " + DAY_THREE_START_MAP, 0.1 )
}

function TransitionToDayFour()
{
	EntFire( "@command", "command", "changelevel " + DAY_FOUR_START_MAP, 0.1 )
}

function TransitionToDayFive()
{
	EntFire( "@command", "command", "changelevel " + DAY_FIVE_START_MAP, 0.1 )
}

function TransitionToDaySix()
{
	EntFire( "@command", "command", "changelevel " + DAY_SIX_START_MAP, 0.1 )
}

////////////////////////
// level select buttons
////////////////////////

function SubtractLevelSelect( nBranch )
{
	local nCurrentLevel = GetCoopBranchLevelIndex( nBranch )
	
	local nNewLevel = nCurrentLevel - 1
	local bContinue = true
	
	while ( bContinue && nNewLevel > 1 && !IsLevelComplete( nBranch-1, nNewLevel-1 ) )
	{
		if ( nNewLevel > 1 && IsLevelComplete( nBranch-1, nNewLevel-2 ) )
		{
			bContinue = false
		}
		else
		{
			nNewLevel--
		}
	}
	
	if ( DBG )
	{
		printl( "----@=== Selecting Day " + nBranch + ", Level " + (nCurrentLevel - 1) )
	}

	if ( nNewLevel < 1 )
	{
		return
	}

	EntFire( "@command", "Command", "coop_lobby_select_level " + nBranch + " " + nNewLevel, 0)	
	
	local nMaxLevelsInDay = GetMaxLevelsInDay( nBranch )
	UpdateLevelSelectButtons( nBranch, nNewLevel, nMaxLevelsInDay )
}

function AddLevelSelect( nBranch )
{
	if ( nBranch < 1 )
	{
		return 
	}
	
	local nCurrentLevel = GetCoopBranchLevelIndex( nBranch )
	local nMaxLevelsInDay = GetMaxLevelsInDay( nBranch )
	
	//printl( "=== Selecting Day " + nBranch + ", Level " + (nCurrentLevel + 1) )
		
	//printl( "=== nMaxLevelsInDay = " + nMaxLevelsInDay )

	local nNewLevel = nCurrentLevel + 1
	local bContinue = true
	
	while ( bContinue && nNewLevel <= nMaxLevelsInDay && !IsLevelComplete( nBranch-1, nNewLevel-1 ) )
	{
		if ( nNewLevel > 1 && IsLevelComplete( nBranch-1, nNewLevel-2 ) )
		{
			bContinue = false
		}
		else
		{
			nNewLevel++
		}
	}
	
		
	if ( nNewLevel > nMaxLevelsInDay )
	{
		if ( DBG )
		{
			printl( "nNewLevel (" + nNewLevel + ") > nMaxLevelsInDay (" + nMaxLevelsInDay + ")" )
		}
		return 
	}
	
	UpdateLevelSelectButtons( nBranch, nNewLevel, nMaxLevelsInDay )
		
	EntFire( "@command", "Command", "coop_lobby_select_level " + nBranch + " " + nNewLevel, 0)
}

// --------------------------------------------------------
// Sets the local branch #
// --------------------------------------------------------
function SetLocalBranchNumber( nBranch )
{
	// this is to make sure that the client and server are on the same page
	// and sets the selected level to the last in the track or first uncompleted
	//printl( "^^^@=== SetLocalBranchNumber, setting to max in branch " + nBranch )
	EntFire( "@command", "Command", "coop_lobby_select_level " + nBranch + " " + 99, 0)	
	
	if ( DBG )
	{
		printl( "Setting local branch to = " + nBranch )
	}

	m_nLocalBranch = nBranch
	InitLevelSelectButtons( m_nLocalBranch )
}

function InitLevelSelectButtons( nBranch )
{
	if ( nBranch < 1 && m_nLocalBranch < 1 )
	{
		printl( "nBranch and m_nLocalBranch is < 1 !!!!!" )
		return 
	}
	
	if ( nBranch < 1 )
	{
		nBranch = m_nLocalBranch;
	}
	
	if ( DBG )
	{
		printl( "*********CALLING InitLevelSelectButtons( " + nBranch + " )" )
	}

	local nCurrentLevel = GetCoopBranchLevelIndex( nBranch )
	local nMaxLevelsInDay = GetMaxLevelsInDay( nBranch )
	
	UpdateLevelSelectButtons( nBranch, nCurrentLevel, nMaxLevelsInDay )
}

function UpdateLevelSelectButtons( nBranch, nCurrentLevel, nMaxLevelsInDay )
{
	if ( nCurrentLevel <= 1 )
	{
		// turn off the left light
		EntFire( EntityGroup[0].GetName(), "Skin", "1", 0)
	}
	else
	{
		// turn on the left light
		EntFire( EntityGroup[0].GetName(), "Skin", "0", 0)
	}
	
	//IsLevelComplete checks the data directly so we have to subtract one from the branch and level number
	local bCurrentLevelIsComplete = IsLevelComplete( nBranch-1, nCurrentLevel-1 )
	local bMoreLevelsComplete = false
	
	if ( !bCurrentLevelIsComplete )
	{
		for ( local j = nCurrentLevel + 1; j < nMaxLevelsInDay; j++ )
		{
			if ( IsLevelComplete( nBranch-1, j-1 ) )
			{
				bMoreLevelsComplete = true;
			}
		}
	}
	
	if ( DBG )
	{
		printl( "bCurrentLevelIsComplete = " + bCurrentLevelIsComplete + "-- nCurrentLevel = " + nCurrentLevel + "-- nMaxLevelsInDay = " + nMaxLevelsInDay )
	}

	if ( ( bCurrentLevelIsComplete == false && !bMoreLevelsComplete ) || nCurrentLevel >= nMaxLevelsInDay )
	{
		// turn off the right light
		EntFire( EntityGroup[1].GetName(), "Skin", "1", 0)
	}
	else
	{
		// turn on the right light
		EntFire( EntityGroup[1].GetName(), "Skin", "0", 0)
	}
	
}

function GetMaxLevelsInDay( nBranch )
{
	local nMaxLevelsInDay = DAY_ONE_LENGTH
		
	if ( nBranch == 2 )
	{
		nMaxLevelsInDay = DAY_TWO_LENGTH
	}
	else if ( nBranch == 3 )
	{
		nMaxLevelsInDay = DAY_THREE_LENGTH
	}
	else if ( nBranch == 4 )
	{
		nMaxLevelsInDay = DAY_FOUR_LENGTH
	}
	else if ( nBranch == 5 )
	{
		nMaxLevelsInDay = DAY_FIVE_LENGTH
	}
	else if ( nBranch == 6 )
	{
		nMaxLevelsInDay = DAY_SIX_LENGTH
	}
	
	return nMaxLevelsInDay
}