add_library(glad STATIC src/glad.c)
target_include_directories(glad SYSTEM PUBLIC include)

# glad charge les fonctions dynamiquement : -ldl sur Linux, rien sur Windows
target_link_libraries(glad PUBLIC ${CMAKE_DL_LIBS})
