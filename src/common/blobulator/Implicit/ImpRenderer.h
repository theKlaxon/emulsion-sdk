//==== Blobulator. Copyright © 2004-2024, Ilya D. Rosenberg, All rights reserved. ====//
#pragma once
#include "SweepRenderer.h"

class ImpRendererFactory {
public:

	SweepRenderer* getRenderer();
	void returnRenderer(SweepRenderer* pRenderer);

private:

	ImpRendererFactory();

};
