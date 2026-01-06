cmake_minimum_required(VERSION 3.20)

project(KnightOnline LANGUAGES CXX RC)

# Root build entry point for CLion/CMake.
set(KO_DX8_SDK_DIR "${CMAKE_SOURCE_DIR}/third_party/MsSdkTmp/DXF" CACHE PATH "Path to DirectX 8 SDK root (must contain lib and include)")
add_subdirectory("KNIGHT ONLINE/Aujard")
add_subdirectory("KNIGHT ONLINE/Ebenezer")
add_subdirectory("KNIGHT ONLINE/GameServer")
add_subdirectory("KNIGHT ONLINE/LogInServer")
