# GD2P02 - Physics Programming
Assignment 2 - 2D Physics Project  
2D Physics Game

## Contents  
- [Description](#description)  
- [Features](#features)  
- [Requirements](#requirements)  
- [Installation](#installation)  
- [Controls](#controls)  
- [Issues](#issues)
- [Disclaimer](#disclaimer)  
- [Credits](#credits)  
  
## Description  
2D Physics Game attempts to recreate the core gameplay from Angry Birds (Rovio, 2009) using Box2D, for Assignment 2 of GD2P02. 
## Features  
- Two levels
- Indestructible block/plank items with physics behaviour
- Enemy entities
- Indestructible ground
- Fling-able projectiles
#
- Gravity
- Forces
- Rigid Bodies
## Requirements  
- Visual Studio 2022 (v143)
- Windows SDK 10.0
- C++ Language Standard: ISO C++20 Standard (/std:c++20)
- C Language Standard: ISO C17 (2018) Standard (/std:c17)
- Windows x64
- Box2D dependency
- SFML dependency
## Installation  
Download and extract from the zip file.  
This program can be run from the .exe file in the release build zip. Only relevant resources are provided in their respective folder and dependencies are provided.  
This program can also be run by opening the "Physics.sln" file in the source code folder and built and run in Debug or Release mode with Windows x64.  
## Controls  
This program has been designed to be operated with standard mouse and keyboard controls.  
#### Mouse
- Left Mouse Button: Click and drag to aim and set power. Release to fire
#### Keyboard
- P: Pause
## Issues  
- Including restitution for bounciness of projectile objects made collisions inconsistent. This has been removed last minute and is not included in the Post Mortem.
## Disclaimer  
This program is as complete as possible for submission. Most errors should not be present and any warnings should only come from dependencies.  
All code was written by Shiko based off my own knowledge from classes with lecturers and self driven research of the C++ coding language.  
This program has been cleaned for submission. Known issues have been searched for and fixed, unknown bugs may still be present.  
.vs/Physics/v17/.suo is included for ease of solution loadup purposes due to the project being manually configured. The solution should start with x64 Release, with main.cpp and Scene.h in side by side docks.  
## Credits
[Font - Sugar Bread](https://www.dafont.com/sugar-bread.font)  
[UI - UI Pack](https://kenney.nl/assets/ui-pack)  
[Characters - Animal Pack Redux](https://kenney.nl/assets/animal-pack-redux)  
[Environment - Physics Assets](https://kenney.nl/assets/physics-assets)  
#
Shikomisen 2024  
Media Design School  
GD2P02 - Physics Programming  
Written for Assignment 2  
  
[Home ^](#gd2p02---physics-programming)