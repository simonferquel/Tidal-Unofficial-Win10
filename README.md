# Tidal-Unofficial-Win10
This is the source code of the Tidal Unofficial application for Windows 10 (currently PC and Phone).
It is a community based implementation of the Tidal app for the Windows platforms.

The app is built completely in C++ (mostly 11/14 flavor, with some bits of experimental C++17 code for await support).

# how to build

## download dependencies
Go to the folder /dependancies and execute the init.sh script. It will clone the cpp rest sdk (used for json parsing), date.h (used for date representations) and UWP-LocalDB-CPP (for local storage)

## build the project
The file /src/Common/Api/Config.cpp is explicitly gitignored. You must obtain an api token from and get the resources and apis urls used by Tidal by yourself and implement the symbols declared in Config.h

Please feel free to submit issues, pull requests etc. on this repo.

