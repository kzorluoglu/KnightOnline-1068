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

target_compile_definitions(VersionManager PRIVATE
  WIN32
  _WINDOWS
  _MBCS
)

if(MSVC)
  target_compile_options(VersionManager PRIVATE /UUNICODE /U_UNICODE /W3)
endif()

target_link_libraries(VersionManager PRIVATE ws2_32)

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
)

if(MSVC)
  target_compile_options(ZipLib PRIVATE /UUNICODE /U_UNICODE /W3)
endif()
