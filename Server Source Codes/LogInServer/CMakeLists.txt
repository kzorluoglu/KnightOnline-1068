cmake_minimum_required(VERSION 3.20)

project(LogInServer LANGUAGES CXX RC)

if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  set(CMAKE_MFC_FLAG 1)
endif()

set(LOGINSERVER_SOURCES
  CircularBuffer.cpp
  DBProcess.cpp
  DlgBrowsePath.cpp
  IOCPort.cpp
  IOCPSocket2.cpp
  SettingDlg.cpp
  ShellTree.cpp
  StdAfx.cpp
  User.cpp
  VersionManager.cpp
  VersionManagerDlg.cpp
  VersionSet.cpp
  VersionManager.rc
)

add_executable(VersionManager WIN32 ${LOGINSERVER_SOURCES})

target_include_directories(VersionManager PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

set(_vm_out "${CMAKE_SOURCE_DIR}/dist/bin/VersionManager")
set_target_properties(VersionManager PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${_vm_out}"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${_vm_out}"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${_vm_out}"
  RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${_vm_out}"
  RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${_vm_out}"
)

target_compile_definitions(VersionManager PRIVATE
  WIN32
  _WINDOWS
  _MBCS
  _WIN32_WINNT=0x0501
  _CRT_SECURE_NO_WARNINGS
  _WINSOCK_DEPRECATED_NO_WARNINGS
)

if(MSVC)
  target_compile_options(VersionManager PRIVATE
    /UUNICODE /U_UNICODE /W3
    /Zc:forScope-
    /wd4005
    /wd4018
    /wd4244
    /wd4267
    /wd4566
    /wd4996
    /wd4477
    /wd4474
    /wd4102
    /wd4101
    /wd4288
  )
  target_link_options(VersionManager PRIVATE
    /NODEFAULTLIB:LIBC
    /NODEFAULTLIB:LIBCD
  )
endif()

target_link_libraries(VersionManager PRIVATE
  ws2_32
  ZipLib
  "${CMAKE_CURRENT_SOURCE_DIR}/zlib.lib"
)

set(ZIP_SOURCES
  AutoBuffer.cpp
  BigFile.cpp
  CentralDir.cpp
  FileHeader.cpp
  StdAfx.cpp
  ZipArchive.cpp
  ZipException.cpp
  ZipInternalInfo.cpp
  ZipStorage.cpp
)

add_library(ZipLib STATIC ${ZIP_SOURCES})

target_include_directories(ZipLib PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_compile_definitions(ZipLib PRIVATE
  WIN32
  _WINDOWS
  _MBCS
  ZLIB_DLL
  _WIN32_WINNT=0x0501
  _CRT_SECURE_NO_WARNINGS
  _WINSOCK_DEPRECATED_NO_WARNINGS
)

if(MSVC)
  target_compile_options(ZipLib PRIVATE
    /UUNICODE /U_UNICODE /W3
    /Zc:forScope-
    /wd4005
    /wd4018
    /wd4244
    /wd4267
    /wd4566
    /wd4996
    /wd4477
    /wd4474
    /wd4102
    /wd4101
    /wd4288
  )
endif()
