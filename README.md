Code System for the book "Solving Havana Syndrome and Biological Effects of RF Using the Hodgkin-Huxley Neuron Model"

Copyright (C) 2022 Mclean Research Institute Clint Mclean <clint@mcleanresearchinstitute.com>

The settings configuration files in the settings/ folder are for reproducing the experiments described in the book

Refer to these for configuration settings.

USAGE:
------

HodgkinHuxley.exe settings_file.txt

i.e,
HodgkinHuxley.exe ../settings/nn_amplification_layer_activity_testing_6_31C.txt

You should edit the launch.bat (windows) or launch.sh (Linux) files to do this. Use a file explorer GUI to launch these or from the command line:

./launch.bat (Windows)
./launch.sh (Linux. Requires setting permissions to run as a program. Right Click->Permissions->"Allow exectuting file as program" or from the command line: "chmod +x launch.sh") 


Compilation and Building
------------------------

If you haven't got cmake install it from here https://cmake.org/download/ for windows and linux.

Or for linux from the command line using commands such as:

For Ubuntu/Debian:
sudo apt-get install cmake

For Fedora:
sudo dnf install cmake

Arch Linux:
sudo pacman -S cmake

Compilation and building is straightforward. For windows and linux just go to the "\build" folder and at the command line enter:

cmake ..

and then:

cmake --build .


There's a CMakeLists.txt file in the main folder so "cmake .." generates the required cmake files in the build folder using this. Then "cmake --build ." generates the application also in the build folder.

VS Code editor and compiler should also be able to open the folder, compile and build it.


Licence:
--------

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.
