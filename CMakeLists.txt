# 
# Copyright (c) 2017 Josua Mayer <josua.mayer97@gmail.com>
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 

cmake_minimum_required(VERSION 2.6)
project(expand_fs C)
SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(LibFdisk REQUIRED)

add_executable(expand_partition expand_partition.c)
include_directories(${LIBFDISK_INCLUDE_DIRS})
target_link_libraries(expand_partition ${LIBFDISK_LIBRARIES})
install(TARGETS expand_partition RUNTIME DESTINATION bin)

add_executable(find_root find_root.c)
install(TARGETS find_root RUNTIME DESTINATION bin)
