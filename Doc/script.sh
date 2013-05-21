#!/bin/bash

MODDIR=$(pwd)/mod/
DESTDIR=$(pwd)/tmp/
BOOST=$(pwd)/../../../src/3rdParty/boost/
BOOST_FILES="boost/preprocessor/control/if.hpp \
boost/preprocessor/array/elem.hpp \
boost/preprocessor/repetition/enum.hpp \
boost/preprocessor/debug/assert.hpp \
boost/preprocessor/comparison/equal.hpp \
boost/preprocessor/stringize.hpp \
boost/mpl/aux_/preprocessor/token_equal.hpp"
CPP_EXTRA=
for f in $BOOST_FILES ; do
	CPP_EXTRA="$CPP_EXTRA -imacros $BOOST/$f"
done
echo $CPP_EXTRA

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
   c++ -I$BOOST $CPP_EXTRA -x c++ -E - | \
   sed 's/;/;\'$'\n/g' \
   > $DESTDIR/$file
done

popd

pushd $DESTDIR
anjuta-tags --c-kinds=p --fields=-k+n+S+T  -f - *.h | \
perl -pe '1 while s/\/\^(.*)\t(.*)\/\;/\/\^\1 \2\/\;/gc' | \
sed  "s#$DESTDIR##g" > ../result.txt
popd
