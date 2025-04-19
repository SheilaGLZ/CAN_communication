cmake_minimum_required(VERSION 3.16)
project(CAN_comm LANGUAGES CXX)

find_package(Qt5 COMPONENTS Widgets Core Gui REQUIRED)
find_package (Threads REQUIRED)

# === READER
add_executable(can_reader
    can_comm.h
    main_reader.cpp
)
target_include_directories(can_reader PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    widgets
)
target_link_libraries(can_reader
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Threads::Threads
)
set_target_properties(can_reader
  PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED TRUE
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)


# === SENDER
add_executable(can_sender
    can_comm.h
    main_sender.cpp
)
target_include_directories(can_sender PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    widgets
    gui
)
target_link_libraries(can_sender
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Threads::Threads
)
set_target_properties(can_sender
  PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED TRUE
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)