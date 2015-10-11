# QDriverStation

[![Build Status](https://travis-ci.org/WinT-3794/QDriverStation.svg?branch=master)](https://travis-ci.org/WinT-3794/QDriverStation)
[![BitCoin donate button](https://img.shields.io/badge/bitcoin-donate-yellow.svg)](https://blockchain.info/address/1K85yLxjuqUmhkjP839R7C23XFhSxrefMx "Donate once-off to this project using BitCoin")
[![Project Stats](https://www.openhub.net/p/qdriverstation/widgets/project_thin_badge.gif)](https://www.openhub.net/p/qdriverstation)

### Introduction

QDriverStation is an open-source, cross-platform alternative to the FRC Driver Station.

The application looks and behaves almost identically to the FRC Driver Station, we may release some tutorials in the future. If you are already familiar with the FRC Driver Station, and like our project, we would welcome any contributions regarding the documentation and/or tutorials for new users.

Joystick input is achieved with SDL 2.0, which is a cross-platform library for C/C++.

The actual code that moves and manages a FRC robot is found in a [separate repository](https://github.com/WinT-3794/LibDS), which you can use in your own Qt projects or change it to support older (and future) communication protocols. Please check the [progress](https://github.com/WinT-3794/LibDS#progress) section for more info.

[![Screenshot](/etc/screenshots/Mac.png)](https://raw.githubusercontent.com/WinT-3794/QDriverStation/master/etc/screenshots/Mac.png)

### Install notes

Once you finish installing the software, you can launch it and begin driving your robot. Just be sure to input your team number and to verify that the joysticks are working correctly. 

Notes:
- Mac users will be prompted to download an additional driver for Xbox 360 controllers to work.
- Windows users will be prompted to download Bonjour to get mDNS working, this is not necesary if you have the official FRC Driver Station already installed or if you use a static IP.

If your team uses a custom IP address for your roboRIO (or mDNS is not working, try 10.XX.YY.26), navigate to the 'Preferences' tab and click the 'Advanced Settings...' button to configure the driver station.

### Build instructions

###### Requirements

The only requirement to compile the application is to have [Qt](http://www.qt.io/download-open-source/) installed in your system. The desktop application will compile with both Qt4 and Qt5.

- If you are using Linux, make sure that you have installed the following packages:
    - libsdl2-dev
    
The project already contains the compiled SDL libraries for Windows and Mac.

###### Compiling the application

Once you have Qt installed, open "BuildNormal.pro" with Qt Creator. If you just want to compile the application by yourself (and not change the source code), open "BuildSCU.pro" with Qt Creator.

The main difference between these two files is that the BuildSCU.pro uses the SCU (single compulation unit) method, which results with a faster compilation and (in some cases) a smaller binary file. The caveat with this method is that we must do some ugly hacking and include all the *.cpp files in a master build file, which must be updated regularly in order to include all files.

### Credits

This application was created by FRC team 3794 "WinT" from Metepec, Mexico. We sincerely hope that you enjoy our application and we would love some feedback from your team about it.
