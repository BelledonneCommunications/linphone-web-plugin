#!/bin/bash

MODDIR=$(pwd)/mod/
DESTDIR=$(pwd)/tmp/
BOOST=$(pwd)/../../../src/3rdParty/boost/
echo $DESTDIR
rm result.txt
rm -rf $MODDIR
mkdir -p $MODDIR
rm -rf $DESTDIR
mkdir -p $DESTDIR

pushd ../

headers=$(find . -maxdepth 1 -type f -name '*.h')
for file in $headers ; do
   echo $file
   cat $file | \
   sed "s/#include/\/\/#include/g" | \
   sed "s/\/\/#include \"macro.h\"/#include \"macro.h\"/g" | \
   sed "s/\/\/#include \"wrapperapi.h\"/#include \"wrapperapi.h\"/g" \
   > $MODDIR/$file 
done

popd

pushd $MODDIR

headers=$(find . -maxdepth 1 -type f -name '*.h')
for file in $headers ; do
   echo $file
   cat $file | \
   c++ -I$BOOST -x c++ -E - > $DESTDIR/$file
done

popd

pushd $DESTDIR
anjuta-tags --c-kinds=p --fields=-k+n+S+T  -f - *.h | \
perl -pe '1 while s/\/\^(.*)\t(.*)\/\;/\/\^\1 \2\/\;/gc' | \
sed  "s#$DESTDIR##g" > ../result.txt
popd
