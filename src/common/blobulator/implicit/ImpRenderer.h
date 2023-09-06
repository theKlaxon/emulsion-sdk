#pragma once
#include "SweepRenderer.h"

static class ImpRendererFactory {
public:

	ImpRendererFactory() {}

	SweepRenderer*	getRenderer();
	void			returnRenderer(SweepRenderer* p1);
	
} factory;