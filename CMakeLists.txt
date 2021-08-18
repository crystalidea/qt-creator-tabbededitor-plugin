cmake_minimum_required(VERSION 3.10)

# Remove when sharing with others.
list(APPEND CMAKE_PREFIX_PATH "your Path to QTCreator Instance")

project(TabbedEditor)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)

find_package(QtCreator COMPONENTS Core REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
set(QtX Qt${QT_VERSION_MAJOR})

add_qtc_plugin(TabbedEditor
  PLUGIN_DEPENDS
    QtCreator::Core
  DEPENDS
    ${QtX}::Widgets
    QtCreator::ExtensionSystem
    QtCreator::Utils
  SOURCES
    .github/workflows/build_cmake.yml
    .github/workflows/README.md
    README.md
    tabbededitor.cpp
    tabbededitor.h
    tabbededitor_global.h
    tabbededitorconstants.h
    tabbar.h
    tabbar.cpp
    include/plog/Log.h
    include/plog/Init.h
    include/plog/Logger.h
    include/plog/Record.h
    include/plog/Severity.h
    include/plog/Util.h
    include/plog/WinApi.h
    include/plog/Appenders/AndroidAppender.h
    include/plog/Appenders/ColorConsoleAppender.h
    include/plog/Appenders/ConsoleAppender.h
    include/plog/Appenders/DebugOutputAppender.h
    include/plog/Appenders/EventLogAppender.h
    include/plog/Appenders/IAppender.h
    include/plog/Appenders/RollingFileAppender.h
    include/plog/Converters/NativeEOLConverter.h
    include/plog/Converters/UTF8Converter.h
    include/plog/Formatters/CsvFormatter.h
    include/plog/Formatters/FuncMessageFormatter.h
    include/plog/Formatters/TxtFormatter.h
    include/plog/Formatters/MessageOnlyFormatter.h
    resources/styles/default.qss
    resources/res.qrc
)
