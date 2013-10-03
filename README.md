Linphone Web
============

Prerequisites
-------------

### Common
* python (2.7)
* python-M2Crypto (for python 2.7): Used for signing
* cmake
* java (used for packaging)
* openssl

### Windows platform
* Visual studio

### Linux platform
* X11 dev

### Mac OS X platform
* Xcode
* Mac ports (for python and modules)

Rootfs
------
The Rootfs is a path containing all the dependencies needed to make works 
linphone project. In order to compile linphone-web we have to use a special
compiled version of linphone and its dependencies.

Following the used system(GNU/Linux, Windows, Mac OS X) you have to extract the
matched *rootfs* archive in the `./Rootfs/` directory at the linphone-web
project root (create it if doesn't exist).

* [GNU/Linux x86](http://www.linphone.org/snapshots/linphone-web/linphone-rootfs-linphone-webplugin-linux-x86-latest.tar.gz)
* [GNU/Linux x86_64](http://www.linphone.org/snapshots/linphone-web/linphone-rootfs-linphone-webplugin-linux-x86_64-latest.tar.gz)
* [Windows 32 bits](http://www.linphone.org/snapshots/linphone-web/linphone-rootfs-linphone-webplugin-windows-x86_64-latest.tar.gz)
* [Mac OS X x86](http://www.linphone.org/snapshots/linphone-web/linphone-rootfs-linphone-webplugin-darwin-x86-latest.tar.gz)
* [Mac OS X x86_64](http://www.linphone.org/snapshots/linphone-web/linphone-rootfs-linphone-webplugin-darwin-x86_64-latest.tar.gz)


### Generate a Rootfs
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
Before compile remove all `.h` files in `./Rootfs/include/` in linphone-web
project directory. Added to this you have to add python and openssl in the
PATH

### Mac OS X
Don't use XCode directly it doesn't use corrects environment and target 
architectures. For configuring the firebreath, use the following command: 

    ./prepmac.sh -DCMAKE_OSX_DEPLOYMENT_TARGET="10.5" -DCMAKE_OSX_ARCHITECTURES="x86_64"

This is permit the plugin to run on older version of Mac OS X than the one
you use and force only one architecture. After enter in `./build/` directory 
of Firebreath and run the following command:

    xcodebuild -arch x86_64


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

License
-------

	Linphone Web - Web plugin of Linphone an audio/video SIP phone
	Copyright (C) 2012-2013 Belledonne Communications
	
	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
