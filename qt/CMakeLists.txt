cmake_minimum_required(VERSION 3.20)
project(dnschanger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(DNSLIB_LIB "${CMAKE_CURRENT_SOURCE_DIR}/../rust/target/release/libdnslib.a")

add_executable(dnschanger
    main.cpp
    MainWindow.cpp
    MainWindow.h
)

target_link_libraries(dnschanger
    Qt6::Widgets
    ${DNSLIB_LIB}
)

set_target_properties(dnschanger PROPERTIES AUTOMOC ON)
