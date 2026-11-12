############################################
## For mingw530_32 define PLATFORM_VER=32 ##
## For mingw730_64 define PLATFORM_VER=64 ##
############################################
win32:PLATFORM_VER=64
unix :PLATFORM_VER=64

QT += core gui opengl serialport network
CONFIG += c++17 thread

TARGET = CorsairTerminalV
TEMPLATE = app

include(build_counter.pri)

DEFINES += QT_DEPRECATED_WARNINGS
CONFIG(debug, debug|release):  DEFINES += DEBUG

CONFIG(release, debug|release) {
    CONFIG += optimize_full
}

QMAKE_CXXFLAGS += -fopenmp
QMAKE_LFLAGS += -fopenmp
DEFINES += OMP_EN=1 OMP_THR=4


## =========== VLX STREAM TYPE ===============

## vlx_stream_type_rtp_mpeg_ts
## vlx_stream_type_srt_mpeg_ts

CONFIG += vlx_stream_type_rtp_mpeg_ts

## ===========================================

INCLUDEPATH += $$PWD/src
INCLUDEPATH += $$PWD/src/vlx_common

SOURCES += \
    src/vlx_common/vlx_cmd_info_dialog.cpp \
    src/vlx_common/vlx_display_parameters_dialog.cpp \
    src/vlx_common/vlx_settings.cpp \
    src/device_parts/channel/cpuchannel.cpp \
    src/device_parts/vlx/vlx_media_parameters_dialog.cpp \
    src/device_parts/vlx/vlx_media/vlx_av_data_manager.cpp \
    src/device_parts/vlx/vlx_media/vlx_av_decoder.cpp \
    src/device_parts/vlx/vlx_media/vlx_av_input_demuxer.cpp \
    src/device_parts/vlx/vlx_stream_channel.cpp \
    src/device_parts/vlx/vlx_telemetry_channel.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/controller.cpp \
    src/pluggable_controls/vpc_wb/vpcwb.cpp \
    src/pluggable_controls/vpc_wb/vpcwb_joystick.cpp \
    src/pluggable_controls/vpc_wb/vpcwb_joystick_internal.cpp \
    src/tools/CorsairPainter.cpp \
    src/tools/JoystickWidget.cpp \
    src/tools/gsp_initialization.cpp \
    src/vlx_common/vlx_virtual_joysick_form.cpp \
    src/vlx_common/vlx_virtual_joystick_hats_form.cpp \
    src/vlx_input_control/vlx_input_cmd.cpp \
    src/vlx_input_control/vlx_input_joystick_mapper.cpp \
    src/vlx_input_control/vlx_input_joystick_mapping_form.cpp \
    src/vlx_input_control/vlx_input_mapping.cpp
HEADERS += \
    src/vlx_common/vlx_cmd_info_dialog.h \
    src/vlx_common/vlx_display_parameters_dialog.h \
    src/vlx_common/vlx_settings.h \
    src/device_parts/channel/cpuchannel.h \
    src/device_parts/vlx/vlx_media_parameters_dialog.h \
    src/device_parts/vlx/vlx_media/vlx_av.h \
    src/device_parts/vlx/vlx_media/vlx_av_common.h \
    src/device_parts/vlx/vlx_media/vlx_av_data_manager.h \
    src/device_parts/vlx/vlx_media/vlx_av_decoder.h \
    src/device_parts/vlx/vlx_media/vlx_av_input_data.h \
    src/device_parts/vlx/vlx_media/vlx_av_input_demuxer.h \
    src/device_parts/vlx/vlx_media/vlx_av_input_stream.h \
    src/device_parts/vlx/vlx_stream_channel.h \
    src/device_parts/cmd/vdb_registers.h \
    src/dev_types.h \
    src/device_parts/vlx/vlx_telemetry_channel.h \
    src/mainwindow.h \
    src/controller.h \
    src/pluggable_controls/vpc_wb/vpcwb.h \
    src/pluggable_controls/vpc_wb/vpcwb_joystick.h \
    src/pluggable_controls/vpc_wb/vpcwb_joystick_internal.h \
    src/tools/CorsairPainter.hpp \
    src/tools/JoystickWidget.hpp \
    src/tools/global_types.h \
    src/tools/gsp_initialization.h \
    src/vlx_common/vlx_virtual_joysick_form.h \
    src/vlx_common/vlx_virtual_joystick_hats_form.h \
    src/vlx_input_control/vlx_input_cmd.hpp \
    src/vlx_input_control/vlx_input_joystick_mapper.hpp \
    src/vlx_input_control/vlx_input_joystick_mapping_form.hpp \
    src/vlx_input_control/vlx_input_mapping.hpp
FORMS += \
    src/device_parts/vlx/vlx_media_parameters_dialog.ui \
    src/mainwindow.ui \
    src/vlx_common/vlx_cmd_info_dialog.ui \
    src/vlx_common/vlx_display_parameters_dialog.ui \
    src/vlx_common/vlx_virtual_joysick_form.ui \
    src/vlx_common/vlx_virtual_joystick_hats_form.ui \
    src/vlx_input_control/vlx_input_joystick_mapping_form.ui
RESOURCES += \
    resource/corsairterminal_resources.qrc
DISTFILES += \
    build_counter.pri \
    resource/QSS_master/AMOLED.qss \
    resource/QSS_master/Aqua.qss \
    resource/QSS_master/ConsoleStyle.qss \
    resource/QSS_master/ElegantDark.qss \
    resource/QSS_master/LICENSE \
    resource/QSS_master/MacOS.qss \
    resource/QSS_master/ManjaroMix.qss \
    resource/QSS_master/MaterialDark.qss \
    resource/QSS_master/NeonButtons.qss \
    resource/QSS_master/QSS_IMG/go-down-symbolic.symbolic.png \
    resource/QSS_master/QSS_IMG/go-next-symbolic.symbolic.png \
    resource/QSS_master/QSS_IMG/go-previous-symbolic.symbolic.png \
    resource/QSS_master/QSS_IMG/go-up-symbolic.symbolic.png \
    resource/QSS_master/QSS_IMG/object-select-symbolic.symbolic.png \
    resource/QSS_master/README.md \
    resource/QSS_master/Ubuntu.qss \
    resource/corsairterminal.rc \
    resource/docs/help.html \
    resource/docs/about.html \
    resource/docs/style/main.css
RC_FILE = resource/corsairterminal.rc
#---------------------------------------------------------------------------
#----- Platform Version
#---------------------------------------------------------------------------
win32:PLATFORM = win$$PLATFORM_VER
unix:PLATFORM  = unix$$PLATFORM_VER
#---------------------------------------------------------------------------
#----- Channels Sources/Headers/Forms
#---------------------------------------------------------------------------
SOURCES += \
    src/device_parts/channel/abstractchannel.cpp \
    src/device_parts/channel/tvrgbchannel.cpp \
    src/device_parts/channel/tvgraychannel.cpp \
    src/device_parts/channel/swirchannel.cpp \
    src/device_parts/channel/mwirchannel.cpp \
    src/device_parts/channel/testchannel.cpp \
    src/device_parts/channel/fusionchannel.cpp \
    src/device_parts/channel/tvgraymwirchannel.cpp
HEADERS += \
    src/device_parts/channel/abstractchannel.h \
    src/device_parts/channel/tvrgbchannel.h \
    src/device_parts/channel/tvgraychannel.h \
    src/device_parts/channel/swirchannel.h \
    src/device_parts/channel/mwirchannel.h \
    src/device_parts/channel/testchannel.h \
    src/device_parts/channel/fusionchannel.h \
    src/device_parts/channel/tvgraymwirchannel.h
#---------------------------------------------------------------------------
#----- Tools Sources/Headers/Forms
#---------------------------------------------------------------------------
SOURCES += \
    src/tools/framegenerator.cpp \
    src/tools/imgp.cpp \
    src/tools/drawp.cpp \
    src/tools/logger.cpp \
    src/tools/resource.cpp \
    src/tools/fusionalgorithm.cpp \
    src/tools/videoregistrator.cpp \
    src/tools/autofocusmodel.cpp
HEADERS += \
    src/tools/framegenerator.h \
    src/tools/imgp.h \
    src/tools/drawp.h \
    src/tools/logger.h \
    src/tools/resource.h \
    src/tools/fusionalgorithm.h \
    src/tools/videoregistrator.h \
    src/tools/meanvalue.h
    src/tools/autofocusmodel.h
#---------------------------------------------------------------------------
#----- Pluggable controls Sources/Headers/Forms
#---------------------------------------------------------------------------
SOURCES += src/pluggable_controls/corsairkeyfilter.cpp \
           src/pluggable_controls/joystick.cpp \
           src/pluggable_controls/panel.cpp \

HEADERS += src/pluggable_controls/corsairkeyfilter.h \
           src/pluggable_controls/joystick.h \
           src/pluggable_controls/panel.h
#---------------------------------------------------------------------------
#----- GSP Sources/Headers/Forms
#---------------------------------------------------------------------------
SOURCES += \
    src/device_parts/gsp/gspcontroller.cpp \
    src/device_parts/gsp/gsppidcontroller.cpp
HEADERS += \
    src/device_parts/gsp/gspcontroller.h \
    src/device_parts/gsp/gsppidcontroller.h
#---------------------------------------------------------------------------
#----- MV Instances Sources/Headers/Forms
#---------------------------------------------------------------------------
SOURCES += \
    src/program_settings/model/ethmodel.cpp \
    src/aids/model/telemetrymodel.cpp \
    src/device_parts/model/tvctrlmodel.cpp \
    src/device_parts/model/swirctrlmodel.cpp \
    src/device_parts/model/mwirctrlmodel.cpp \
    src/device_parts/model/gspctrlmodel.cpp \
    src/device_parts/model/fusionctrlmodel.cpp \
    src/program_settings/model/terminalsettingsmodel.cpp \
    src/device_parts/model/highlightermodel.cpp \
    src/program_settings/view/ethview.cpp \
    src/aids/view/telemetryview.cpp \
    src/device_parts/view/tvctrlview.cpp \
    src/device_parts/view/swirctrlview.cpp \
    src/device_parts/view/mwirctrlview.cpp \
    src/device_parts/view/gspctrlview.cpp \
    src/device_parts/view/fusionctrlview.cpp \
    src/program_settings/view/terminalsettingsview.cpp \
    src/aids/view/vdbMsgSenderView.cpp \
    src/device_parts/view/highlighterview.cpp
HEADERS += \
    src/device_parts/model/tvctrlmodel.h \
    src/device_parts/model/swirctrlmodel.h \
    src/device_parts/model/mwirctrlmodel.h \
    src/device_parts/model/gspctrlmodel.h \
    src/device_parts/model/fusionctrlmodel.h \
    src/device_parts/model/highlightermodel.h \
    src/aids/model/telemetrymodel.h \
    src/program_settings/model/ethmodel.h \
    src/program_settings/model/terminalsettingsmodel.h \
    src/device_parts/view/tvctrlview.h \
    src/device_parts/view/swirctrlview.h \
    src/device_parts/view/mwirctrlview.h \
    src/device_parts/view/gspctrlview.h \
    src/device_parts/view/fusionctrlview.h \
    src/device_parts/view/highlighterview.h \
    src/aids/view/telemetryview.h \
    src/aids/view/vdbMsgSenderView.h \
    src/program_settings/view/ethview.h \
    src/program_settings/view/terminalsettingsview.h

FORMS += \
    src/device_parts/view/tvctrlview.ui \
    src/device_parts/view/swirctrlview.ui \
    src/device_parts/view/mwirctrlview.ui \
    src/device_parts/view/gspctrlview.ui \
    src/device_parts/view/fusionctrlview.ui \
    src/device_parts/view/highlighterview.ui \
    src/aids/view/telemetryview.ui \
    src/aids/view/vdbMsgSenderView.ui \
    src/program_settings/view/ethview.ui \
    src/program_settings/view/terminalsettingsview.ui
#---------------------------------------------------------------------------
#----- Ethernet Interface Sources/Headers
#---------------------------------------------------------------------------
include(modules/eth/ETH.pri)
#---------------------------------------------------------------------------
#----- CMD
#---------------------------------------------------------------------------
SOURCES += \
    src/device_parts/cmd/cmd.cpp \
    src/device_parts/cmd/cmdmanager.cpp \
    src/device_parts/cmd/telemetry.cpp \
    src/device_parts/cmd/packet.cpp
HEADERS += \
    src/device_parts/cmd/cmd.h \
    src/device_parts/cmd/cmdmanager.h \
    src/device_parts/cmd/telemetry.h \
    src/device_parts/cmd/packet.h
#---------------------------------------------------------------------------
#----- Video Widget Defines/Sources/Headers/Forms
#---------------------------------------------------------------------------
SOURCES += src/tools/video/video.cpp
HEADERS += src/tools/video/video.h
FORMS   += src/tools/video/video.ui
#---------------------------------------------------------------------------
#----- OpenCV
#---------------------------------------------------------------------------
win32 {
OPENCV_VER = 412
MSYS64Root = "w:/msys64/mingw64"
OPENCV_SDK = $${MSYS64Root}/include/opencv4
INCLUDEPATH += $${OPENCV_SDK}
INCLUDEPATH += $${MSYS64Root}/include

LIBS += -L$$MSYS64Root/bin -lopencv_core-$${OPENCV_VER}
LIBS += -L$$MSYS64Root/bin -lopencv_imgproc-$${OPENCV_VER}
LIBS += -L$$MSYS64Root/bin -lopencv_imgcodecs-$${OPENCV_VER}
LIBS += -L$$MSYS64Root/bin -lopencv_videoio-$${OPENCV_VER}
# LIBS += -L$$OPENCV_SDK/mingw64/bin -lopencv_videoio_ffmpeg$${OPENCV_VER}_64
LIBS += -L$${MSYS64Root}/lib -lavformat -lavcodec -lavutil -lswscale
}

unix {

CONFIG += opencv4 ffmpeg
CONFIG(opencv4) {
    packagesExist(opencv4) {
        CONFIG += link_pkgconfig
        PKGCONFIG += opencv4

        DEFINES += OPENCV4
        INCLUDEPATH += /usr/include/opencv4
    }

    !packagesExist(opencv4) {
        message("opencv4 not found via pkg-config, using manual paths")
        INCLUDEPATH += /usr/include/opencv4
        LIBS += -L/usr/lib/opencv4 \
                -lopencv_core \
                -lopencv_imgproc \
                -lopencv_highgui \
                -lopencv_videoio \
                -lopencv_imgcodecs
    }

}


CONFIG(ffmpeg) {
    DEFINES += FFMPEG
    INCLUDEPATH += /usr/include/x86_64-linux-gnu
    LIBS += -L/usr/lib/x86_64-linux-gnu \
        -lavcodec \
        -lavformat \
        -lavutil \
        -lswscale \
        -lswresample
}


}
#---------------------------------------------------------------------------
#----- OpenCV
#---------------------------------------------------------------------------


CONFIG(vlx_stream_type_rtp_mpeg_ts) {
    DEFINES += VLX_STREAM_TYPE_RTP_MPEG_TS
}else:CONFIG(vlx_stream_type_srt_mpeg_ts) {
    DEFINES += VLX_STREAM_TYPE_SRT_MPEG_TS
}else{
    error("vlx_stream_type... missing on CONFIG. Expected on of: vlx_stream_type_rtp_mpeg_ts | vlx_stream_type_srt_mpeg_ts")
}

#---------------------------------------------------------------------------
#----- QMV
#---------------------------------------------------------------------------
include(modules/QMV/QMV.pri)
#---------------------------------------------------------------------------
#----- ETLD LIB
#---------------------------------------------------------------------------
INCLUDEPATH += $$PWD/external_libs/ETLD/include
CONFIG(debug, debug|release): LIBS += -L$$PWD/external_libs/ETLD/lib/$${PLATFORM} -lETLDd
CONFIG(release, debug|release): LIBS += -L$$PWD/external_libs/ETLD/lib/$${PLATFORM} -lETLD
#---------------------------------------------------------------------------
#----- SDL LIBs
#---------------------------------------------------------------------------
win32 {
MSYS64Root = "w:/msys64/mingw64"
INCLUDEPATH += $${MSYS64Root}/include
LIBS += -L$$MSYS64Root/bin -lSDL2
}
unix {
INCLUDEPATH += /usr/include/SDL2
LIBS += -lSDL2
}
#---------------------------------------------------------------------------
#----- Install RUN
#---------------------------------------------------------------------------
win32 {
dll_opencv_rt.path = $${OUT_PWD}
dll_opencv_rt.files += $${OPENCV_SDK}/x$${PLATFORM_VER}/mingw/bin/libopencv_core$${OPENCV_VER}.dll
dll_opencv_rt.files += $${OPENCV_SDK}/x$${PLATFORM_VER}/mingw/bin/libopencv_imgproc$${OPENCV_VER}.dll
dll_opencv_rt.files += $${OPENCV_SDK}/x$${PLATFORM_VER}/mingw/bin/libopencv_imgcodecs$${OPENCV_VER}.dll
dll_opencv_rt.files += $${OPENCV_SDK}/x$${PLATFORM_VER}/mingw/bin/libopencv_videoio$${OPENCV_VER}.dll
dll_opencv_rt.files += $${OPENCV_SDK}/x$${PLATFORM_VER}/mingw/bin/opencv_videoio_ffmpeg$${OPENCV_VER}_64.dll

dll_etld_rt.path = $${OUT_PWD}
CONFIG(debug, debug|release): dll_etld_rt.files += $${PWD}/external_libs/ETLD/lib/$${PLATFORM}/ETLDd.dll
CONFIG(release, debug|release): dll_etld_rt.files += $${PWD}/external_libs/ETLD/lib/$${PLATFORM}/ETLD.dll

dll_sdl_rt.path = $${OUT_PWD}
dll_sdl_rt.files += $${PWD}/external_libs/SDL/lib/$${PLATFORM}/SDL2.dll

dll_pcap_rt.path = $${OUT_PWD}
dll_pcap_rt.files += $${PWD}/external_libs/PCAP/lib/$${PLATFORM}/wpcap.dll
dll_pcap_rt.files += $${PWD}/external_libs/PCAP/lib/$${PLATFORM}/Packet.dll

config_rt.path = $${OUT_PWD}
config_rt.files += $${PWD}/resource/etld_config.ini

INSTALLS += dll_opencv_rt dll_etld_rt dll_sdl_rt config_rt
}
unix {
config_rt.path = $${OUT_PWD}
config_rt.files += $${PWD}/resource/etld_config.ini

INSTALLS += config_rt
}
#---------------------------------------------------------------------------
#----- Install BIN
#---------------------------------------------------------------------------
win32 {
CONFIG(release, debug|release) {
    MINGW_VER = 81
    MINGW_Q = $${PLATFORM_VER}
    QT_DIR = $$[QT_INSTALL_ARCHDATA]/../..
    OUT_DIR = $${PWD}/../_$${TARGET}_$${PLATFORM}

    dll_bin.path = $${OUT_DIR}
    dll_bin.files += $$[QT_INSTALL_BINS]/libgcc_s_dw2-1.dll
    dll_bin.files += $$[QT_INSTALL_BINS]/libgcc_s_seh-1.dll
    dll_bin.files += $$[QT_INSTALL_BINS]/libstdc++-6.dll
    dll_bin.files += $$[QT_INSTALL_BINS]/libwinpthread-1.dll
    dll_bin.files += $$[QT_INSTALL_BINS]/Qt5Core.dll
    dll_bin.files += $$[QT_INSTALL_BINS]/Qt5Gui.dll
    dll_bin.files += $$[QT_INSTALL_BINS]/Qt5Widgets.dll
    dll_bin.files += $$[QT_INSTALL_BINS]/Qt5Multimedia.dll
    dll_bin.files += $$[QT_INSTALL_BINS]/Qt5Network.dll
    dll_bin.files += $$[QT_INSTALL_BINS]/Qt5OpenGL.dll
    dll_bin.files += $$[QT_INSTALL_BINS]/Qt5SerialPort.dll
    dll_bin.files += $$[QT_INSTALL_BINS]/Qt5PrintSupport.dll

    dll_bin.files += $${QT_DIR}/Tools/mingw$${MINGW_VER}0_$${MINGW_Q}/bin/libgomp-1.dll

    dll_platforms.path = $${OUT_DIR}/platforms
    dll_platforms.files += $$[QT_INSTALL_PLUGINS]/platforms/qminimal.dll
    dll_platforms.files += $$[QT_INSTALL_PLUGINS]/platforms/qoffscreen.dll
    dll_platforms.files += $$[QT_INSTALL_PLUGINS]/platforms/qwindows.dll

    dll_styles.path = $${OUT_DIR}/styles
    dll_styles.files += $$[QT_INSTALL_PLUGINS]/styles/qwindowsvistastyle.dll

    dll_imageformats.path = $${OUT_DIR}/imageformats
    dll_imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qgif.dll
    dll_imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qicns.dll
    dll_imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qico.dll
    dll_imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qjpeg.dll
    dll_imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qsvg.dll
    dll_imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qtga.dll
    dll_imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qtiff.dll
    dll_imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qwbmp.dll
    dll_imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qwebp.dll

    dll_opencv.path = $${OUT_DIR}
    dll_opencv.files += $${OPENCV_SDK}/x$${PLATFORM_VER}/mingw/bin/libopencv_core$${OPENCV_VER}.dll
    dll_opencv.files += $${OPENCV_SDK}/x$${PLATFORM_VER}/mingw/bin/libopencv_imgproc$${OPENCV_VER}.dll
    dll_opencv.files += $${OPENCV_SDK}/x$${PLATFORM_VER}/mingw/bin/libopencv_imgcodecs$${OPENCV_VER}.dll
    dll_opencv.files += $${OPENCV_SDK}/x$${PLATFORM_VER}/mingw/bin/libopencv_videoio$${OPENCV_VER}.dll
    dll_opencv.files += $${OPENCV_SDK}/x$${PLATFORM_VER}/mingw/bin/opencv_ffmpeg$${OPENCV_VER}_64.dll

    dll_etld.path = $${OUT_DIR}
    dll_etld.files += $${PWD}/external_libs/ETLD/lib/$${PLATFORM}/ETLD.dll

    dll_sdl.path = $${OUT_DIR}
    dll_sdl.files += $${PWD}/external_libs/SDL/lib/$${PLATFORM}/SDL2.dll

    dll_pcap.path = $${OUT_DIR}
    dll_pcap.files += $${PWD}/external_libs/PCAP/lib/$${PLATFORM}/wpcap.dll
    dll_pcap.files += $${PWD}/external_libs/PCAP/lib/$${PLATFORM}/Packet.dll

    config.path = $${OUT_DIR}
    config.files += $${PWD}/resource/etld_config.ini

    target.path = $${OUT_DIR}

    INSTALLS += target dll_bin dll_platforms dll_styles dll_imageformats dll_opencv dll_etld dll_sdl config
}
}
unix {
CONFIG(release, debug|release) {
OUT_DIR = $${PWD}/../_$${TARGET}_$${PLATFORM}

config.path = $${OUT_DIR}
config.files += $${PWD}/resource/etld_config.ini

target.path = $${OUT_DIR}

INSTALLS += target config
}
}
