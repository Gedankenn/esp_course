# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/fabio/esp/esp-idf/components/bootloader/subproject"
  "/home/fabio/github/esp_course/build/bootloader"
  "/home/fabio/github/esp_course/build/bootloader-prefix"
  "/home/fabio/github/esp_course/build/bootloader-prefix/tmp"
  "/home/fabio/github/esp_course/build/bootloader-prefix/src/bootloader-stamp"
  "/home/fabio/github/esp_course/build/bootloader-prefix/src"
  "/home/fabio/github/esp_course/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/fabio/github/esp_course/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/fabio/github/esp_course/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
