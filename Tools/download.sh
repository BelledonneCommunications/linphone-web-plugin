#!/bin/sh
VERSION=0.0.1.2 
rm linphone-web*
wget -r -nH --cut-dirs=2 --no-parent -A linphone-web*-latest.* http://www.linphone.org/snapshots/linphone-web/
rm robots.txt
for i in *; do
	a=$(echo "$i" | sed "s/^\(linphone-web\)\(.*\)-latest\(.*\)$/\1-$VERSION\2\3/g")
	echo "$i to $a"
	mv $i $a
done
