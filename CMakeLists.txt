cmake_minimum_required(VERSION 3.16)
project(LostCity VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Pour clangd (génère build/compile_commands.json)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Release par défaut si rien n'est précisé
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Tous les exécutables dans build/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(dependencies)
add_subdirectory(src)

enable_testing()
add_subdirectory(test)
