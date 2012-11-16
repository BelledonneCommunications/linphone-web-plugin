#!/bin/bash

DESTDIR=$(pwd)/tmp/
echo $DESTDIR
rm result.txt
rm -rf $DESTDIR
mkdir -p $DESTDIR

pushd ../

headers=$(find . -maxdepth 1 -type f -name '*.h')
for file in $headers ; do
   echo $file
   cat $file | \
   sed "s/#include/\/\/#include/g" | \
   sed "s/\/\/#include \"macro.h\"/#include \"macro.h\"/g" | \
   c++ -x c++ -E - > $DESTDIR/$file
done

popd

pushd $DESTDIR
anjuta-tags --c-kinds=p --fields=-k+n+S+T  -f - *.h | \
perl -pe '1 while s/\/\^(.*)\t(.*)\/\;/\/\^\1 \2\/\;/gc' | \
sed  "s#$DESTDIR##g" > ../result.txt
popd
