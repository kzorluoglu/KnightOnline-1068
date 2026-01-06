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
  Server.rc
)

add_executable(GameServer WIN32 ${GAMESERVER_SOURCES})

target_include_directories(GameServer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_compile_definitions(GameServer PRIVATE
  WIN32
  _WINDOWS
  _MBCS
  _3DSERVER
  $<$<CONFIG:Debug>:_REPENT>
)

if(MSVC)
  target_compile_options(GameServer PRIVATE /UUNICODE /U_UNICODE /W3)
endif()

if(EXISTS "${KO_DX8_SDK_DIR}/lib/d3d8.lib")
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
