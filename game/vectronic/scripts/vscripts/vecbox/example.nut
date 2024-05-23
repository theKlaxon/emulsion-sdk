// This is a script that you can use to make your own box effects.

// enable DBG to display debug spew 
DBG 		<- 0

self.ConnectOutput( "OnBall3", "EffectBall3" )
self.ConnectOutput( "OnBall4", "EffectBall4" )
self.ConnectOutput( "OnBall5", "EffectBall5" )
self.ConnectOutput( "OnReset", "OnReset" )

function EffectBall3()
{
	// Do something.....
	printl("==========================Effect 3!")
	self.EmitSound( "Vectronic.SwitchLocked" )
	
	// Stop any other balls from touching this!
	EntFire(self.GetName(),"DisableBallTouch","",0.0)
	
	// Explode!
	EntFire(self.GetName(),"Explode","",1.0)
}

function ClearEffectBall3()
{
	// Do something.....
	// We do nothing cause we are dead.
}

function EffectBall4()
{
	// Do something.....
	printl("==========================Effect 4!")
	
	// Spawn a ghost like Ball1
	EntFire(self.GetName(),"MakeGhost","",0.0)
}

function ClearEffectBall4()
{
	// Do something.....
	
	// Kill Our Ghost.
	EntFire(self.GetName(),"KillGhost","",0.0)
}

function EffectBall5()
{
	// Do something.....
	printl("==========================Effect 5!")	
	EntFire(self.GetName(),"EnableGravity","0",0.0)
}

function ClearEffectBall5()
{
	// Do something.....
	EntFire(self.GetName(),"EnableGravity","1",0.0)
}

function OnReset()
{
	ClearEffectBall3()
	ClearEffectBall4()
	ClearEffectBall5()
}