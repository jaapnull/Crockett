#pragma once

#include <CCore/Types.h>
#include <CCore/Assert.h>

template <typename T>
class RefObject
{
public:
	RefObject()														{}
	void IncRef() const												{ if (mRefCount == sRefNoOwner) mRefCount = 1; else mRefCount++; }
	void DecRef() const
	{ 
		gAssert(mRefCount > 0); 
		mRefCount--; 
		if (mRefCount == 0) 
		{
			Destruct(); 
		}
	}

private:
	// special refcount to indicate refcount zero while no owner
	const uint sRefNoOwner = 0xFFFFFFFF;
	RefObject(const RefObject<T>& inOther)							{ gAssert(inOther.mRefCount == 0); }
	void operator=(const RefObject<T>& inOther)						{ gAssert(inOther.mRefCount == 0); }
	void				Destruct() const							{ delete (T*) this; }
	mutable volatile uint		mRefCount = sRefNoOwner;
};

template <typename T>
class RefPtr
{
public:
	RefPtr() : mPtr(nullptr)										{}
	RefPtr(const RefPtr<T>& inOther)								{ Aquire(inOther.mPtr); }
	RefPtr(T* inPointer)											{ Aquire(inPointer); }
	~RefPtr()														{ Release(); }
	T*					GetPtr() const								{ return T; }
	T*					operator->() const							{ return mPtr; }
	bool				operator!=(T* inPointer)					{ return mPtr != inPointer; }
	bool				operator==(T* inPointer)					{ return mPtr == inPointer; }
	void				operator=(T* inNewPointer)					{ Release(); Aquire(inNewPointer);  }
	void				operator=(const RefPtr<T>& inOther)			{ Release(); Aquire(inOther.mPtr);  }
	operator T*() const												{ return mPtr; }

private:
	T*					mPtr		= nullptr;
	void				Release()
	{
		if (mPtr != nullptr)
		{ 
			mPtr->DecRef();
		}
		mPtr = nullptr; 
	}
	void				Aquire(T* inPointer)
	{ 
		gAssert(mPtr == nullptr); 
		mPtr = inPointer; 
		if (mPtr != nullptr) 
		{
			mPtr->IncRef(); 
		}
	}
};



