QT += widgets

TARGET = QMV
TEMPLATE = lib

unix:CONFIG += staticlib

DEFINES += QMV_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG(debug, debug|release): DEFINES += DEBUG

DEFINES += \
    VER="2.0"

win32:PLATFORM_VER=64

SOURCES += \
    qmv/qmvmodel.cpp \
    qmv/qmvview.cpp \
    qmv/property/property.cpp \
    qmv/property/valueproperty.cpp \
    qmv/property/intvalueproperty.cpp \
    qmv/property/doublevalueproperty.cpp \
    qmv/property/booleanvalueproperty.cpp \
    qmv/property/stringvalueproperty.cpp \
    qmv/property/cmdproperty.cpp \
    qmv/property/comboboxproperty.cpp \
    qmv/property/textvalueproperty.cpp \
    qmv/widget/keyfilter.cpp \
    qmv/widget/enterkeyfilter.cpp \
    qmv/widget/inoutwidget.cpp \
    qmv/widget/setgetwidget.cpp \
    qmv/widget/inoutcheckboxwidget.cpp \
    qmv/widget/inoutcomboboxwidget.cpp \
    qmv/widget/inoutcmdwidget.cpp \
    qmv/widget/inoutsliderwidget.cpp \
    qmv/widget/outplaintextwidget.cpp \
    qmv/widget/inoutuintwidget.cpp \
    qmv/widget/inoutintwidget.cpp \
    qmv/widget/inoutufloatwidget.cpp \
    qmv/widget/inoutfloatwidget.cpp \
    qmv/widget/inoutbinwidget.cpp \
    qmv/widget/inoutstringwidget.cpp \
    qmv/widget/inouthexwidget.cpp \
    qmv/widget/waitinggifmoviesingleton.cpp \
    qmv/tools/resource.cpp \
    qmv/tools/accessor.cpp \
    qmv/tools/help.cpp \
    qmv/tools/about.cpp \
    qmv/tools/logger.cpp
HEADERS += \
    qmv/qmv_global.h \
    qmv/qmvmodel.h \
    qmv/qmvview.h \
    qmv/property/property.h \
    qmv/property/valueproperty.h \
    qmv/property/intvalueproperty.h \
    qmv/property/doublevalueproperty.h \
    qmv/property/booleanvalueproperty.h \
    qmv/property/stringvalueproperty.h \
    qmv/property/cmdproperty.h \
    qmv/property/comboboxproperty.h \
    qmv/property/textvalueproperty.h \
    qmv/widget/keyfilter.h \
    qmv/widget/enterkeyfilter.h \
    qmv/widget/inoutwidget.h \
    qmv/widget/setgetwidget.h \
    qmv/widget/inoutcheckboxwidget.h \
    qmv/widget/inoutcomboboxwidget.h \
    qmv/widget/inoutcmdwidget.h \
    qmv/widget/inoutsliderwidget.h \
    qmv/widget/outplaintextwidget.h \
    qmv/widget/inoutuintwidget.h \
    qmv/widget/inoutintwidget.h \
    qmv/widget/inoutufloatwidget.h \
    qmv/widget/inoutfloatwidget.h \
    qmv/widget/inoutbinwidget.h \
    qmv/widget/inoutstringwidget.h \
    qmv/widget/inouthexwidget.h \
    qmv/widget/waitinggifmoviesingleton.h \
    qmv/tools/resource.h \
    qmv/tools/accessor.h \
    qmv/tools/help.h \
    qmv/tools/about.h \
    qmv/tools/logger.h
FORMS += \
    qmv/widget/inoutwidget.ui \
    qmv/tools/help.ui \
    qmv/tools/about.ui \
    qmv/tools/logger.ui
RESOURCES += \
    qmv_resources.qrc
#---------------------------------------------------------------------------
#----- Install LIBQMV
#---------------------------------------------------------------------------
win32:PLATFORM = win$$PLATFORM_VER
unix:PLATFORM = unix$$PLATFORM_VER
OUT = $$PWD/../_$$TARGET

headers.path = $$OUT/include/qmv
headers.files += $$PWD/qmv/*.h
headers_tools.path = $$OUT/include/qmv/tools
headers_tools.files += $$PWD/qmv/tools/*.h
headers_property.path = $$OUT/include/qmv/property
headers_property.files += $$PWD/qmv/property/*.h
headers_widget.path = $$OUT/include/qmv/widget
headers_widget.files += $$PWD/qmv/widget/*.h

target.path = $$OUT/lib/$$PLATFORM
CONFIG(debug, debug|release) {
win32:TARGET = $$join(TARGET,,,d)
unix:TARGET = $$join(TARGET,,,d)
}

INSTALLS += target headers headers_tools headers_property headers_widget
