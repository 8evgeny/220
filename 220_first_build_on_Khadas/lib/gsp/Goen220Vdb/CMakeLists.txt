cmake_minimum_required(VERSION 3.14)

project(Goen220Vdb LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#   Поиск пакетов
find_package(QT NAMES  Qt5 Qt6 COMPONENTS Core Network SerialPort REQUIRED )

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network SerialPort REQUIRED)

if(QT_VERSION_MAJOR EQUAL 6)
    find_package( Qt6 COMPONENTS Scxml StateMachine REQUIRED)
endif()

set( CORE_SRC_DIR       ${CMAKE_CURRENT_SOURCE_DIR}/src/NCore)
set( NETWORK_SRC_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/src/NNetwork)
set( COMMANDS_SRC_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/src/NCommands)

add_library(${PROJECT_NAME}
	STATIC
        #   PUBLIC INTERFACE
        #	    CORE
        ${CORE_SRC_DIR}/CNewThread.cpp
        ${CORE_SRC_DIR}/CNewThread.h
        ${CORE_SRC_DIR}/Message.cpp
        ${CORE_SRC_DIR}/Message.h
        ${CORE_SRC_DIR}/MessageParser.cpp
        ${CORE_SRC_DIR}/MessageParser.h
        ${CORE_SRC_DIR}/CDelimiterWithLenAndCrcMessageParser.cpp
        ${CORE_SRC_DIR}/CDelimiterWithLenAndCrcMessageParser.h

        #       NETWORK
        ${NETWORK_SRC_DIR}/Goen220Crc.cpp
        ${NETWORK_SRC_DIR}/Goen220Crc.h
        ${NETWORK_SRC_DIR}/DeviceCommunicator.cpp
        ${NETWORK_SRC_DIR}/DeviceCommunicator.h
        ${NETWORK_SRC_DIR}/CClientTcp.cpp
        ${NETWORK_SRC_DIR}/CClientTcp.h
        ${NETWORK_SRC_DIR}/SerialPort.cpp
        ${NETWORK_SRC_DIR}/SerialPort.h
        ${NETWORK_SRC_DIR}/VdbProtocol.h

        #       COMMANDS
        ${COMMANDS_SRC_DIR}/Command.cpp
        ${COMMANDS_SRC_DIR}/Command.h
        ${COMMANDS_SRC_DIR}/CommandExecutor.cpp
        ${COMMANDS_SRC_DIR}/CommandExecutor.h
        ${COMMANDS_SRC_DIR}/CommandDecorator.cpp
        ${COMMANDS_SRC_DIR}/CommandDecorator.h
        ${COMMANDS_SRC_DIR}/DelimiterAndCrc8CommandDecorator.cpp
        ${COMMANDS_SRC_DIR}/DelimiterAndCrc8CommandDecorator.h
        ${COMMANDS_SRC_DIR}/VdbCommand.cpp
        ${COMMANDS_SRC_DIR}/VdbCommand.h
        ${COMMANDS_SRC_DIR}/WriteRequestCommand.cpp
        ${COMMANDS_SRC_DIR}/WriteRequestCommand.h
        ${COMMANDS_SRC_DIR}/WriteResponseCommand.cpp
        ${COMMANDS_SRC_DIR}/WriteResponseCommand.h
        ${COMMANDS_SRC_DIR}/ReadRequestCommand.cpp
        ${COMMANDS_SRC_DIR}/ReadRequestCommand.h
        ${COMMANDS_SRC_DIR}/ReadResponseCommand.cpp
        ${COMMANDS_SRC_DIR}/ReadResponseCommand.h
        ${COMMANDS_SRC_DIR}/SetUnknownCommand.cpp
        ${COMMANDS_SRC_DIR}/SetUnknownCommand.h
        ${COMMANDS_SRC_DIR}/ResultSetUnknownCommand.cpp
        ${COMMANDS_SRC_DIR}/ResultSetUnknownCommand.h
        ${COMMANDS_SRC_DIR}/ResultStatusWithErrorCommand.cpp
        ${COMMANDS_SRC_DIR}/ResultStatusWithErrorCommand.h
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        LIBINTERFACE_STATIC
)

# Публичные заголовки
target_include_directories( ${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/NCore>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/NNetwork>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/NCommands>
        $<INSTALL_INTERFACE:NCore>
        $<INSTALL_INTERFACE:NNetwork>
        $<INSTALL_INTERFACE:NCommands>
)

#   Подключение каталогов с исходниками, расположенными вне папки проекта

#   Подключение пакета к проекту
target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::SerialPort
)

if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries( ${PROJECT_NAME} PRIVATE Qt6::Scxml Qt6::StateMachine)
endif()



