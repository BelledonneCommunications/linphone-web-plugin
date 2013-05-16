#!/bin/sh
for file in *.crx; do
   ./crx2zip.py "${file}" "${file%.crx}.zip"
done
