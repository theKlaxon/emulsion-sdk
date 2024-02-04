#include "cbase.h"
#include "emulsion_in_main.h"
#include "convar_serverbounded.h"
#include "c_emulsion_player.h"

extern ConVar m_yaw;
extern ConVar m_side;
extern ConVar m_forward;
extern ConVar lookstrafe;
extern ConVar cl_pitchup;
extern ConVar cl_pitchdown;
extern ConVar cam_idealyaw;
extern ConVar cam_idealpitch;
extern ConVar thirdperson_platformer;

extern ConVar_ServerBounded* m_pitch;

static CEmulsionInput g_Input;
IInput* input = (IInput*)&g_Input;

CEmulsionInput* GetPaintInput() {
	return &g_Input;
}

void CEmulsionInput::ApplyMouse(int nSlot, QAngle& viewangles, CUserCmd* cmd, float mouse_x, float mouse_y) {

	PerUserInput_t& user = GetPerUser(nSlot);

	//roll the view angles so roll is 0 (the HL2 assumed state) and mouse adjustments are relative to the screen.
	//Assuming roll is unchanging, we want mouse left to translate to screen left at all times (same for right, up, and down)

	if (!((in_strafe.GetPerUser(nSlot).state & 1) || lookstrafe.GetInt()))
	{
		if (CAM_IsThirdPerson() && thirdperson_platformer.GetInt())
		{
			if (mouse_x)
			{
				// use the mouse to orbit the camera around the player, and update the idealAngle
				user.m_vecCameraOffset[YAW] -= m_yaw.GetFloat() * mouse_x;
				cam_idealyaw.SetValue(user.m_vecCameraOffset[YAW] - viewangles[YAW]);
			}
		}
		else
		{
			// Otherwize, use mouse to spin around vertical axis
			if (m_bFixStickMouse)
				mouse_x = -mouse_x;

			viewangles[YAW] -= m_yaw.GetFloat() * mouse_x;
		}
	}
	else
	{
		// If holding strafe key or mlooking and have lookstrafe set to true, then apply
		//  horizontal mouse movement to sidemove.
		if (m_bFixStickMouse)
			mouse_x = -mouse_x;

		cmd->sidemove += m_side.GetFloat() * mouse_x;
	}

	// If mouselooking and not holding strafe key, then use vertical mouse
	//  to adjust view pitch.
	if (!(in_strafe.GetPerUser(nSlot).state & 1))
	{
		if (CAM_IsThirdPerson() && thirdperson_platformer.GetInt())
		{
			if (mouse_y)
			{
				// use the mouse to orbit the camera around the player, and update the idealAngle
				user.m_vecCameraOffset[PITCH] += m_pitch->GetFloat() * mouse_y;
				cam_idealpitch.SetValue(user.m_vecCameraOffset[PITCH] - viewangles[PITCH]);
			}
		}
		else
		{
			viewangles[PITCH] += m_pitch->GetFloat() * mouse_y;

			// Check pitch bounds
			if (viewangles[PITCH] > cl_pitchdown.GetFloat())
			{
				viewangles[PITCH] = cl_pitchdown.GetFloat();
			}
			if (viewangles[PITCH] < -cl_pitchup.GetFloat())
			{
				viewangles[PITCH] = -cl_pitchup.GetFloat();
			}
		}
	}
	else
	{
		// Default is to apply vertical mouse movement as a forward key press.
		cmd->forwardmove -= m_forward.GetFloat() * mouse_y;
	}

	// Finally, add mouse state to usercmd.
	// NOTE:  Does rounding to int cause any issues?  ywb 1/17/04
	cmd->mousedx = (int)mouse_x;
	cmd->mousedy = (int)mouse_y;

}

void CEmulsionInput::SetUseStickMouseFix(bool bUseFix) {
	m_bFixStickMouse = bUseFix;
}