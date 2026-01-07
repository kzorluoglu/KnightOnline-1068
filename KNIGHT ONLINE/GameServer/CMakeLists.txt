cmake_minimum_required(VERSION 3.20)

project(GameServer LANGUAGES CXX RC)

if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  set(CMAKE_MFC_FLAG 1)
endif()

set(GAMESERVER_SOURCES
  CircularBuffer.cpp
  Compress.cpp
  Extern.cpp
  GameSocket.cpp
  global.cpp
  Ini.cpp
  IOCPort.cpp
  IOCPSocket2.cpp
  MagicProcess.cpp
  MagicTableSet.cpp
  MagicType1Set.cpp
  MagicType2Set.cpp
  MagicType3Set.cpp
  MagicType4Set.cpp
  MakeDefensiveTableSet.cpp
  MakeGradeItemTableSet.cpp
  MakeItemTableSet.cpp
  MakeLareItemTableSet.cpp
  MakeWeaponTableSet.cpp
  MAP.cpp
  MonPosSet.cpp
  MonTableSet.cpp
  Npc.cpp
  NpcInfoTable.cpp
  NpcItem.cpp
  NpcItemSet.cpp
  NpcMagicProcess.cpp
  NpcPosSet.cpp
  NpcPosTable.cpp
  NpcTable.cpp
  NpcTableSet.cpp
  NpcThread.cpp
  Party.cpp
  PartyUser.cpp
  PathFind.cpp
  Region.cpp
  RNpcPosSet.cpp
  RoomEvent.cpp
  Server.cpp
  ServerDlg.cpp
  StdAfx.cpp
  User.cpp
  ZoneInfoSet.cpp
  N3BASE/N3ShapeMgr.cpp
  N3BASE/N3VectorAndMatrix.cpp
  Server.rc
)

add_executable(GameServer WIN32 ${GAMESERVER_SOURCES})

target_include_directories(GameServer PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/N3BASE"
)

set(_gs_out "${CMAKE_SOURCE_DIR}/dist/bin/GameServer")
set_target_properties(GameServer PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${_gs_out}"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${_gs_out}"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${_gs_out}"
  RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${_gs_out}"
  RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${_gs_out}"
)

target_compile_definitions(GameServer PRIVATE
  WIN32
  _WINDOWS
  _MBCS
  _3DSERVER
  DIRECT3D_VERSION=0x0800
  _WIN32_WINNT=0x0501
  _CRT_SECURE_NO_WARNINGS
  _WINSOCK_DEPRECATED_NO_WARNINGS
  $<$<CONFIG:Debug>:_REPENT>
)

if(MSVC)
  target_compile_options(GameServer PRIVATE
    /UUNICODE /U_UNICODE /W3
    /wd4005
    /wd4018
    /wd4244
    /wd4267
    /wd4566
    /wd4996
  )
  target_link_options(GameServer PRIVATE
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
      target_include_directories(GameServer PRIVATE
        "${_win_sdk_include}/um"
        "${_win_sdk_include}/shared"
        "${_win_sdk_include}/ucrt"
      )
    endif()
  endif()
endif()

if(EXISTS "${KO_DX8_SDK_DIR}/lib/d3d8.lib")
  target_include_directories(GameServer PRIVATE "${KO_DX8_SDK_DIR}/include")
  target_link_directories(GameServer PRIVATE "${KO_DX8_SDK_DIR}/lib")
else()
  message(FATAL_ERROR "DX8 SDK not found at ${KO_DX8_SDK_DIR}. Expected lib/d3d8.lib and lib/d3dx8.lib.")
endif()

target_link_libraries(GameServer PRIVATE
  ws2_32
  winmm
  imm32
  d3d8
  d3dx8
  "${CMAKE_CURRENT_SOURCE_DIR}/IMPLODE.LIB"
)
