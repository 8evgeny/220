#--------------------------------------------------------------------------------------------------
#--------------------------------------------------------------------------------------------------
# Подключение модулей Qt
#if(!equals(QT_MAJOR_VERSION, 5):greaterThan(QT_MINOR_VERSION, 6))|greaterThan(QT_MAJOR_VERSION, 5) {
#  error( "Unsupported version QMV" )
#}

LIST_LIB = widgets
for(LIB, LIST_LIB): {
  !contains(QT, $$LIB) {
    QT += $$LIB
  }
}
#--------------------------------------------------------------------------------------------------
DEFINES += QT_DEPRECATED_WARNINGS SUBJECT_PROJECT

CONFIG(debug, debug|release): DEFINES += DEBUG

DEFINES += \
    VER_QMV="2.0"

#--------------------------------------------------------------------------------------------------
INCLUDEPATH += $$PWD
#--------------------------------------------------------------------------------------------------
SOURCES += \
    $$PWD/qmv/qmvmodel.cpp \
    $$PWD/qmv/qmvview.cpp \
    $$PWD/qmv/property/property.cpp \
    $$PWD/qmv/property/valueproperty.cpp \
    $$PWD/qmv/property/intvalueproperty.cpp \
    $$PWD/qmv/property/doublevalueproperty.cpp \
    $$PWD/qmv/property/booleanvalueproperty.cpp \
    $$PWD/qmv/property/stringvalueproperty.cpp \
    $$PWD/qmv/property/cmdproperty.cpp \
    $$PWD/qmv/property/comboboxproperty.cpp \
    $$PWD/qmv/property/textvalueproperty.cpp \
    $$PWD/qmv/widget/keyfilter.cpp \
    $$PWD/qmv/widget/enterkeyfilter.cpp \
    $$PWD/qmv/widget/inoutwidget.cpp \
    $$PWD/qmv/widget/setgetwidget.cpp \
    $$PWD/qmv/widget/inoutcheckboxwidget.cpp \
    $$PWD/qmv/widget/inoutcomboboxwidget.cpp \
    $$PWD/qmv/widget/inoutcmdwidget.cpp \
    $$PWD/qmv/widget/inoutsliderwidget.cpp \
    $$PWD/qmv/widget/outplaintextwidget.cpp \
    $$PWD/qmv/widget/inoutuintwidget.cpp \
    $$PWD/qmv/widget/inoutintwidget.cpp \
    $$PWD/qmv/widget/inoutufloatwidget.cpp \
    $$PWD/qmv/widget/inoutfloatwidget.cpp \
    $$PWD/qmv/widget/inoutbinwidget.cpp \
    $$PWD/qmv/widget/inoutstringwidget.cpp \
    $$PWD/qmv/widget/inouthexwidget.cpp \
    $$PWD/qmv/widget/waitinggifmoviesingleton.cpp \
    $$PWD/qmv/tools/accessor.cpp \
    $$PWD/qmv/tools/help.cpp \
    $$PWD/qmv/tools/about.cpp
#--------------------------------------------------------------------------------------------------
HEADERS += \
    $$PWD/qmv/qmv_global.h \
    $$PWD/qmv/qmvmodel.h \
    $$PWD/qmv/qmvview.h \
    $$PWD/qmv/property/property.h \
    $$PWD/qmv/property/valueproperty.h \
    $$PWD/qmv/property/intvalueproperty.h \
    $$PWD/qmv/property/doublevalueproperty.h \
    $$PWD/qmv/property/booleanvalueproperty.h \
    $$PWD/qmv/property/stringvalueproperty.h \
    $$PWD/qmv/property/cmdproperty.h \
    $$PWD/qmv/property/comboboxproperty.h \
    $$PWD/qmv/property/textvalueproperty.h \
    $$PWD/qmv/widget/keyfilter.h \
    $$PWD/qmv/widget/enterkeyfilter.h \
    $$PWD/qmv/widget/inoutwidget.h \
    $$PWD/qmv/widget/setgetwidget.h \
    $$PWD/qmv/widget/inoutcheckboxwidget.h \
    $$PWD/qmv/widget/inoutcomboboxwidget.h \
    $$PWD/qmv/widget/inoutcmdwidget.h \
    $$PWD/qmv/widget/inoutsliderwidget.h \
    $$PWD/qmv/widget/outplaintextwidget.h \
    $$PWD/qmv/widget/inoutuintwidget.h \
    $$PWD/qmv/widget/inoutintwidget.h \
    $$PWD/qmv/widget/inoutufloatwidget.h \
    $$PWD/qmv/widget/inoutfloatwidget.h \
    $$PWD/qmv/widget/inoutbinwidget.h \
    $$PWD/qmv/widget/inoutstringwidget.h \
    $$PWD/qmv/widget/inouthexwidget.h \
    $$PWD/qmv/widget/waitinggifmoviesingleton.h \
    $$PWD/qmv/tools/accessor.h \
    $$PWD/qmv/tools/help.h \
    $$PWD/qmv/tools/about.h
FORMS += \
    $$PWD/qmv/widget/inoutwidget.ui \
    $$PWD/qmv/tools/help.ui \
    $$PWD/qmv/tools/about.ui
RESOURCES += \
    $$PWD/qmv_resources.qrc
DISTFILES += \
    $$PWD/qmv_resources.qrc
#---------------------------------------------------------------------------
#----- Install LIBQMV
#---------------------------------------------------------------------------
#win32:PLATFORM = win$$PLATFORM_VER
#unix:PLATFORM = unix$$PLATFORM_VER
#OUT = $$PWD/../_$$TARGET

#headers.path = $$OUT/include/qmv
#headers.files += $$PWD/qmv/*.h
#headers_tools.path = $$OUT/include/qmv/tools
#headers_tools.files += $$PWD/qmv/tools/*.h
#headers_property.path = $$OUT/include/qmv/property
#headers_property.files += $$PWD/qmv/property/*.h
#headers_widget.path = $$OUT/include/qmv/widget
#headers_widget.files += $$PWD/qmv/widget/*.h

#target.path = $$OUT/lib/$$PLATFORM
#CONFIG(debug, debug|release) {
#win32:TARGET = $$join(TARGET,,,d)
#unix:TARGET = $$join(TARGET,,,d)
#}

#INSTALLS += target headers headers_tools headers_property headers_widget
