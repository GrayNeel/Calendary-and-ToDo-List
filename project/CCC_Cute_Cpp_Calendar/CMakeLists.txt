cmake_minimum_required(VERSION 3.5)

project(CCC_Cute_Cpp_Calendar VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.2.0/mingw81_64/lib/cmake")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

# Include the QT Network library for API calls
find_package(Qt6 COMPONENTS Network REQUIRED)

# Include the QT XML Library for APIs
find_package(Qt6 COMPONENTS Xml REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        gui/mainwindow.cpp
        gui/mainwindow.h
        gui/mainwindow.ui
        gui/dialog.cpp
        gui/dialog.h
        gui/dialog.ui
        logic/client.h
        logic/client.cpp
        logic/calendar.h
        logic/calendar.cpp
        logic/event.h
        logic/event.cpp
        logic/event_api.cpp
        logic/todo.h
        logic/todo.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CCC_Cute_Cpp_Calendar
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET CCC_Cute_Cpp_Calendar APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(CCC_Cute_Cpp_Calendar SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(CCC_Cute_Cpp_Calendar
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(CCC_Cute_Cpp_Calendar PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
# Include the QT Network library for API calls
target_link_libraries(CCC_Cute_Cpp_Calendar PRIVATE Qt6::Network)
# Include the QT XML Library for APIs
target_link_libraries(CCC_Cute_Cpp_Calendar PRIVATE Qt6::Xml)

set_target_properties(CCC_Cute_Cpp_Calendar PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CCC_Cute_Cpp_Calendar)
endif()
