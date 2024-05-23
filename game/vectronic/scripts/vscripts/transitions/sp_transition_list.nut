DBG <- 0

LAST_PLAYTEST_MAP <- "t1_ghost"

initialized <- false
loopsingleplayermaps <- false

// This is the order to play the maps
MapPlayOrder<- [
// We need new mapnames. Have it be like vcrnic_t1_myconcept? 
"t1_lasers",
"t1_throw",
"t1_blower",
"t1_goo",
"t1_ghost", // <- Steam Dev Days 2016 map
]

// --------------------------------------------------------
// EntFire_MapLoopHelper
// --------------------------------------------------------
function EntFire_MapLoopHelper( classname, suffix, command, param, delay )
{
	// This calls EntFire on an entity of a given type, named with the given suffix.
	// This deals with instance name mangling (though it doesn't guarantee uniqueness)
	local suffix_len = suffix.len()
	for ( local ent = Entities.FindByClassname( null, classname ); ent != null; ent = Entities.FindByClassname( ent, classname ) )
	{
		local ent_name = ent.GetName()
		local suffix_offset = ent_name.find( suffix )
		if ( ( suffix_offset != null ) && ( suffix_offset == ( ent_name.len() - suffix_len ) ) )
		{
			EntFire( ent_name, command, param, delay )
			return
		}
	}
	printl( "MAPLOOP: ---- ERROR! Failed to find entity " + suffix + " while initiating map transition" );
}

// --------------------------------------------------------
// Think
// --------------------------------------------------------
function Think()
{	
	// Start the game loop if the cvar is set
	if ( initialized && loopsingleplayermaps )
	{
		// initialize the timer
		if( LOOP_TIMER == 0 )
		{
			LOOP_TIMER = Time() + 10 // restart time in seconds
		}
		
		// transition to the next map if the timer has expired
		if ( LOOP_TIMER < Time() )
		{
			// reset loop timer
			LOOP_TIMER = 0

			printl( "\nMAPLOOP: timer expired, moving on..." )

			// Ensure point_viewcontrollers are disabled
			EntFire( "point_viewcontrol", "disable", 0, 0 )
		
			// Change the level (this sequence was originally in the 'transition_without_survey' logic_relay)
			EntFire_MapLoopHelper( "trigger_once",   "survey_trigger",    "Disable",       "",                    0.0 )
			EntFire_MapLoopHelper( "env_fade",       "exit_fade",         "Fade",          "",                    0.0 )
			EntFire_MapLoopHelper( "point_teleport", "exit_teleport",     "Teleport",      "",                    0.3 )
			EntFire_MapLoopHelper( "logic_script",   "transition_script", "RunScriptCode", "TransitionFromMap()", 0.4 )
		}
	}
	
	
	if (initialized)
	{
		return
	}
	initialized = true

	DumpMapList()

	local portalGunCommand = ""
	local portalGunSecondCommand = ""
	local foundMap = false
	
	foreach (index, map in MapPlayOrder)
	{
		if (GetMapName() == MapPlayOrder[index])
		{
			break
		}
	}
}

// --------------------------------------------------------
// TransitionFromMap
// --------------------------------------------------------
function DumpMapList()
{
	if(DBG)
	{
		local mapcount = 0
		
		printl("================DUMPING MAP PLAY ORDER")
		
		foreach( index, map in MapPlayOrder )
		{
			// weed out our transitions
			if( MapPlayOrder[index].find("@") == null )
			{
				if( GetMapName() == MapPlayOrder[index] )
				{
					printl( mapcount + " " + MapPlayOrder[index] + " <--- You Are Here" )
				}
				else
				{
					printl( mapcount + " " + MapPlayOrder[index] )
				}
				mapcount++
			}
			
		}
		printl( mapcount + " maps total." )
		
		printl("================END DUMP")
	}
}

// --------------------------------------------------------
// TransitionFromMap
// --------------------------------------------------------
function TransitionFromMap()
{
	local next_map = null
	foreach( index, map in MapPlayOrder )
	{
		if( GetMapName() == MapPlayOrder[index] )
		{
			// make good
			local skipIndex = index
			for(local i=0;i<2;i+=1)
			{
				if( skipIndex + 1 < MapPlayOrder.len() )
				{
					if( MapPlayOrder[skipIndex + 1].find("@") != null )
					{
						skipIndex++
					}
					else
					{
						break
					}
				}
			}		
			
			if( ( skipIndex + 1 < MapPlayOrder.len() ) &&
			    ( GetMapName() != LAST_PLAYTEST_MAP  )    )
			{
				next_map = MapPlayOrder[ skipIndex + 1 ]
				if(DBG) printl( "Map " + GetMapName() + " connects to " + next_map )

				if ( Entities.FindByName( null, "@changelevel" ) == null )
				{
					if(DBG) printl( "('@changelevel' entity missing, using 'map' command instead)" )
					SendToConsole( "map " + next_map );
				}
				else
				{
					EntFire( "@changelevel", "Changelevel", next_map, 0.0 )			
				}
			}
			else
			{
				printl( "Disconnecting..." )
				EntFire( "@command", "Command", "disconnect", 2.5 )			
			}
		}
	}
	
	if ( next_map == null )
	{
		if(DBG) printl( "Map " + GetMapName() + " is the last map" )
		EntFire( "end_of_playtest_text", "display", 0 )
		EntFire( "@end_of_playtest_text", "display", 0 )

		// If we are in the map loop and at the end of the list, start over at the beginning
		if ( loopsingleplayermaps )
		{
			printl( "MAPLOOP: No more maps, restarting loop." )
			next_map = MapPlayOrder[0]
			if ( Entities.FindByName( null, "@changelevel" ) == null )
			{
				SendToConsole( "map " + next_map );
			}
			else
			{
				EntFire( "@changelevel", "Changelevel", next_map, 0.0 )			
			}
		}
	}

	printl( "" )
}

// --------------------------------------------------------
// MakeBatFile - dumps the map list in a formatted way, for easy recompilin'
// --------------------------------------------------------
function MakeBatFile()
{
		local mapcount = 0
		
		printl("================DUMPING maps formatted for batch file")
		
		foreach( index, map in MapPlayOrder )
		{
			printl( "call build " + MapPlayOrder[index] )	
		}
		
		foreach( index, map in MapPlayOrder )
		{
			printl( "call buildcubemaps " + MapPlayOrder[index] )	
		}
}

// this lets the elevator know that we are ready to transition.
function TransitionReady()
{
	::TransitionReady <- 1	
}
