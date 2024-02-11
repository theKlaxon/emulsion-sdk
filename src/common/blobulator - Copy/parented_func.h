#pragma once

class IParentedFuncPtr {
public:
	virtual void Do(void* pData) = 0;
	void SetParent(void* pParent) { m_pParent = pParent; }

protected:
	void* m_pParent;
};

class COpPtr_DoNothing : public IParentedFuncPtr {
public:
	virtual void Do(void* pData) {} // do nothing :/
} extern g_BlobDoNothing;