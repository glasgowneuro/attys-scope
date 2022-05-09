cmake_minimum_required(VERSION 3.7.0)

project(attys-scope VERSION 2.3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets Multimedia Network REQUIRED)

add_executable(attys-scope
  attys-scope.cpp
  scopewindow.cpp
  gain.cpp
  lowpass.cpp
  highpass.cpp
  channel.cpp
  special.cpp
  current.cpp
  bandstop.cpp
  recbutton.cpp
  samplingrate.cpp
  audiobeep.cpp
  application.qrc
)

target_link_libraries(attys-scope Qt5::Widgets)
target_link_libraries(attys-scope Qt5::Multimedia)

if(CMAKE_HOST_SYSTEM MATCHES Linux)
  message(STATUS "Build host runs Linux")
  target_link_libraries(attys-scope iir attyscomm bluetooth)
endif()

if(CMAKE_SYSTEM MATCHES Windows)
   message(STATUS "Target system is Windows")
endif()

install(TARGETS attys-scope)
