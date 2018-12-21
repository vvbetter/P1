#pragma once
#define CACHELINE32(x)	char ve_cacheline_##x [32];
#define CACHELINE(x)	char ve_cacheline_##x [64];

template<typename T>
class SafeArray
{
public:
	SafeArray(UINT nSize)
	{
		UINT k = nSize & (nSize - 1);
		if (k != 0)
		{
			k = 0;
			while (nSize)
			{
				nSize >>= 1;
				++k;
			}
			nSize = 1 << k;
		}
		nSize = max(nSize, 8);
		m_ArraySize = nSize;
		m_ArraySizeInv = nSize - 1;
		m_pData = new T[nSize];
		m_PushIdx = 0;
		m_PopIdx = 0;
	}
	~SafeArray()
	{
		delete[](m_pData);
		m_pData = NULL;
	}
	bool HasItem()const
	{
		return m_PushIdx != m_PopIdx;
	}
	void RemoveTopItem()
	{
		m_PopIdx = (m_PopIdx + 1) & m_ArraySizeInv;
	}
	T GetItem()
	{
		if (HasItem() == false)
			return T(0);

		UINT idx = m_PopIdx;
		m_PopIdx = (m_PopIdx + 1) & m_ArraySizeInv;
		return m_pData[idx];
	}
	T GetItemNoRemove()
	{
		return m_pData[m_PopIdx];
	}
	bool HasSpace()
	{
		return ((m_PushIdx + 1) & m_ArraySizeInv) != m_PopIdx;
	}
	void AddItem(const T& t)
	{
		m_pData[m_PushIdx] = t;
		m_PushIdx = (m_PushIdx + 1) & m_ArraySizeInv;
	}
	UINT Count()
	{
		int d = int(m_PushIdx - m_PopIdx);
		return UINT((d < 0) ? (m_ArraySize + d) : d);
	}
	UINT ArraySize()const
	{
		return m_ArraySize;
	}
protected:
	UINT m_ArraySize;
	UINT m_ArraySizeInv;
	CACHELINE(0);
	T*	volatile m_pData;
	CACHELINE(1);
	volatile UINT m_PushIdx;
	CACHELINE(2);
	volatile UINT m_PopIdx;
	CACHELINE(3);
};
