#include <CUtils/Perf.h>
#include <CCore/String.h>
#include <cstring>

///@brief Initialize timeline with buffer size in bytes
TimeLine::TimeLine(uint inBufferSizeInBytes)
{
	mBuffer = gAllocAligned(inBufferSizeInBytes, alignof(TimedEventBase));
	mBufferSizeInBytes = inBufferSizeInBytes;
}

///@brief Clear buffer inited in ctor()
TimeLine::~TimeLine()
{
	if (mBuffer != nullptr)
		gFreeAligned(mBuffer);
	mBufferSizeInBytes = 0;
	mBuffer = nullptr;
}

///@brief Push timed event [THREADSAFE]
void TimeLine::PushTimedEvent(const TimedEventBase& inNewEvent)
{
	gAssert(mStartTime != 0);
	uint64 relative_time = sGetCurrentTime() - mStartTime;
	gAssert(relative_time < 0xFFFFFFFF);
	uint64 data_size_in_bytes = inNewEvent.GetSizeInBytes();
	// thread-safe atomic push
	uint64 prev_offset = mPointer.Add(data_size_in_bytes);
	gAssert(mPointer < mBufferSizeInBytes);
	TimedEventBase* output_pointer = (TimedEventBase*)gOffsetPointer(mBuffer, prev_offset);
	memcpy(output_pointer, &inNewEvent, data_size_in_bytes);
	output_pointer->SetTime(uint32(relative_time));
}

///@brief Start timeline, setting the timeline base
void TimeLine::Start()
{
	gAssert(mBuffer != nullptr);
	mPerfFreq = sGetFreq();
	mStartTime = sGetCurrentTime();
	mRunning = true;
}

///@brief Stop timeline and unlock it for read-back
void TimeLine::Stop()
{
	gAssert(mRunning == true);
	mStopTime = sGetCurrentTime();
	mRunning = false;
}

void TimeLine::GetPushedEvents(Array<TimedEventBase*>& outEvents)
{
	gAssert(mRunning == false);
	TimedEventBase* pointer = (TimedEventBase*) mBuffer;
	while (pointer != gOffsetPointer(mBuffer, mPointer))
	{
		outEvents.Append(pointer);
		pointer = gOffsetPointer<TimedEventBase>(pointer, (int) pointer->GetSizeInBytes());
	};
}


/// Win32 only impl
#include <Windows.h>

TimeStamp TimeLine::sGetCurrentTime()
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	return now.QuadPart;
}

uint64 TimeLine::sGetFreq()
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	return freq.QuadPart;
}


