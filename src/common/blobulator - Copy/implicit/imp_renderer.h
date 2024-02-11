//==== Blobulator. Copyright © 2004-2007, Rosware LLC, All rights reserved. ====//
#pragma once
#include "sweep_renderer.h"

class ImpRendererFactory {
public:

	SweepRenderer* getRenderer();
	void returnRenderer(SweepRenderer* pRenderer);

private:

	ImpRendererFactory();

};
