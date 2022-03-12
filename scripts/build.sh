#!/bin/csh -f

# Instructions
#  . make empty build directory
#  . cp build.sh to empty build directory
#  . Run ./build.sh

set cdir = `pwd`

if (! -e CQPropertyView) then
  git clone https://github.com/colinw7/CQPropertyView.git CQPropertyView
endif

set inc = `cat CQPropertyView/src/CQPropertyView.pro | sed ':x; /\\$/ { N; s/\\\n//; tx }' | grep INCLUDEPATH | sed 's/INCLUDEPATH += //'`

set dirs = ()

foreach v ($inc)
  set n = `echo $v | grep '..\/..\/' | wc -l`

  if ($n != 1) then
    continue
  endif

  set v1 = `echo $v | sed 's@../../\(.*\)/include@\1@'`

  set dirs = ($dirs $v1)

  if (! -e $v1) then
    echo "--- Clone $v1 ---"

    git clone https://github.com/colinw7/${v1}.git $v1
  endif
end

foreach dir ($dirs)
  echo "--- Build $dir ---"

  if (-e $cdir/$dir/src) then
    cd $cdir/$dir/src

    if (! -e ../obj) then
      mkdir ../obj
    endif

    if (! -e ../lib) then
      mkdir ../lib
    endif

    set n = `find . -maxdepth 1 -name "*.pro" | wc -l`

    if ($n == 1) then
      qmake
    endif

    make
  endif
end

echo "--- Build CQPropertyView (src) ---"

cd $cdir/CQPropertyView/src

qmake

make

echo "--- Build CQPropertyView (test) ---"

cd $cdir/CQPropertyView/test

qmake

make
