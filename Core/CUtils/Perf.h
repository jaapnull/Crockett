#pragma once
#include <CCore/Types.h>
#include <CCore/Array.h>
#include <CCore/String.h>
#include <CUTils/Atomics.h>

/**
TimeLine class
Light-weight thread safe perf tool to record records into a timeline.
Use PushTimedEvent() to push a TimedEventBase object onto the stack
Use GetPushedEvents() to traverse linear buffer and retrieve all event offsets for read-back
Uses Win32 High Requency performance API in CPP
**/

typedef uint64 TimeStamp;

///@brief Base class for all timed events; has a header containing threadID, Size and Type for a light weight RTTI
class TimedEventBase
{
public:

	enum class TimedEventType : uint8
	{
		tetText,				///< TimedEvent containing a string
		tetInteger,				///< TimedEvent containing a single integer
		tetCount,
		tetInvalid = tetCount
	};

	TimedEventBase(uint16 inPayloadSize, TimedEventType inType) : 
		mSizeInBytes(inPayloadSize), 
		mType(inType)						{}
	TimedEventBase()						{}
	uint32 GetRelativeTime() const			{ return mRelativeTime; }
	void SetTime(uint32 inRelativeStamp)	{ mRelativeTime = inRelativeStamp; }
	uint GetSizeInBytes() const				{ return mSizeInBytes; }
private:
	uint32				mRelativeTime		= 0;							///< Relative timestamp from start of time line
	uint16				mSizeInBytes		= 0;							///< Size of this header object (including sizeof(TimedEventBase) TODO make this auto-fill with some template magic
	uint8				mThreadID			= 0;							///< ID of posting thread
	TimedEventType		mType				= TimedEventType::tetInvalid;	///< Type of Event (lightweight RTTI)
};

///@brief simple timed event with a string
class TimedEventText : public TimedEventBase
{
public:
	TimedEventText(const String& inText) : 
		TimedEventBase(uint(sizeof(TimedEventText)), TimedEventType::tetText), 
		mText(inText) {}
private:
	String mText;
};

///@brief simple timed event with an integer
class TimedEventInteger : public TimedEventBase
{
public:
	TimedEventInteger(uint inInteger) : TimedEventBase(uint(sizeof(TimedEventInteger)), TimedEventType::tetInteger), mInteger(inInteger) {}
private:
	uint32 mInteger;
};

///@brief Base timing object that handles multi-threaded posting on it, using TimeEventHeader objects
class TimeLine
{
public:
	TimeLine(uint inBufferSizeInBytes);
	~TimeLine();
	void						GetPushedEvents(Array<TimedEventBase*>& outEvents);
	void						Start();
	void						Stop();
	void						PushTimedEvent(const TimedEventBase& inNewEvent);
	float						GetTimeInSeconds(const TimedEventBase& inHeader) const
	{
		return float(inHeader.GetRelativeTime()) / float(mPerfFreq);
	}

private:
	TimeStamp				mStartTime;					///< Time the timeline was stopped
	TimeStamp				mStopTime;					///< Time the timeline was started
	uint64					mPerfFreq;					///< High Prec. Timer ticks/ms

	uint64					mBufferSizeInBytes = 0;		///< Memory size of block allocated by mBuffer
	void*					mBuffer			= nullptr;	///< Array of time events; when unlocked size = MaxSize, and atomic is used to keep track; when locked, resize is called to proper size
	AtomicInt64				mPointer;					///< Pointer to latest time index in bytes offset from mBuffer
	bool					mRunning = false;			///< Is timeline locked for read-back during stop and next start()

	inline static uint64	sGetCurrentTime();			///< Get Current timestamp
	inline static uint64	sGetFreq();					///< Get timestap-to-second conversion

};


