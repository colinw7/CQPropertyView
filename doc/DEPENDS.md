This package depends on other packages in my github directory.

To get all code to successfully build follow the following steps:
 + Download charts into directory
   + git clone "https://github.com/colinw7/CQPropertyView" CQPropertyView
 + cd $ROOT/CQPropertyView/src
 + Check list of paths in CQPropertyView.pro (INCLUDEPATH variable)
   + for each path download the missing project into the same directory as you downloaded CQPropertyView
 + Check list of libraries in CQPropertyView/test/CQPropertyViewTest.pro (unix:LIBS variable)
   + for each library download the missing project into the same directory as you downloaded CQPropertyView
 + build each downloaded library using its src and/or test directory
   + each library may have additional dependencies in its .pro or Makefile

Additional dependencies are standard UNIX packages:
 + png
   + libpng12.so.0
 + jpeg
   + libjpeg.so.8
 + tre
   + libtre.so.5

There is also a 'experimental' build script in $ROOT/CQPropertyView/scripts/build.sh. To use:
 . make empty build directory
 . cp $ROOT/CQPropertyView/scripts/build.sh to empty build directory
 . Run ./build.sh
