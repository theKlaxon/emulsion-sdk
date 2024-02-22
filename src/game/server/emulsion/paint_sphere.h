#pragma once
#include "baseentity.h"

class CPaintSphere : public CPointEntity {
	DECLARE_CLASS(CPaintSphere, CPointEntity)
	DECLARE_DATADESC()
public:

	CPaintSphere() {
		m_nPaintType = 0;
		m_flRad = 128.0f, m_flHardness = 1.0f;
	}

	void Spawn();
	void Paint();

	void InputPaint(inputdata_t& data) { Paint(); }

private:

	int m_nPaintType;
	float m_flRad, m_flHardness;

};
