#pragma once

#include <CCore/Types.h>
#include <CCore/Array.h>

// Define aliases for two implementation specific types, (HWND/HANDLE) and ATOM
typedef void*	_Handle;
typedef uint16	_Atom;
typedef void*	MessageParam;

bool gDoMessageLoop(bool inBlocking);

enum MessageReturnCode
{
	mrcUnhandled = 0,	// unhandled in windows is usually zero
	mrcHandled,
	mrcNoHandler
};

template<class ParentType>
class IMessageHandler
{
public:
	virtual MessageReturnCode HandleMessage(ParentType* inParent, uint inMessage, MessageParam inParamA, MessageParam  inParamB) = 0;
};


// A Source generates messages and pushes them to its handlers; anything not pushed on is handled by parent itself
template<class ParentType>
class MessageSource
{
public: 
	// A Handler is a function that can handle messages
	
	void AddHandler(IMessageHandler<ParentType>* inHandler) { mHandlers.Append(inHandler); }

	virtual MessageReturnCode	HandleMessage(ParentType* inParent, uint inMessage, MessageParam inParamA, MessageParam inParamB)
	{
		for (uint x = 0; x < mHandlers.GetLength(); x++)
		{
			if (mHandlers[x]->HandleMessage(inParent, inMessage, inParamA, inParamB) == mrcHandled) return mrcHandled;
		}
		return mrcUnhandled;
	}	

private:
	Array<IMessageHandler<ParentType> *> mHandlers;	// we assume all handlers have the same lifespan as Parent	
};



