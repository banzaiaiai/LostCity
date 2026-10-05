# Récupère automatiquement tous les .cc / .cpp (sous-dossiers inclus)
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(app ${APP_SOURCES})

target_include_directories(app PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(app PRIVATE glfw glad glm::glm)

# Warnings selon le compilateur
if(MSVC)
    target_compile_options(app PRIVATE /W4)
else()
    target_compile_options(app PRIVATE -Wall -Wextra)
endif()
# Dossier où les shaders sont copiés (indépendant de la cible app)
set(SHADER_OUT_DIR ${CMAKE_BINARY_DIR}/bin/shaders)

add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${PROJECT_SOURCE_DIR}/shaders
            ${SHADER_OUT_DIR}
    COMMENT "Copie des shaders")
add_dependencies(app copy_shaders)

# Chemin absolu accessible dans le code C++
target_compile_definitions(app PRIVATE SHADER_DIR="${SHADER_OUT_DIR}/")
