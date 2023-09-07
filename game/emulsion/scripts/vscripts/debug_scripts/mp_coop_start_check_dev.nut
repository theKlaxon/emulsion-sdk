function SpawnCheckDev()
{
	if ( GetDeveloperLevel() >= 1 )
	{
		printl( "Skipping wait for partner- starting map" )
		EntFire( "@relay_debug_start_both_connected", "Trigger", "", 2.0 )	
	}
}

