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

set(_auj_out "${CMAKE_SOURCE_DIR}/dist/bin/Aujard")
set_target_properties(Aujard PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${_auj_out}"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${_auj_out}"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${_auj_out}"
  RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${_auj_out}"
  RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${_auj_out}"
)

target_compile_definitions(Aujard PRIVATE
  WIN32
  _WINDOWS
  _MBCS
  __SAMMA
  _WIN32_WINNT=0x0501
  _CRT_SECURE_NO_WARNINGS
)

if(MSVC)
  target_compile_options(Aujard PRIVATE
    /UUNICODE /U_UNICODE /W3
    /Zc:forScope-
    /wd4005
    /wd4018
    /wd4244
    /wd4267
    /wd4566
    /wd4996
    /wd4474
  )
endif()
