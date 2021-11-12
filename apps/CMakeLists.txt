find_package(Qt6 COMPONENTS Widgets REQUIRED)

qt_add_executable(hue-lights-2
    ${CMAKE_SOURCE_DIR}/resources/resources.qrc
    ${CMAKE_SOURCE_DIR}/src/mainmenu.h
    ${CMAKE_SOURCE_DIR}/src/mainmenu.cpp
    ${CMAKE_SOURCE_DIR}/src/mainmenubridge.h
    ${CMAKE_SOURCE_DIR}/src/mainmenubridge.cpp
    ${CMAKE_SOURCE_DIR}/src/mainmenubridgeutils.h
    ${CMAKE_SOURCE_DIR}/src/mainmenubridgeutils.cpp
    hue-lights-2.cpp)

target_include_directories(hue-lights-2 PUBLIC ${CMAKE_SOURCE_DIR}/src/)

target_link_libraries(hue-lights-2 PRIVATE Qt6::Widgets)
target_link_libraries(hue-lights-2 PRIVATE hue)
target_link_libraries(hue-lights-2 PRIVATE menu)
