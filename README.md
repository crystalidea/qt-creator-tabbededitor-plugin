# Qt Creator Tabbed Editor Plugin

The aim of this project is to provide a tab-based experience in [Qt Creator](https://wiki.qt.io/Category:Tools::QtCreator). Currently compatible with Qt 5.15.x and Qt Creator 4.15.x.

![pic1](./tab.png)

### Copyrights

Original idea by [SNA Soft](https://sourceforge.net/projects/tabbededitor/), modified by [Oleg Shparber](https://github.com/trollixx/qtcreator-tabbededitor-plugin.git), fixed by [wanjiadenghuo111](https://github.com/wanjiadenghuo111/qtcreator-tabbededitor-plugin) and extended by [Rolf Hausen](https://github.com/RolfHausen/qtcreator-TabbedEditor-plugin). 

### Installation

#### Step 1: compile Qt

If you own a commercial license, then use Qt 5.15.4, otherwise latest available Qt 5.15.2 would be also fine.

Download and extract Qt to some folder, e.g.

`~/Desktop/qt-everywhere-src-5.15.4`

and compile it (we used -developer-build option and built only qtbase, qttools and qtdeclarative modules). Our [compile script](qt_compile_mac_dev.sh) we use on macOS.

#### Step 2: compile Qt Creator and the tabbededitor plugin

You are supposed to have [cmake](https://cmake.org/) installed.

Download and extract the Qt Creator source code to some folder:

`~/Desktop/qt-creator-opensource-src-4.15.2`

Put the contents of this repository to 

`~/Desktop/qt-creator-opensource-src-4.15.2/src/plugins/tabbededitor`

Open CMakeLists.txt located at

`~/Desktop/qt-creator-opensource-src-4.15.2/src/plugins/CMakeLists.txt`

and add the following line

`add_subdirectory(tabbededitor)`

to the Level 3.

Now open Terminal to build:

```
cd ~/Desktop/qt-creator-opensource-src-4.15.2
mkdir qtcreator_build
cd qtcreator_build

cmake -DCMAKE_BUILD_TYPE=Release "-DCMAKE_PREFIX_PATH=~/Desktop/qt-everywhere-src-5.15.4/qtbase;~/Desktop/qt-creator-opensource-src-4.15.2" ..
cmake --build .
```

**Notice**

When compiling we had issues with two missing Qt private headers that were not copied for some reason. If you also experience same problems please make sure you have the following private headers:

```
~/Desktop/qt-everywhere-src-5.15.4/qtbase/include/QtQuick/5.15.4/QtQuick/private/qtquick-config_p.h
~/Desktop/qt-everywhere-src-5.15.4/qtbase/include/QtQml/5.15.4/QtQml/private/qtqml-config_p.h
```
