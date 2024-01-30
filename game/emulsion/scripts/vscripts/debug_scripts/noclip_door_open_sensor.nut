//=============================================================================
// this script will open prop_portal_linked_door entities if the player is 
// within OPEN_DIST of the door and also in noclip mode
// script will also draw debug lines between linked doors while in noclip mode
//=============================================================================

DBG <- 0
// parallel arrays to contain list of doors and targets
portal_door_array <- []
portal_door_linked_target_array <- []

// number of doors in map
number_of_doors <- 0

// opening distance
OPEN_DIST <- 80


//---------------------------------------------------------
// Collect Doors in map into arrays and remove reciprocal entries
//---------------------------------------------------------
function CollectDoors()
{
	// iterators
	local cur_ent = null
	
	// clear the arrays
	portal_door_array.clear()
	portal_door_linked_target_array.clear()

	// reset door count
	number_of_doors = 0
	
	// collect doors into array
	do
	{
		local reciprocal = false
		cur_ent = Entities.FindByClassname( cur_ent, "prop_linked_portal_door" )	
		if ( cur_ent != null  )
		{
			foreach( index, door in portal_door_linked_target_array )
			{
				if( portal_door_linked_target_array[index] )
				{
					if( cur_ent.GetName() == portal_door_linked_target_array[index].GetName() )
					{
						if( portal_door_linked_target_array[index].GetPartnerInstance().GetName() == portal_door_array[index].GetName() )
						{
							reciprocal = true
						}
					}
				}
			}

			if( !reciprocal)
			{
				portal_door_array.append( cur_ent )
				portal_door_linked_target_array.append( portal_door_array[number_of_doors].GetPartnerInstance() )			
				number_of_doors++
			}
		}
	} while ( cur_ent != null )
	
	// dump data to the console if debugging is enabled
	if(DBG) debugDumpDoorArrayData()
}

function debugDumpDoorArrayData()
{
	printl("--- doors = " + number_of_doors )
	
	foreach( index, door in portal_door_linked_target_array )
	{
		if( portal_door_linked_target_array[index] )
		{
			printl( index + ") --- " + portal_door_array[index].GetName() + "<->" + portal_door_linked_target_array[index].GetName() )
		}
	}
}

function DrawLinkedPartnerLines()
{		
	// draw line between doors
	for(local i=0; i<portal_door_linked_target_array.len(); i++)
	{
		if( portal_door_linked_target_array[i] )
		{
			DebugDrawLine( portal_door_array[i].GetOrigin(), portal_door_linked_target_array[i].GetOrigin(), 100, 100, 100, true, 0.2 )
		}
	}
}

function Think()
{	
	// open nearby doors
	if( IsMultiplayer() == false && player.IsNoclipping() )
	{
		// draw links between doors and partners
		CollectDoors()
		if (DBG) DrawLinkedPartnerLines()
		
		local cur_ent = null
		local linked_partner_name = null
		do 
		{
			cur_ent = Entities.FindByClassnameWithin( cur_ent, "prop_linked_portal_door", player.GetOrigin(), OPEN_DIST )
			if( cur_ent )
			{
				EntFire( cur_ent.GetName(), "open" )
			}
		} while ( cur_ent != null )
	}
}