cmake_minimum_required(VERSION 3.20)

project(Aujard LANGUAGES CXX RC)

if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  set(CMAKE_MFC_FLAG 1)
endif()

set(AUJARD_SOURCES
  Aujard.cpp
  AujardDlg.cpp
  DBAgent.cpp
  ItemTableSet.cpp
  SharedMem.cpp
  StdAfx.cpp
  Aujard.rc
)

add_executable(Aujard WIN32 ${AUJARD_SOURCES})

target_include_directories(Aujard PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_compile_definitions(Aujard PRIVATE
  WIN32
  _WINDOWS
  _MBCS
  __SAMMA
)

if(MSVC)
  target_compile_options(Aujard PRIVATE /UUNICODE /U_UNICODE /W3)
endif()
