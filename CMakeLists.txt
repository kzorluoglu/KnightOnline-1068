cmake_minimum_required(VERSION 3.20)

project(KnightOnline LANGUAGES CXX RC)

# Root build entry point for CLion/CMake.
set(KO_DX8_SDK_DIR "${CMAKE_SOURCE_DIR}/third_party/dx8sdk/DXF" CACHE PATH "Path to DirectX 8 SDK root (must contain lib and include)")
add_subdirectory("Server Source Codes/Aujard")
add_subdirectory("Server Source Codes/Ebenezer")
add_subdirectory("Server Source Codes/GameServer")
add_subdirectory("Server Source Codes/LogInServer")
