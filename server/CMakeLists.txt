cmake_minimum_required(VERSION 3.5)

project(h265Server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Network)

add_compile_options(-Wall -lx11)

pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
    libavdevice
    libavfilter
    libavformat
    libavcodec
    libswresample
    libswscale
    libavutil
)

add_executable(h265Server main.cpp
    encoder.h encoder.cpp
    server.h server.cpp)

include_directories( ../share)

target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::LIBAV)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)

include(GNUInstallDirs)
install(TARGETS h265Server
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
