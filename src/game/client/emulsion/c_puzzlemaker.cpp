#include "cbase.h"
#include "c_puzzlemaker.h"
#include <utility>
#include <stdint.h>
#include <tuple>

SLIB_DEFINE("puzzlemaker.dll");

CPuzzleMaker::CPuzzleMaker() {
	SLIB_LOAD();
	SLIB_FLOAD(Testing, "TestTest");

	//typedef FArgs<decltype(&IPuzzleMaker::TestTest)>::rtype(__cdecl* P_TestName)(FArgs<decltype(&IPuzzleMaker::TestTest)>::args ...);
	using fn = FArgs<decltype(&IPuzzleMaker::TestTest)>;
	typedef fn::rtype(__cdecl* P_TestName)(typename fn::args...);

	P_TestName callname;

	callname(0, 420.0f);

	float t = Testing(69, 420.0f);
	Msg("\nPuzzleMaker said: %f\n");
}

CPuzzleMaker::~CPuzzleMaker() {
	
}