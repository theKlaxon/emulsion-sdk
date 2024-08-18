//========= Copyright © 1996-2007, Valve Corporation, All rights reserved. ============//
//
//	LZMA Decoder. Designed for run time decoding.
//
//	LZMA SDK 4.43 Copyright (c) 1999-2006 Igor Pavlov (2006-05-01)
//	http://www.7-zip.org/
//
//=====================================================================================//

#ifndef _LZMADECODER_H
#define _LZMADECODER_H
#pragma once

#if !defined( _X360 )
#define LZMA_ID				(('A'<<24)|('M'<<16)|('Z'<<8)|('L'))
#else
#define LZMA_ID				(('L'<<24)|('Z'<<16)|('M'<<8)|('A'))
#endif

// bind the buffer for correct identification
#pragma pack(1)
struct lzma_header_t
{
	unsigned int	id;
	unsigned int	actualSize;		// always little endian
	unsigned int	lzmaSize;		// always little endian
	unsigned char	properties[5];
};

/*

for use with imhex

struct lzma_header_t
{
	u32	id;
	u32	actualSize;		// always little endian
	u32	lzmaSize;		// always little endian
	u8	properties[5];
};

lzma_header_t file_header @ 0x00 // for normally compressed lzma files
lzma_header_t file_header @ 0x1B // for .VCS files ONLY!!!!!			<-----------------------

*/

#pragma pack()

typedef void ( *LZMAReadProgressCallbackFunc_t )();

class CLZMA
{
public:
	unsigned int	Uncompress( unsigned char *pInput, unsigned char *pOutput, LZMAReadProgressCallbackFunc_t pCallback = NULL );
	unsigned int	UncompressVCS( unsigned char *pInput, unsigned char *pOutput, LZMAReadProgressCallbackFunc_t pCallback = NULL );
	bool			IsCompressed( unsigned char *pInput );
	unsigned int	GetActualSize( unsigned char *pInput );

private:
};

#endif

