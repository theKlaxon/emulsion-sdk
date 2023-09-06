#pragma once
#include "game/shared/portal2/paint_enum.h"
#include "mathlib/mathlib.h"

struct PaintResult_t {
	PaintResult_t() {
		type = NO_POWER;
		traceNum = 0;
	}

	PaintResult_t(PaintPowerType _type, byte _traceNum) {
		type = _type;
		traceNum = _traceNum;
	}

	PaintPowerType type;
	byte traceNum;
};

struct PaintInfo_t {
public:
	PaintInfo_t() {
		type = INVALID_PAINT_POWER;
		plane = cplane_t();
		pos = Vector(0, 0, 0);
		m_pEnt = NULL;
		m_bIsPainted = false;
	}

	bool operator!=(PaintInfo_t other) {

		if (plane.normal != other.plane.normal ||
			type != other.type)
			return true;

		return false;
	}

	PaintPowerType	type;
	cplane_t		plane;
	Vector			pos;
	CBaseEntity*	m_pEnt;
	trace_t*		tr;
	short			m_bIsPainted;
};
