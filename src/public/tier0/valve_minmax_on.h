//========= Copyright Valve Corporation, All rights reserved. ============//
#if !defined(POSIX)
#ifndef min
	#define min(a,b)  (((a) < (b)) ? (a) : (b))
	#define Min min
#endif
#ifndef max
	#define max(a,b)  (((a) > (b)) ? (a) : (b))
	#define Max max
#endif
#endif
