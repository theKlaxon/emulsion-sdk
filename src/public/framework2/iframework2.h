#pragma once
#include "appframework/IAppSystem.h"

class IFramework2 : public IBaseInterface {
public:

	virtual void Init(CreateInterfaceFn factory) = 0;
	virtual void Shutdown() = 0;

	virtual CreateInterfaceFn Facotries() = 0;

};