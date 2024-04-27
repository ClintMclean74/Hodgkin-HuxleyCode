/*
 * Code System for the book "Solving Havana Syndrome and Biological Effects of RF
 * Using the Hodgkin-Huxley Neuron Model"
 * Copyright (C) 2022 by Clint Mclean <clint@mcleanresearchinstitute.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Compatibility.h"
#define _DEFAULT_SOURCE

#ifdef _WIN32
#include <sysinfoapi.h>

namespace Compatibility
{
    void Delay(unsigned milliseconds)
    {
        Sleep(milliseconds);
    }

    int GetTickCount()
    {
        return GetTickCount64();
    }

    void Itoa(int num, char *buffer, int base)
    {
        _itoa_s(num, buffer, 100, base);
    }

    HANDLE LaunchThread(ThreadFunction threadFunction, void *param)
    {
        HANDLE threadHandle = (HANDLE)_beginthread(threadFunction, 0, param);
        // bool result = SetThreadPriority(threadHandle, THREAD_PRIORITY_HIGHEST);

        return threadHandle;
    }

    void CloseThread()
    {
        _endthread();
    }
}

#elif __linux__
#include <unistd.h>
#include <time.h>
#include <cstdio> // Required for snprintf
#include <thread>

namespace Compatibility
{
    void Delay(unsigned milliseconds)
    {
        usleep(milliseconds * 1000); // Convert milliseconds to microseconds
    }

    int GetTickCount()
    {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
    }

    void Itoa(int num, char *buffer, int base)
    {        
        if (base == 10)
        {
            snprintf(buffer, sizeof(buffer), "%d", num);
        }
    }

    void *LaunchThread(ThreadFunction threadFunction, void *param)
    {
        pthread_t *newThread = new pthread_t();
        int result = pthread_create(newThread, NULL, threadFunction, param);

        return newThread;
    }

    void CloseThread()
    {
        pthread_exit(NULL);
    }
}
#endif
