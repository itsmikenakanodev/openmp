# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Programacion Paralela/openmp/cmake-build-debug/_deps/fort-src"
  "D:/Programacion Paralela/openmp/cmake-build-debug/_deps/fort-build"
  "D:/Programacion Paralela/openmp/cmake-build-debug/_deps/fort-subbuild/fort-populate-prefix"
  "D:/Programacion Paralela/openmp/cmake-build-debug/_deps/fort-subbuild/fort-populate-prefix/tmp"
  "D:/Programacion Paralela/openmp/cmake-build-debug/_deps/fort-subbuild/fort-populate-prefix/src/fort-populate-stamp"
  "D:/Programacion Paralela/openmp/cmake-build-debug/_deps/fort-subbuild/fort-populate-prefix/src"
  "D:/Programacion Paralela/openmp/cmake-build-debug/_deps/fort-subbuild/fort-populate-prefix/src/fort-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Programacion Paralela/openmp/cmake-build-debug/_deps/fort-subbuild/fort-populate-prefix/src/fort-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Programacion Paralela/openmp/cmake-build-debug/_deps/fort-subbuild/fort-populate-prefix/src/fort-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
