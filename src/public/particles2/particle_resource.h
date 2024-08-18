#pragma once
#include "filesystem.h"

class CParticleResource {
public:

	void ParseParticles(IFileSystem* pFilesystem);

	const CParticleDefinition* GetParticleDef(const char* pszName);

protected:

};