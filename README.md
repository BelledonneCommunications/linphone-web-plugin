Compilation
===========

Prerequisites
-------------

### Common
* python (2.7)
* python-M2Crypto (for python 2.7): Used for signing
* cmake
* java (used for packaging)
* openssl
* awk (on Windows get it from http://gnuwin32.sourceforge.net/packages/gawk.htm
  and add it to your PATH environment variable)
* Make sure you have cloned the linphone-web-plugin repository recursively.
  If this is not the case, get the submodules:

	git submodules update --recursive --init

### Windows platform
* Visual studio
* WiX to generate the MSI installer (http://wixtoolset.org/)

### Linux platform
* X11 dev
* chrpath

### Mac OS X platform
* Xcode
* Mac ports (for python and modules)

Linphone Deps
------
The Linphone deps are a package containing all the dependencies needed to be
able to compile the linphone-web project on the Windows platform. Therefore
it is only needed when compiling the plugin on Windows. It will be downloaded
automatically from the linphone website when configuring the plugin so you
should not need to build it yourself.

### Generate the Linphone deps
Clone the oe-lite git repository from:

	git clone git://git.linphone.org/oe-lite.git --recursive

Follow the README.md file from this repository.


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
