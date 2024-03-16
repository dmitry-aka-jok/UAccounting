
link_libraries(
    Qt6::Quick
    Qt6::Sql
)

#include_directories("../../main")

set(icons_files
    icons/fi-bs-add.svg
    icons/fi-bs-angle-down.svg
    icons/fi-bs-angle-left.svg
    icons/fi-bs-angle-right.svg
    icons/fi-bs-angle-up.svg
    icons/fi-bs-angry.svg
    icons/fi-bs-arrow-down.svg
    icons/fi-bs-arrows-h-copy.svg
    icons/fi-bs-arrow-up.svg
    icons/fi-bs-broom.svg
    icons/fi-bs-cross.svg
    icons/fi-bs-delete.svg
    icons/fi-bs-document.svg
    icons/fi-bs-download.svg
    icons/fi-bs-exchange.svg
    icons/fi-bs-filter.svg
    icons/fi-bs-folder.svg
    icons/fi-bs-level-down.svg
    icons/fi-bs-level-up.svg
    icons/fi-bs-list.svg
    icons/fi-bs-menu-burger.svg
    icons/fi-bs-menu-dots.svg
    icons/fi-bs-menu-dots-vertical.svg
    icons/fi-bs-redo.svg
    icons/fi-bs-refresh.svg
    icons/fi-bs-remove.svg
    icons/fi-bs-search.svg
    icons/fi-bs-settings.svg
    icons/fi-bs-sign-in.svg
    icons/fi-bs-sign-out.svg
    icons/fi-bs-sign-out-alt.svg
    icons/fi-bs-undo.svg
    icons/fi-bs-sun.svg
    icons/fi-bs-moon.svg
    icons/logo.png
)

set(qml_files
    qml/Table.qml

    qml/Header.qml

    qml/Cell.qml
    qml/CellBool.qml
    qml/CellNumeric.qml
    qml/CellString.qml
)


qt_add_qml_module(Core
    URI modules.Core
    VERSION 1.0

    QML_FILES ${qml_files}

    RESOURCES ${icons_files}
    RESOURCE_PREFIX /


    SOURCES defs.h
    SOURCES macros.h
    SOURCES datapipe.h datapipe.cpp
    SOURCES sqldatabase.h sqldatabase.cpp
    SOURCES sqlquery.h sqlquery.cpp

    SOURCES sqladapter.h sqladapter.cpp
    SOURCES sqladaptersqlite.h sqladaptersqlite.cpp
    SOURCES sqladapterpostgres.h sqladapterpostgres.cpp
    SOURCES asqltablemodel.cpp asqltablemodel.h
    SOURCES guitablemodel.cpp guitablemodel.h
    SOURCES guiitem.cpp guiitem.h
    SOURCES guitable.cpp guitable.h
    SOURCES theme.cpp theme.h
    SOURCES guicell.cpp guicell.h

)


