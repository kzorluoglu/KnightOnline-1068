cmake_minimum_required(VERSION 3.20)

project(Ebenezer LANGUAGES CXX RC)

if(MSVC)
  # Match the legacy VC6 runtime (/MT, /MTd).
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  # Use MFC as a static library to align with the legacy project settings.
  set(CMAKE_MFC_FLAG 1)
endif()

set(EBENEZER_SOURCES
  AISocket.cpp
  BattleSet.cpp
  CircularBuffer.cpp
  CoefficientSet.cpp
  Compress.cpp
  Ebenezer.cpp
  EbenezerDlg.cpp
  EVENT.cpp
  EVENT_DATA.cpp
  EventSet.cpp
  EXEC.cpp
  GameEvent.cpp
  HomeSet.cpp
  Ini.cpp
  IOCPort.cpp
  IOCPSocket2.cpp
  ItemTableSet.cpp
  Knights.cpp
  KnightsManager.cpp
  KnightsRankSet.cpp
  KnightsSet.cpp
  KnightsUserSet.cpp
  LevelUpTableSet.cpp
  LOGIC_ELSE.cpp
  MagicProcess.cpp
  MagicTableSet.cpp
  MagicType1Set.cpp
  MagicType2Set.cpp
  MagicType3Set.cpp
  MagicType4Set.cpp
  MagicType5Set.cpp
  MagicType8Set.cpp
  Map.cpp
  Npc.cpp
  Region.cpp
  SharedMem.cpp
  StdAfx.cpp
  UdpSocket.cpp
  User.cpp
  ZoneInfoSet.cpp
  Ebenezer.rc
  N3Base/N3ShapeMgr.cpp
)

add_executable(Ebenezer WIN32 ${EBENEZER_SOURCES})

target_include_directories(Ebenezer PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/N3Base"
)

set(_eb_out "${CMAKE_SOURCE_DIR}/dist/bin/Ebenezer")
set_target_properties(Ebenezer PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${_eb_out}"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${_eb_out}"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${_eb_out}"
  RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${_eb_out}"
  RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${_eb_out}"
)

target_compile_definitions(Ebenezer PRIVATE
  WIN32
  _WINDOWS
  _MBCS
  _3DSERVER
  DIRECT3D_VERSION=0x0800
  _WIN32_WINNT=0x0501
  _CRT_SECURE_NO_WARNINGS
  _WINSOCK_DEPRECATED_NO_WARNINGS
)

if(MSVC)
  # Force multibyte (legacy project was not Unicode).
  target_compile_options(Ebenezer PRIVATE
    /UUNICODE /U_UNICODE /W3
    /Zc:forScope-
    /wd4005
    /wd4018
    /wd4244
    /wd4267
    /wd4566
    /wd4996
    /wd4477
    /wd4102
    /wd4288
  )
  target_link_options(Ebenezer PRIVATE
    /NODEFAULTLIB:LIBC
    /NODEFAULTLIB:LIBCD
  )
endif()

if(MSVC)
  set(_win_sdk_dir "$ENV{WindowsSdkDir}")
  set(_win_sdk_ver "$ENV{WindowsSDKVersion}")
  if (NOT _win_sdk_ver AND CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION)
    set(_win_sdk_ver "${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")
  endif()
  if (NOT _win_sdk_dir)
    set(_win_sdk_dir "C:/Program Files (x86)/Windows Kits/10/")
  endif()
  if (_win_sdk_ver)
    string(REGEX REPLACE "[/\\\\]$" "" _win_sdk_ver "${_win_sdk_ver}")
    set(_win_sdk_include "${_win_sdk_dir}Include/${_win_sdk_ver}")
    if (EXISTS "${_win_sdk_include}/um/windows.h")
      target_include_directories(Ebenezer PRIVATE
        "${_win_sdk_include}/um"
        "${_win_sdk_include}/shared"
        "${_win_sdk_include}/ucrt"
      )
    endif()
  endif()
endif()

if(EXISTS "${KO_DX8_SDK_DIR}/lib/d3d8.lib")
  target_include_directories(Ebenezer PRIVATE "${KO_DX8_SDK_DIR}/include")
  target_link_directories(Ebenezer PRIVATE "${KO_DX8_SDK_DIR}/lib")
else()
  message(FATAL_ERROR "DX8 SDK not found at ${KO_DX8_SDK_DIR}. Expected lib/d3d8.lib and lib/d3dx8.lib.")
endif()

target_link_libraries(Ebenezer PRIVATE
  ws2_32
  winmm
  d3d8
  d3dx8
  "${CMAKE_CURRENT_SOURCE_DIR}/Implode.lib"
  "${CMAKE_CURRENT_SOURCE_DIR}/JvCryption.lib"
)
