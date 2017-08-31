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

# 
# - Try to find LibFdisk
# Once done this will define
#  LIBFDISK_FOUND - System has LibFdisk
#  LIBFDISK_INCLUDE_DIRS - The LibFdisk include directories
#  LIBFDISK_LIBRARIES - The libraries needed to use LibFdisk
# 

# find header
find_path( LIBFDISK_INCLUDE_DIR libfdisk.h PATH_SUFFIXES libfdisk )

# find library
find_library( LIBFDISK_LIBRARY fdisk )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBFDISK_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args( LibFdisk DEFAULT_MSG LIBFDISK_INCLUDE_DIR LIBFDISK_LIBRARY )

mark_as_advanced( LIBFDISK_INCLUDE_DIR LIBFDISK_LIBRARY )

set(LIBFDISK_LIBRARIES ${LIBFDISK_LIBRARY} )
set(LIBFDISK_INCLUDE_DIRS ${LIBFDISK_INCLUDE_DIR} )
