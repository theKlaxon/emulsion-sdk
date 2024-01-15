// ======== ISOtope - standards ============================
// Purpose: Programming standards example file
// 
// theKlaxon
// =============================================

#include "cbase.h" // all cpp files must include cbase.h to compile
#include "props.h"
#include "ai_addon.h" // this header did not need to be included. dont include headers u dont need pls, it'll save compile times
#include <vector> // all includes not using "" should come last (with some exceptions for any headers that MUST be last)

// definitions should come right after the includes
#define AN_EXAMPLE_STRING_DEF "sauce engin"
#define AN_EXAMPLE_INTEGER_DEF 69 // nice

// any global variables should come right after the defnitions (static globals can come after normal ones aswell)
int g_nExampleGlobalInteger = 2;
static int g_nExampleGlobalStaticInteger = 420;

// Class Declarations should always go in their own matching header file.
// The header file should have the same ISOtope header and Purpose / author listed.
// I only included this class declaration in this cpp to keep the example all in one file.
class CExampleProp : public CPhysicsProp {
	DECLARE_CLASS(CExampleProp, CPhysicsProp) // any of the standard macros should be created above public
	DECLARE_SERVERCLASS()
public: // public should always come first in the class decl

	CExampleProp();

	// groups of functions should all keep the same indentation if possible (looks neater, easier to read large chunks)
	void Spawn();
	void Precache();

	// getters and setters should always come at the end of the public section,
	// and they should also come in pairs like they do here, with their definitions inside the class decl;
	// this is mainly to keep all of the getters and setters together and to keep clutter out of the cpp file.
	int		GetThePrivateExampleInt() { return m_nExampleIntWithGetterAndSetter; }
	void	SetThePrivateExampleInt(int nNewValue) { m_nExampleIntWithGetterAndSetter = nNewValue; }
								// ^^^^^^^^^^^^ params should follow the same naming conventions as 
								// normal member variables, but without the 'm_' at the beginning.

protected:	// the protected section should include any class / ent specific functions
			// to ensure that we can add friend classes if needed.
	 
	void FunctionSpecificToThisEntity();
	void AnotherOne();

	short m_snExampleShort;
	int m_nExampleInteger;
	float m_flExampleFloat;
	double m_dnExampleDouble;

	Vector m_vExampleVector;
	Quaternion m_qExampleQuat;

private:	// private should be used for variables that will never be needed by any other ents / classes,
			// or for any variables that have getters / setters

	int m_nExampleIntWithGetterAndSetter;
};

void CExampleProp::FunctionSpecificToThisEntity() { // pls keep opening brackets up top if u can.
													// not required ofc, but it looks nice.
	if (false)
		AnotherOne(); // avoid using brackets if you dont need them


}