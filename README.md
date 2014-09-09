Compilation
===========

Prerequisites
-------------

### Common
* python (2.7)
* python-M2Crypto (for python 2.7): Used for signing
* cmake (2.8.2 or greater)
* java (used for packaging)
* openssl
* awk (on Windows get it from http://gnuwin32.sourceforge.net/packages/gawk.htm
  and add it to your PATH environment variable)
* patch (on Windows get it from http://gnuwin32.sourceforge.net/packages/patch.htm
  and add it to your PATH environment variable)
* Make sure you have cloned the linphone-web-plugin repository recursively.
  If this is not the case, get the submodules:

	git submodules update --recursive --init

### Windows platform
* Visual studio C++ 2010
* Windows Driver Kit Version 7.1.0 (http://www.microsoft.com/en-us/download/details.aspx?id=11800)
* WiX to generate the MSI installer (http://wixtoolset.org/). Use the version 3.7, newer versions do not work as of now.
* MinGW32 (http://mingw.org/)
  You need to install mingw-developer-toolkit, mingw32-base, mingw32-gcc-g++ and msys-base in the "Basic Setup".
  Make sure to follow the post-installation instruction from http://mingw.org/wiki/Getting_Started#toc2.
* nasm (http://www.nasm.us/)

### Linux platform
* X11 dev
* chrpath

### Mac OS X platform
* Xcode
* Mac ports (for python and modules)

Firebreath
----------
FireBreath aims to be a cross-platform plugin architecture. You have to
download the last stable version using git:

    git clone git://git.linphone.org/firebreath.git -b firebreath-1.7 --recursive

Place linphone-web-plugin project in the `./projects/` directory at the firebreath
root (create it if doesn't exist).
Follow the [Firebreath documentation](http://www.firebreath.org/display/documentation/Building+FireBreath+Plugins)
following the used system for compiling linphone-web.


Compile
-------
Follow firebreath document in order to compile linphone-web plugin. 
The generated files can be found in `./build/bin` directory inside
Firebreath project root. 

### Windows
You have to add python, openssl and WiX in the PATH environment variable.
Make sure you are building with Visual Studio 2010, using the prep2010.cmd
script.
If you want to compile in Debug mode, use the command line:

	prep2010.cmd projects\linphone-web-plugin build "-DWITH_DYNAMIC_MSVC_RUNTIME=1" "-DCMAKE_BUILD_TYPE=Debug"

and then use the Debug configuration in Visual Studio.
If you want to compile in Release mode, use the command line:

	prep2010.cmd projects\linphone-web-plugin build "-DWITH_DYNAMIC_MSVC_RUNTIME=1"

and then use the Release configuration in Visual Studio.

### Mac OS X
Don't use XCode directly it doesn't use corrects environment and target 
architectures. For configuring the firebreath, use the following command: 

    ./prepmac.sh -DCMAKE_OSX_DEPLOYMENT_TARGET="10.6" -DCMAKE_OSX_ARCHITECTURES="i386"

This is permit the plugin to run on older version of Mac OS X than the one
you use and force only one architecture. After enter in `./build/` directory 
of Firebreath and run the following command:

    xcodebuild -arch i386

### Support for additional features
If you want to activate/deactivate some features, you can add some "-D{option}=0|1" options to the
preparation command described above. Here is a list of some available features:
* ENABLE_VIDEO
* ENABLE_GPL_THIRD_PARTIES
* ENABLE_SRTP
* ENABLE_AMRNB
* ENABLE_AMRWB
* ENABLE_G729
* ENABLE_GSM
* ENABLE_OPUS
* ENABLE_SPEEX
* ENABLE_FFMPEG
* ENABLE_H263
* ENABLE_H263P
* ENABLE_MPEG4
* ENABLE_VPX
* ENABLE_X264
* ENABLE_OPENH264

For example, if you want to activate OpenH264 support, add the "-DENABLE_OPENH264=1" option to
the preparation command.


Sign
---
In order to sign each produced container you have to copy in `./sign/` 
directory at the linphone-web project root (create it if doesn't exist) the 
following files:

* **linphoneweb.pfx**: The file containing private/public keys and the 
certificate (only for active-x part)
* **linphoneweb.pem**: The file containing private/public keys and the 
certificate
* **passphrase.txt**: The password used for open the two previous files 
(can be added just before compile and remove after)
