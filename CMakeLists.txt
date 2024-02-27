cmake_minimum_required(VERSION 3.26)
project(openmp)

set(CMAKE_CXX_STANDARD 17)

#habilitar el soporte para OpenMP
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fopenmp")

include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 10.1.1
)

FetchContent_Declare(
        fort
        GIT_REPOSITORY https://github.com/seleznevae/libfort.git
        GIT_TAG v0.4.2
)

FetchContent_MakeAvailable(fmt)

FetchContent_MakeAvailable(fort)

add_executable(main main.cpp)

target_link_libraries(main fmt::fmt)

add_executable(reduccion reduccion.cpp)

target_link_libraries(reduccion fmt::fmt)

add_executable(calculo_pi calculo_pi.cpp
        practica_reduccion.cpp
        correccion_examen.cpp
)

target_link_libraries(calculo_pi fmt::fmt libfort::fort)

add_executable(pi pi.cpp)

target_link_libraries(pi fmt::fmt)

add_executable(reduccion2 reduccion2.cpp)

target_link_libraries(reduccion2 fmt::fmt)

add_executable(tareagrupo01 tareagrupo01.cpp)

target_link_libraries(tareagrupo01 fmt::fmt)