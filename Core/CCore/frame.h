#pragma once
#include <CCore/types.h>
#include <CGeo/quad.h>

template <class T>
class BaseFrame
{
protected:
	T*					mData;
	uint				mWidth;
	uint				mHeight;
	int					mPitch;

public:
	BaseFrame() : mWidth(0), mData(0), mHeight(0), mPitch(0)					{}

	inline uint			GetWidth()		const	{return mWidth;}
	inline uint			GetHeight()		const	{return mHeight;}
	inline int			GetPitch()		const	{return mPitch;}
	inline T*			GetBaseData()			{return mData;}
	inline const T*		GetBaseData()	const	{return mData;}

	void SetAll(const T& inValue)
	{
		offset64 byte_addres = ((offset64) GetBaseData());
		byte_addres += ((offset64)(mHeight-1)*-mPitch) * (mPitch < 0); // shifts the pointer to the last scanline when mPitch < 0

		T* pointer = (T*) byte_addres;

		for (uint y = 0; y < mHeight; y++)
		{
			for (uint x = 0; x < mWidth; x++)
			{
				pointer[x] = inValue;
			}
			pointer = (T*) (offset64(pointer) + GetPitch());
		}			
	}

	void SetRegion(const iquad& inRegion, const T& inValue)
	{
		offset64 byte_addres = ((offset64)GetBaseData());
		byte_addres += ((offset64)(mHeight - 1)*-mPitch) * (mPitch < 0); // shifts the pointer to the last scanline when mPitch < 0

		T* pointer = (T*)byte_addres;

		for (int y = inRegion.mTop; y < inRegion.mBottom; y++)
		{
			for (int x = inRegion.mLeft; x < inRegion.mRight; x++)
			{
				pointer[x] = inValue;
			}
			pointer = (T*)(offset64(pointer) + GetPitch());
		}
	}

	void Set(int inX, int inY, T inValue)
	{
//		if (inX < 0 || inY < 0 || inX >= (int) mWidth || inY >= (int) mHeight )
//			return;
		assert(inX < (int)GetWidth() && inY < (int)GetHeight());
		offset64 byte_addres = ((offset64 ) GetBaseData());
		byte_addres += ((offset64)(mHeight-1)*-mPitch) * (mPitch < 0); // shifts the pointer to the last scanline when mPitch < 0
		byte_addres += inX * sizeof(T) + inY * mPitch;;
		*((T*)byte_addres) = inValue;
	}

	inline T& Get(int x, int y)
	{
		assert(x < (int)GetWidth() && y < (int)GetHeight());
		offset64  byte_addres = ((offset64 ) GetBaseData());
		byte_addres += ((offset64)(mHeight-1)*-mPitch) * (mPitch < 0); // shifts the pointer to the last scanline when mPitch < 0
		offset64  i = x * sizeof(T) + y * mPitch;
		byte_addres += i;
		return *((T*)byte_addres);
	}

	inline const T& Get(int x, int y) const
	{
		assert(x < (int)GetWidth() && y < (int)GetHeight());
		offset64 byte_addres = ((offset64) GetBaseData());
		byte_addres += ((offset64)(mHeight-1)*-mPitch) * (mPitch < 0); // shifts the pointer to the last scanline when mPitch < 0
		byte_addres += x * sizeof(T) + y * mPitch;
		return *((T*)byte_addres);
	}

	inline void SetBaseData(T* d) {mData = d;}

	virtual void Resize(unsigned int w, unsigned int h, int p = 0)
	{
		if (p == 0) p = w * sizeof(T);		
		mWidth = w;
		mHeight = h;
		mPitch = p;
	}
};

// An array for mData
template <class T>
class DataFrame : public BaseFrame<T>
{
public:
	bool mWrapped; //if true, the array is mWrapped and cannot be deleted or Resized
	virtual void Resize(unsigned int w, unsigned int h, int mPitch = 0)
	{
		assert(!mWrapped);
		if (mData) delete[] mData;
		mData = new T[w*h];
		BaseFrame::Resize(w, h, mPitch);
	}

	DataFrame() : mWrapped(false)
	{}

	DataFrame(iquad inSection, BaseFrame<T>& inSoureFrame) : mWrapped(true)
	{
		BaseFrame::Resize(inSection.GetWidth(), inSection.GetHeight(), inSoureFrame.GetPitch());
		if (GetPitch() > 0)
		{
			mData = &inSoureFrame.Get(inSection.mLeft, inSection.mTop);
		}
		else
		{
			mData = &inSoureFrame.Get(inSection.mLeft, inSection.mBottom);
		}
	}
	
	DataFrame(unsigned int mWidth, unsigned int mHeight) : mWrapped(false)
	{
		SetBaseData(new T[mWidth * mHeight]);
		Resize(mWidth, mHeight);
	}
	DataFrame(unsigned int mWidth, unsigned int mHeight, T* mData) : mWrapped(true)
	{
		SetBaseData(mData);
		Resize(mWidth, mHeight);
	}
	
	~DataFrame()
	{
		if (!mWrapped) delete mData;
	}
};
