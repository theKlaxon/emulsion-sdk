#pragma once
#include "c_sharploader.h"
#include "cdll_int.h"
#include "materialsystem/imaterialsystem.h"
#include "globalvars_base.h"
#include "puzzlemaker/puzzlemaker.h"

class IPuzzleMaker {
public:

	virtual float TestTest(int a, float b) = 0;
};

class CPuzzleMaker {
public:

	CPuzzleMaker();
	~CPuzzleMaker();

	SLIB_MOCK(Testing, &IPuzzleMaker::TestTest);

private:

};