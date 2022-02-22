#include <process.h>
#include "Threading.h"

namespace Threading
{
    HANDLE CreateThread(ThreadFunction threadFunction, void *param)
        {
            HANDLE threadHandle = (HANDLE)_beginthread(threadFunction, 0, param);

            return threadHandle;
        }

        void CloseThread()
        {
            _endthread();
        }
}
