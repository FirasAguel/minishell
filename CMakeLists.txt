cmake_minimum_required(VERSION 3.13)

project(codecrafters-shell)

file(GLOB_RECURSE SOURCE_FILES src/*.c src/*.h)

set(CMAKE_C_STANDARD 23) # Enable the C23 standard

add_executable(shell ${SOURCE_FILES})

# add_library(libft STATIC IMPORTED)
# set_target_properties(libft PROPERTIES
#     IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/libft/libft.a
# )

# add_library(gnl STATIC IMPORTED)
# set_target_properties(gnl PROPERTIES
#     IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/get_next_line/libftgnl.a
# )

target_link_libraries(shell PRIVATE readline)# libft gnl)
