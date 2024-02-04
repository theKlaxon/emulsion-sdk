#pragma once
#include "input.h"
#include "game\shared\portal2\paint_enum.h"

class CEmulsionInput : public CInput {
public:

	CEmulsionInput() {
		m_bFixStickMouse = false;
	}

	virtual void ApplyMouse(int nSlot, QAngle& viewangles, CUserCmd* cmd, float mouse_x, float mouse_y);
	
	void SetUseStickMouseFix(bool bUseFix);

private:

	bool m_bFixStickMouse;

};

extern CEmulsionInput* GetPaintInput();