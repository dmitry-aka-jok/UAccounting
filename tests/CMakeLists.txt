cmake_minimum_required(VERSION 3.5)

find_package(Qt6 REQUIRED COMPONENTS Test QuickTest)

enable_testing()

include_directories("../main")
include_directories("../modules/Core")

## no need to copy around qml test files for shadow builds - just set the respective define
add_definitions(-DQUICK_TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/qml")


add_executable(tst_qml tst_qml.cpp)


#qt_add_qml_module(tst_qml
#    URI tst_qml
#    VERSION 1.0
#    QML_FILES qml/tst_sql.qml
#)


target_link_libraries(tst_qml PRIVATE Qt6::QuickTest Core)


add_executable(tst_asqltablemodel tst_asqltablemodel.cpp)
target_link_libraries(tst_asqltablemodel PRIVATE Qt6::Test Core)

add_executable(tst_guitablemodel tst_guitablemodel.cpp)
target_link_libraries(tst_guitablemodel PRIVATE Qt6::Test Core)



