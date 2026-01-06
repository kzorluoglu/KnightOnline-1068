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

target_compile_definitions(Ebenezer PRIVATE
  WIN32
  _WINDOWS
  _MBCS
  _3DSERVER
)

if(MSVC)
  # Force multibyte (legacy project was not Unicode).
  target_compile_options(Ebenezer PRIVATE /UUNICODE /U_UNICODE /W3)
endif()

target_link_libraries(Ebenezer PRIVATE
  ws2_32
  winmm
  "${CMAKE_CURRENT_SOURCE_DIR}/Implode.lib"
  "${CMAKE_CURRENT_SOURCE_DIR}/JvCryption.lib"
)
