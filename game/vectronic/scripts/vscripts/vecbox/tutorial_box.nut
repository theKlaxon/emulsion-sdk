// This is a script is for the very first box the player sees!
// We call the map hint entity.

// enable DBG to display debug spew 
DBG 		<- 0

self.ConnectOutput( "OnPlayerPickup", "PlayerPickup" )
self.ConnectOutput( "OnPhysGunDrop", "PhysGunDrop" )

function PlayerPickup()
{
	EntFire( "hint_pickup_rm0", "EndHint","",0.00, 1)
	EntFire( "hint_drop_rm0", "ShowHint","",0.35)
}

function PhysGunDrop()
{
	EntFire( "hint_drop_rm0", "EndHint","",0.00)
}