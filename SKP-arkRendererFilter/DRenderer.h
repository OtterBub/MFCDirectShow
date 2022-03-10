#pragma once

#include <streams.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <atlbase.h>

template <typename T>
inline void SafeRelease(T& p)
{
	if (NULL != p)
	{
		p.Release();
		p = NULL;
	}
}

#define HR(x) if(FAILED(x)) { return x; }

class CDRenderer
{
public:
	CDRenderer(void);
	virtual ~CDRenderer(void);

protected:
	CComPtr<ID2D1Factory> m_d2dFactory;
};