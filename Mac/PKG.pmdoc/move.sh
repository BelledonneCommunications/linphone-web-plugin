#!/bin/sh
rm -rf ${HOME}/Library/Internet\ Plug-Ins/@FBSTRING_PluginFileName@.plugin
mv /tmp/@FBSTRING_PluginFileName@.plugin ${HOME}/Library/Internet\ Plug-Ins/
exit 0
