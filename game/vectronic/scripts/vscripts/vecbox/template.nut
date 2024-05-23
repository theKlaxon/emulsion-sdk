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
	if (DBG) printl("==========================Effect 3!")
}

function ClearEffectBall3()
{
	// Do something.....
}

function EffectBall4()
{
	// Do something.....
	if (DBG) printl("==========================Effect 4!")
}

function ClearEffectBall4()
{
	// Do something.....
}

function EffectBall5()
{
	// Do something.....
	if (DBG) printl("==========================Effect 5!")
}

function ClearEffectBall5()
{
	// Do something.....
}

function OnReset()
{
	ClearEffectBall3()
	ClearEffectBall4()
	ClearEffectBall5()
}