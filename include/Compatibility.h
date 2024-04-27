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

#ifndef COMPATIBILITY_H
#define COMPATIBILITY_H

#ifdef _WIN32
    #include <windows.h>
    #include <process.h>
    #include <direct.h> // Include this header for _mkdir
    
    namespace Compatibility
    {
        typedef void ( ThreadFunction)(void *param);    

        void Delay(unsigned milliseconds);
        int GetTickCount();
        void Itoa(int num, char *buffer, int base);        
        HANDLE LaunchThread(ThreadFunction threadFunction, void *param);
        void CloseThread();
    }
#elif __linux__
    #include <unistd.h>
    #include <time.h>
    #include <pthread.h>

    typedef unsigned long DWORD;

    namespace Compatibility
    {        
        typedef void* ( ThreadFunction)(void *param);

        void Delay(unsigned milliseconds);
        int GetTickCount();
        void Itoa(int num, char *buffer, int base);        
        
        void* LaunchThread(ThreadFunction threadFunction, void *param);
        void CloseThread();
    }
#endif

#endif // COMPATIBILITY_H
