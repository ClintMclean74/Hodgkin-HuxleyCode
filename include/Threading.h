#ifndef THREADING_H
#define THREADING_H

typedef void* HANDLE;

typedef void ( ThreadFunction)(void *param);

void funct(int a, ThreadFunction funct2);

namespace Threading
{
    HANDLE CreateThread(ThreadFunction threadFunction, void *param);

    void CloseThread();
}

#endif // THREADING_H
