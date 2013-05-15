#!/bin/sh
rm -rf ${HOME}/Library/Internet\ Plug-Ins/@FBSTRING_PluginFileName@.@PLUGIN_EXT@
mv /tmp/@FBSTRING_PluginFileName@.@PLUGIN_EXT@ ${HOME}/Library/Internet\ Plug-Ins/
exit 0
