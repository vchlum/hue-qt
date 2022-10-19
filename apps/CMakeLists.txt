find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(hue-qt
    ${CMAKE_SOURCE_DIR}/resources/resources.qrc
    mainmenu.h
    mainmenu.cpp
    mainmenubridge.h
    mainmenubridge.cpp
    mainmenubridgeutils.h
    mainmenubridgeutils.cpp
    mainmenusyncbox.h
    mainmenusyncbox.cpp
    mainmenusyncboxutils.h
    mainmenusyncboxutils.cpp
    hue-qt.cpp)

target_include_directories(hue-qt PUBLIC ${CMAKE_SOURCE_DIR}/apps/)

target_link_libraries(hue-qt PRIVATE Qt5::Gui)
target_link_libraries(hue-qt PRIVATE Qt5::Widgets)
target_link_libraries(hue-qt PRIVATE hue)
target_link_libraries(hue-qt PRIVATE menu)
