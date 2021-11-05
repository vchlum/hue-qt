find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS Network REQUIRED)

set(HUE_INCLUDE ${CMAKE_SOURCE_DIR}/include/hue)
set(HEADER_HUE_LIST
    ${HUE_INCLUDE}/huebridge.h
    ${HUE_INCLUDE}/huebridgelist.h
    ${HUE_INCLUDE}/huedevice.h
    ${HUE_INCLUDE}/huelist.h
    ${HUE_INCLUDE}/huesyncbox.h
    ${HUE_INCLUDE}/huesyncboxlist.h
    ${HUE_INCLUDE}/hueutils.h)
add_library(hue
    huebridge.cpp
    huebridgediscovery.cpp
    huebridgelist.cpp
    huedevice.cpp
    huelist.cpp
    huesyncbox.cpp
    huesyncboxlist.cpp
    hueutils.cpp
    ${HEADER_HUE_LIST})
target_include_directories(hue PUBLIC ${HUE_INCLUDE})
target_link_libraries(hue PRIVATE Qt6::Network)



set(MENU_INCLUDE ${CMAKE_SOURCE_DIR}/include/menu)
set(HEADER_MENU_LIST
    ${MENU_INCLUDE}/menubutton.h
    ${MENU_INCLUDE}/menuexpendable.h
    ${MENU_INCLUDE}/menugroups.h
    ${MENU_INCLUDE}/menulayout.h
    ${MENU_INCLUDE}/menulights.h
    ${MENU_INCLUDE}/menuscenes.h
    ${MENU_INCLUDE}/menuslider.h
    ${MENU_INCLUDE}/menuswitch.h)
qt_add_library(menu
    ${CMAKE_SOURCE_DIR}/resources/menu_resources.qrc
    menubutton.cpp
    menuexpendable.cpp
    menugroups.cpp
    menulayout.cpp
    menulights.cpp
    menuscenes.cpp
    menuslider.cpp
    menuswitch.cpp
    ${HEADER_MENU_LIST}
)
target_include_directories(menu PUBLIC ${MENU_INCLUDE})
target_link_libraries(menu PRIVATE Qt6::Widgets)