TARGET = TabbedEditor
TEMPLATE = lib

include(../../qtcreatorplugin.pri)

DEFINES += TABBEDEDITOR_LIBRARY

# TabbedEditor files

SUBDIRS += \
    tabbededitor.pro

DISTFILES += \
    CMakeLists.txt \
    CMakeLists.txt.user \
    LICENSE \
    README.md \
    TabbedEditor.json.in \
    resources/icons/close_button_dark.png \
    resources/icons/close_button_light_grey.png \
    resources/styles/default.qss \
    tab.png

HEADERS += \
    include/plog/Appenders/AndroidAppender.h \
    include/plog/Appenders/ColorConsoleAppender.h \
    include/plog/Appenders/ConsoleAppender.h \
    include/plog/Appenders/DebugOutputAppender.h \
    include/plog/Appenders/EventLogAppender.h \
    include/plog/Appenders/IAppender.h \
    include/plog/Appenders/RollingFileAppender.h \
    include/plog/Converters/NativeEOLConverter.h \
    include/plog/Converters/UTF8Converter.h \
    include/plog/Formatters/CsvFormatter.h \
    include/plog/Formatters/FuncMessageFormatter.h \
    include/plog/Formatters/MessageOnlyFormatter.h \
    include/plog/Formatters/TxtFormatter.h \
    include/plog/Init.h \
    include/plog/Log.h \
    include/plog/Logger.h \
    include/plog/Record.h \
    include/plog/Severity.h \
    include/plog/Util.h \
    include/plog/WinApi.h \
    tabbar.h \
    tabbededitor.h \
    tabbededitor_global.h \
    tabbededitorconstants.h

SOURCES += \
    tabbar.cpp \
    tabbededitor.cpp

RESOURCES += \
    resources/res.qrc
