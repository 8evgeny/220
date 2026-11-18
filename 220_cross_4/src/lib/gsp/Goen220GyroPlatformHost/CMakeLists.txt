cmake_minimum_required(VERSION 3.14)

project( 
	Goen220GyroPlatformHost
	
	LANGUAGES 
            CXX
)



set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#   Поиск пакетов
find_package(QT
    NAMES
        Qt5
        Qt6

    COMPONENTS
        Core
        Network
        SerialPort

    REQUIRED    #   Прерывать работу CMAKE в случае отсутствия пакета
)


find_package(
	Qt${QT_VERSION_MAJOR}
	
    COMPONENTS
        Core
        Network
        SerialPort

    REQUIRED
)

if(QT_VERSION_MAJOR EQUAL 6)
    find_package( Qt6 COMPONENTS Scxml StateMachine REQUIRED)
endif()



set( CORE_SRC_DIR       ${CMAKE_CURRENT_SOURCE_DIR}/src/NCore)
set( NETWORK_SRC_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/src/NNetwork)
set( COMMANDS_SRC_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/src/NCommands)
set( DEVICES_SRC_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/src/NDevices)


add_library(
	${PROJECT_NAME} 
	
	STATIC
        #   PUBLIC INTERFACE
        include/Goen220GyroPlatformHost.h
        include/MemsBoardStatus.h
        include/MotorStatus.h


        #   PRIVATE
        src/Goen220GyroPlatformHost.cpp
        src/Goen220GyroPlatformHostPrivate.cpp
        src/Goen220GyroPlatformHostPrivate.h


        #	    CORE
        ${CORE_SRC_DIR}/CommandFactory.cpp
        ${CORE_SRC_DIR}/CommandFactory.h

        #       NETWORK
        ${NETWORK_SRC_DIR}/GyroPlatformProtocol.h

        ${NETWORK_SRC_DIR}/GyroPlatformDeviceCommunicator.cpp
        ${NETWORK_SRC_DIR}/GyroPlatformDeviceCommunicator.h

        #       COMMANDS
        ${COMMANDS_SRC_DIR}/SetModeCommand.cpp
        ${COMMANDS_SRC_DIR}/SetModeCommand.h

        ${COMMANDS_SRC_DIR}/ResultSetModeCommand.cpp
        ${COMMANDS_SRC_DIR}/ResultSetModeCommand.h

        ${COMMANDS_SRC_DIR}/GetVersionCommand.cpp
        ${COMMANDS_SRC_DIR}/GetVersionCommand.h

        ${COMMANDS_SRC_DIR}/ResultGetVersionCommand.cpp
        ${COMMANDS_SRC_DIR}/ResultGetVersionCommand.h

        ${COMMANDS_SRC_DIR}/GetStatusCommand.cpp
        ${COMMANDS_SRC_DIR}/GetStatusCommand.h

        ${COMMANDS_SRC_DIR}/ResultGetStatusCommand.cpp
        ${COMMANDS_SRC_DIR}/ResultGetStatusCommand.h

        ${COMMANDS_SRC_DIR}/SetControlPositionCommand.cpp
        ${COMMANDS_SRC_DIR}/SetControlPositionCommand.h

        ${COMMANDS_SRC_DIR}/ResultSetControlPositionCommand.cpp
        ${COMMANDS_SRC_DIR}/ResultSetControlPositionCommand.h

        ${COMMANDS_SRC_DIR}/SetControlSpeedCommand.cpp
        ${COMMANDS_SRC_DIR}/SetControlSpeedCommand.h

        ${COMMANDS_SRC_DIR}/ResultSetControlSpeedCommand.cpp
        ${COMMANDS_SRC_DIR}/ResultSetControlSpeedCommand.h

        ${COMMANDS_SRC_DIR}/SetControlTrackingCommand.cpp
        ${COMMANDS_SRC_DIR}/SetControlTrackingCommand.h

        ${COMMANDS_SRC_DIR}/ResultSetControlTrackingCommand.cpp
        ${COMMANDS_SRC_DIR}/ResultSetControlTrackingCommand.h

        #       DEVICES
        ${DEVICES_SRC_DIR}/Calculator.cpp
        ${DEVICES_SRC_DIR}/Calculator.h
)



# Публичные заголовки
target_include_directories(
    ${PROJECT_NAME}

    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)


#   Подключение каталогов с исходниками, расположенными вне папки проекта
include_directories( ${CORE_SRC_DIR})
include_directories( ${NETWORK_SRC_DIR})
include_directories( ${COMMANDS_SRC_DIR})
include_directories( ${DEVICES_SRC_DIR})



#   Подключение пакета к проекту
target_link_libraries(
	${PROJECT_NAME}
	
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
		Qt${QT_VERSION_MAJOR}::Network
		Qt${QT_VERSION_MAJOR}::SerialPort

        Goen220Vdb
)

if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries( ${PROJECT_NAME} PRIVATE Qt6::Scxml Qt6::StateMachine)
endif()



