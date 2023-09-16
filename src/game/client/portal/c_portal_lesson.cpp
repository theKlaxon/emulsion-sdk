#include "cbase.h"
#include "c_gameinstructor.h"
#include "c_baselesson.h"

bool C_GameInstructor::Mod_HiddenByOtherElements( void )
{
	return false;
}

void CScriptedIconLesson::Mod_PreReadLessonsFromFile( void )
{
	// Add custom actions to the map
	//example: CScriptedIconLesson::LessonActionMap.Insert( "is singleplayer", LESSON_ACTION_IS_SINGLEPLAYER );
}

bool CScriptedIconLesson::Mod_ProcessElementAction( int iAction, bool bNot, const char *pchVarName, EHANDLE &hVar, 
													const CGameInstructorSymbol *pchParamName, float fParam,
													C_BaseEntity *pParam, const char *pchParam, bool &bModHandled )
{
	bModHandled = false;
	return false;
}