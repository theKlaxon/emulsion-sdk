#pragma once
#include "sweep_renderer.h"

class ImpRendererFactory {
public:

	SweepRenderer* getRenderer();
	void returnRenderer(SweepRenderer* pRenderer);

private:

	ImpRendererFactory();

};
