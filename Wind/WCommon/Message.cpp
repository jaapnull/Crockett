// implementation headers
#include <PCHWind.h>

// compilation unit header
#include <WCommon/Message.h>
#include <WWin32/Win32.h>
/* 
	Does one "pump" of the message pump which dispatches waiting messages to the appropriate windows
	Beware that on non-blocking and blocking calls, this will handle all waiting messages, but will exit immediately after handling WM_QUIT

	returns false if a WM_QUIT has been received, returns true otherwise
*/

bool gDoMessageLoop(bool inBlocking)
{
	MSG message;
	bool message_found = false;
	bool do_blocking_retrieval = inBlocking;

	do
	{
		if (do_blocking_retrieval == true)
		{
			// blocking function waiting of at least a single message
 			message_found = GetMessage(&message, 0, 0, 0) != 0;
			// only the first call is blocking, afterwards we clean up any waiting messages using non-blocking calls
			do_blocking_retrieval = false;
		}
		else
		{
			// non-blocking version of GetMessage
			message_found = PeekMessage(&message, 0, 0, 0, PM_REMOVE) != 0;
		}

		if (message_found)
		{
			// break and return false for _only_ this message
			if (message.message == WM_QUIT)
			{
				return false;
			}
			// dispatch message to windows for message handling, this will be picked up by the message handler of a window if connected
			DispatchMessage(&message);
		}
	} while (message_found); // when non-blocking, we go through all the messages that we can, before giving control back.
	return true;
}
