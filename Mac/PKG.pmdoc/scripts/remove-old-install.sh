#!/bin/sh

echo $2 > ~/linphone-web-install.log
rm -rf $2/np@PROJNAME@-*.plugin
rm -rf ~/Library/Internet\ Plug-Ins/np@PROJNAME@-*.plugin
