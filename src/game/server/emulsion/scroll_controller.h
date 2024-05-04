#pragma once

namespace scon {
	typedef void (*scrollfunc)();
	static scrollfunc m_fnScrollUp = nullptr;
	static scrollfunc m_fnScrollDn = nullptr;
}

class CScrollController {
public:

	CScrollController() {
		ClearBinds();
	}

	void SetUp(scon::scrollfunc fn) { scon::m_fnScrollUp = fn; }
	void SetDn(scon::scrollfunc fn) { scon::m_fnScrollDn = fn; }

	void ClearBinds() {
		scon::m_fnScrollUp = nullptr;
		scon::m_fnScrollDn = nullptr;
	}

	static void CallUp() { if (scon::m_fnScrollUp != nullptr) scon::m_fnScrollUp(); }
	static void CallDn() { if (scon::m_fnScrollDn != nullptr) scon::m_fnScrollDn(); }

};

static CScrollController g_sScroller;
static ConCommand scfn_up("scfn_up", g_sScroller.CallUp);
static ConCommand scfn_dn("scfn_dn", g_sScroller.CallDn);