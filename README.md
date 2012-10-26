Linphone Web
============

Prerequisites
-------------

### Common
* python (2.7)
* python-M2Crypto (for python 2.7): Used for signing
* cmake
* java (used for packaging)

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

* [GNU/Linux x86](http://www.linphone.org/rootfs/linphone-rootfs-linphone-webplugin-linux-x86.tar.gz)
* [GNU/Linux x86_64](http://www.linphone.org/rootfs/linphone-rootfs-linphone-webplugin-linux-x86_64.tar.gz)
* [Windows 32 bits](http://www.linphone.org/rootfs/linphone-rootfs-linphone-webplugin-windows.tar.gz)
* [Mac OS X x86_64](http://www.linphone.org/rootfs/linphone-rootfs-linphone-webplugin-darwin.tar.gz)


### Generate a Rootfs
TODO
#### Manual way
TODO
#### oe-lite way
TODO


Firebreath
----------
FireBreath aims to be a cross-platform plugin architecture. You have to
download the last stable version using git:

    git clone git://github.com/firebreath/FireBreath.git -b firebreath-1.6 \
    firebreath-1.6

or [download the zip archive](https://github.com/firebreath/FireBreath/zipball/firebreath-1.6).

Place linphone-web project in the `./projects/` directory at the firebreath
root(create it if doesn't exist). Follow the [Firebreath documentation](http://www.firebreath.org/display/documentation/Building+FireBreath+Plugins)
following the used system for compiling linphone-web.


Compile
-------
Follow firebreath document in order to compile linphone-web plugin. 
The generated files can be found in `./build/bin` directory inside
Firebreath project root. 

### Windows
Before compile remove all `.h` files in `./Rootfs/include/` in linphone-web
project directory.

### Mac OS X
Don't use XCode directly it doesn't use corrects environment and target 
architectures. After having prepared the projects, enter in `./build/`
directory of Firebreath and run the following command:

    xcodebuild -arch x86_64


Sign
---
In order to sign each produced container you have to copy in `./sign/` 
directory at the linphone-web project root (create it if doesn't exist) the 
following files:

* **linphoneweb.pfx**: The file containing private/public keys and the 
certificate(only for active-x part)
* **linphoneweb.pem**: The file containing private/public keys and the 
certificate
* **passphrase.txt**: The password used for open the two previous files 
(can be added just before compile and remove after)


Web
---
The web project use [grunt](http://gruntjs.com/) for generate Web resources.
Follow the [instructions](https://github.com/gruntjs/grunt) for install grunt
(depends on npm). You also have to install
[grunt-css](https://github.com/jzaefferer/grunt-css) and
[grunt-html](https://github.com/jzaefferer/grunt-html).

When theses tools are installed you have to "compile" the Web project using
the following command in the `./Web/` directory:

    grunt


### Development
You can use the grunt argument *dev* in order to create a server at
[http://localhost:8000](http://localhost:8000) exposing the web resources
using the following command:

    grunt dev


License
-------

	Linphone Web - Web plugin of Linphone an audio/video SIP phone
	Copyright (C) 2012  Yann Diorcet <yann.diorcet@linphone.org>	
	
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
