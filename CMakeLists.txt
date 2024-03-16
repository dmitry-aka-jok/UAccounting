cmake_minimum_required(VERSION 3.20)

project(UAccounting VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)


find_package(Qt6 6.5 REQUIRED COMPONENTS Quick Sql)

qt_standard_project_setup(REQUIRES 6.5)

add_subdirectory(modules)
add_subdirectory(tests)

qt_add_executable(UAC
    main/main.cpp
)

include_directories("../modules/Core")

qt_add_qml_module(UAC
    URI UAccounting
    VERSION 1.0
    QML_FILES main/Main.qml
    SOURCES main/build_defs.h
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(UAC PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appUAccounting
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(UAC
    PRIVATE Qt6::Quick Qt6::Sql Core
)

include(GNUInstallDirs)
install(TARGETS UAC
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
