find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)

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
target_link_libraries(hue PRIVATE Qt5::Network)



set(MENU_INCLUDE ${CMAKE_SOURCE_DIR}/include/menu)
set(HEADER_MENU_LIST
    ${MENU_INCLUDE}/menubutton.h
    ${MENU_INCLUDE}/menucolorpicker.h
    ${MENU_INCLUDE}/menuexpendable.h
    ${MENU_INCLUDE}/menuslider.h
    ${MENU_INCLUDE}/menuswitch.h
    ${MENU_INCLUDE}/menuutils.h)
add_library(menu
    ${CMAKE_SOURCE_DIR}/resources/menu_resources.qrc
    menubutton.cpp
    menucolorpicker.cpp
    menuexpendable.cpp
    menuslider.cpp
    menuswitch.cpp
    menuutils.cpp
    ${HEADER_MENU_LIST}
)
target_include_directories(menu PUBLIC ${MENU_INCLUDE})
target_link_libraries(menu PRIVATE Qt5::Widgets)
