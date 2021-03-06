# [![Screenshot](/etc/screenshots/LabVIEW_Simulation.png)](https://github.com/WinT-3794/QDriverStation/tree/master/etc/screenshots)

[![Build Status](https://travis-ci.org/WinT-3794/QDriverStation.svg?branch=master)](https://travis-ci.org/WinT-3794/QDriverStation)
[![Donate button](https://img.shields.io/badge/bitcoin-donate-brightgreen.svg)](https://blockchain.info/address/1K85yLxjuqUmhkjP839R7C23XFhSxrefMx "Donate once-off to this project using BitCoin")
[![Built with Qt](https://img.shields.io/badge/built%20with-Qt-brightgreen.svg)](http://qt.io)
[![Github downloads](https://img.shields.io/github/downloads/wint-3794/qdriverstation/total.svg)](https://github.com/wint-3794/qdriverstation/releases)

The QDriverStation is a cross-platform and open-source alternative to the FRC Driver Station. It runs on both desktop devices (including laptops) and mobile devices (such as phones and tablets), making it easier for FRC teams test their robot code and make public presentations with their robots.

Our application is free to use, share and modify. You are welcome to test it and see the code to understand how it works!

The actual code that moves and manages a FRC robot is found in a [separate repository](https://github.com/WinT-3794/LibDS), which you can use in your own Qt projects or change it to support older (and future) communication protocols.

### Feature Requests

[![Feature Requests](http://feathub.com/WinT-3794/QDriverStation?format=svg)](http://feathub.com/WinT-3794/QDriverStation)

###### Note for Linux users

If you happen to use ArchLinux, [lopsided98](https://github.com/lopsided98) configured a [repository](https://aur.archlinux.org/packages/qdriverstation-git/) for this project.

Until very recently, I did not generate Debian packages. [Dakota Keeler](https://github.com/bearzrobotics) provided a [package for version 0.14](https://drive.google.com/file/d/0BwmIj7Fz03lXZ1JjYnhLVVdRR0E/view) and made a [video tutorial](https://www.youtube.com/watch?v=G9DywTB9_cY) on how to compile the QDriverstation, which you may find very helpful.

### Install notes

Once you finish installing the software, you can launch it and begin driving your robot. Just be sure to input your team number and to verify that the joysticks are working correctly.

Mac users will be prompted to download an additional driver for Xbox 360 controllers to work.

###### Warning for Linux users

Due to a kernel bug, the QDriverStation may detect an external mouse as a joystick. If that happens, just disable the faulty joystick/mouse by clicking on the power button next to its name:

![Blacklisting Animation](etc/animations/blacklist.gif)

### Build instructions

###### Requirements

The only requirement to compile the application is to have [Qt](http://www.qt.io/download-open-source/) installed in your system. The desktop application will compile with Qt 5.2 or greater.

- If you are using Linux, make sure that you have installed the following packages:
    - libsdl2-dev

The project already contains the compiled SDL libraries for Windows and Mac.

###### Compiling the application

Once you have Qt installed, open *QDriverStation.pro* in Qt Creator and click the "Run" button.

Alternatively, you can also use the following commands:
- qmake
- make
- **Optional:** sudo make install
 
You can check this [tutorial by Dakota Keeler](https://www.youtube.com/watch?v=G9DywTB9_cY) to compile the QDriverStation.

### Credits

This application was created by [this guy](http://github.com/alex-spataru).

Of course, many people contributed in different ways to this project, you can find more details in the [contributors list](CONTRIBUTORS.md). Finally, we want to thank you for trying this little project, we sincerely hope that you enjoy our application and we would love some of your feedback.
