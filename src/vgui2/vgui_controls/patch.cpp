#include "KeyValues.h"

//-----------------------------------------------------------------------------
// Returns whether a keyvalues conditional evaluates to true or false
// Needs more flexibility with conditionals, checking convars would be nice.
//-----------------------------------------------------------------------------
bool EvaluateConditional(const char* str)
{
	if (!str)
		return false;

	if (*str == '[')
		str++;

	bool bNot = false; // should we negate this command?
	if (*str == '!')
		bNot = true;

	if (Q_stristr(str, "$X360"))
		return IsX360() ^ bNot;

	if (Q_stristr(str, "$WIN32"))
		return IsPC() ^ bNot; // hack hack - for now WIN32 really means IsPC

	//if (Q_stristr(str, "$WINDOWS"))
	//	return IsWindows() ^ bNot;

	if (Q_stristr(str, "$OSX"))
		return IsOSX() ^ bNot;

	if (Q_stristr(str, "$LINUX"))
		return IsLinux() ^ bNot;

	if (Q_stristr(str, "$POSIX"))
		return IsPosix() ^ bNot;

	return false;
}