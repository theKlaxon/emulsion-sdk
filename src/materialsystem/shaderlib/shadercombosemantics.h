//==== Copyright (c) Valve Corporation, All rights reserved. =======//
//
// Vertex/Pixel Shaders
//
//===========================================================================//

// yoinked from csgo to make deferred work.
// p.s. i dont like yoinking from csgo but im not gonna 
// spend hours re'ing some small structs when they're 
// literally right there. -Klax

#ifndef SHADERCOMBOSEMANTICS_H
#define SHADERCOMBOSEMANTICS_H

#ifdef _WIN32
#pragma once
#endif

struct ShaderComboInformation_t
{
	const char *m_pComboName;
	int m_nComboMin;
	int m_nComboMax;
};

struct ShaderComboSemantics_t
{
	const char *pShaderName;
	const ShaderComboInformation_t *pDynamicShaderComboArray;
	int nDynamicShaderComboArrayCount;
	const ShaderComboInformation_t *pStaticShaderComboArray;
	int nStaticShaderComboArrayCount;
};

#endif // SHADERCOMBOSEMANTICS_H
