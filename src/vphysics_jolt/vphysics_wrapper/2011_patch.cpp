#include "platform.h"
#include "dbg.h"

#pragma warning( disable : 4273 ) // don't worry, it still works
void _AssertValidStringPtr(const tchar* ptr, int maxchar/* = 0xFFFFFF */)
{
#if defined( _WIN32 ) && !defined( _X360 )
#ifdef TCHAR_IS_CHAR
	//Assert(!IsBadStringPtr(ptr, maxchar)); 
#else
	Assert(!IsBadStringPtrW(ptr, maxchar));
#endif
#else
	Assert(ptr);
#endif
}