cmake_minimum_required (VERSION 2.8.11)

# Fix behavior of CMAKE_CXX_STANDARD when targeting macOS.
if (POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif ()

# Fix a warning on macOS.
if (POLICY CMP0042)
  cmake_policy(SET CMP0042 NEW)
endif ()

# Don't use -rdynamic since it breaks causes musl static linking.
if (POLICY CMP0065)
  cmake_policy(SET CMP0065 NEW)
endif ()

project (libusbp)

set (LIBUSBP_VERSION_MAJOR 1)
set (LIBUSBP_VERSION_MINOR 1)
set (LIBUSBP_VERSION_PATCH 0)

# Make 'Release' be the default build type, since the debug builds
# include exported symbols that might cause name conflicts.
if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE "Release" CACHE STRING
    "Options are Debug Release RelWithDebInfo MinSizeRel" FORCE)
endif ()


option (BUILD_SHARED_LIBS "Build as shared library" TRUE)
if (NOT BUILD_SHARED_LIBS)
  add_definitions (-DLIBUSBP_STATIC)
  set (PC_MORE_CFLAGS "-DLIBUSBP_STATIC")
endif ()

set(ENABLE_EXAMPLES FALSE CACHE BOOL
  "True if you want to build the examples.")

set(ENABLE_TESTS FALSE CACHE BOOL
  "True if you want to build the tests.")

set(LIBUSBP_LOG FALSE CACHE BOOL
  "Output log messages to stderr for debugging.")

set(VBOX_LINUX_ON_WINDOWS FALSE CACHE BOOL
  "Skip tests known to cause problems on a Linux VirtualBox guest on Windows.")

set(ENABLE_GCOV FALSE CACHE BOOL
  "Compile with special options needed for gcov.")

# Our C code uses features from the C99 standard.
macro(use_c99)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
    endif ()
  else ()
    set (CMAKE_C_STANDARD 99)
  endif ()
endmacro(use_c99)

# Our C++ code uses features from the C++11 standard.
macro(use_cxx11)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
      # Use --std=gnu++0x instead of --std=gnu++11 in order to support GCC 4.6.
      set (CMAKE_CXX_FLAGS "--std=gnu++0x ${CMAKE_C_FLAGS}")
    endif ()
  else ()
    set (CMAKE_CXX_STANDARD 11)
  endif ()
endmacro(use_cxx11)

set (LIBUSBP_VERSION ${LIBUSBP_VERSION_MAJOR}.${LIBUSBP_VERSION_MINOR}.${LIBUSBP_VERSION_PATCH})

if (CMAKE_VERSION VERSION_GREATER "2.8.10")
  string(TIMESTAMP YEAR "%Y")
endif ()

find_package(PkgConfig)

# Put libraries and executables in the top level of the build directory
# so that the executables can find the libraries and it is easy to run
# everything.
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Warn about everything.
set (CMAKE_C_FLAGS "-Wall -Wextra -pedantic ${CMAKE_C_FLAGS}")
set (CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic ${CMAKE_CXX_FLAGS}")

if (ENABLE_GCOV)
  set (CMAKE_C_FLAGS "-fprofile-arcs -ftest-coverage ${CMAKE_C_FLAGS}")
endif ()

if (WIN32)

  # Enable correct behavior for the return value of vsnprintf.
  add_definitions (-D__USE_MINGW_ANSI_STDIO=1)

  # Enable functions only available in Windows Vista and later,
  # such as StringCompareEx.
  add_definitions (-D_WIN32_WINNT=0x0600 -DNTDDI_VERSION=0x06000000)

endif ()

# Detect Linux.
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set (LINUX 1)
endif ()

# Install the header files into include/
install(FILES include/libusbp.h include/libusbp.hpp
  DESTINATION "include/libusbp-${LIBUSBP_VERSION_MAJOR}")

add_subdirectory (src)

if (ENABLE_TESTS)
  add_subdirectory (test)
  add_subdirectory (manual_tests)
endif ()

if (ENABLE_EXAMPLES)
  add_subdirectory (examples)
endif ()

if (WIN32)
  add_subdirectory (install_helper)
endif ()


# Install python bindings
option (BUILD_PYTHON_BINDINGS "Build python bindings" FALSE)


if (BUILD_PYTHON_BINDINGS)
   set(PYUSB "pyusbp")

   set(PROGINCL ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${CMAKE_SOURCE_DIR}/include)
   set(PROGSRC ${CMAKE_SOURCE_DIR}/src/libusbp_internal.h ${CMAKE_BINARY_DIR}/src/libusbp_config.h ${CMAKE_SOURCE_DIR}/src/error_hresult.c ${CMAKE_SOURCE_DIR}/src/find_device.c ${CMAKE_SOURCE_DIR}/src/list.c ${CMAKE_SOURCE_DIR}/src/pipe_id.c ${CMAKE_SOURCE_DIR}/src/string.c ${CMAKE_SOURCE_DIR}/src/error.c ${CMAKE_SOURCE_DIR}/src/async_in_pipe.c)


   set(EXTRLIBS "")

   if (WIN32)
       set(EXTRSRC ${CMAKE_SOURCE_DIR}/src/windows/serial_port_windows.c ${CMAKE_SOURCE_DIR}/src/windows/device_instance_id_windows.c ${CMAKE_SOURCE_DIR}/src/windows/list_windows.c ${CMAKE_SOURCE_DIR}/src/windows/async_in_transfer_windows.c ${CMAKE_SOURCE_DIR}/src/windows/device_windows.c ${CMAKE_SOURCE_DIR}/src/windows/error_windows.c ${CMAKE_SOURCE_DIR}/src/windows/generic_interface_windows.c ${CMAKE_SOURCE_DIR}/src/windows/interface_windows.c ${CMAKE_SOURCE_DIR}/src/windows/generic_handle_windows.c) 
   elseif (UNIX)
       if (NOT APPLE)
           # Linux
           set(EXTRSRC ${CMAKE_SOURCE_DIR}/src/linux/device_linux.c ${CMAKE_SOURCE_DIR}/src/linux/serial_port_linux.c ${CMAKE_SOURCE_DIR}/src/linux/error_linux.c ${CMAKE_SOURCE_DIR}/src/linux/async_in_transfer_linux.c ${CMAKE_SOURCE_DIR}/src/linux/list_linux.c ${CMAKE_SOURCE_DIR}/src/linux/usbfd_linux.c ${CMAKE_SOURCE_DIR}/src/linux/generic_handle_linux.c ${CMAKE_SOURCE_DIR}/src/linux/udev_linux.c ${CMAKE_SOURCE_DIR}/src/linux/generic_interface_linux.c)
       else ()
           # Mac
           find_library(IOKit IOKit)
           find_library(CoreFoundation CoreFoundation)
           set(EXTRLIBS ${IOKit} ${CoreFoundation})
           set(EXTRSRC ${CMAKE_SOURCE_DIR}/src/mac/iokit_mac.c ${CMAKE_SOURCE_DIR}/src/mac/generic_interface_mac.c ${CMAKE_SOURCE_DIR}/src/mac/async_in_transfer_mac.c ${CMAKE_SOURCE_DIR}/src/mac/generic_handle_mac.c ${CMAKE_SOURCE_DIR}/src/mac/error_mac.c ${CMAKE_SOURCE_DIR}/src/mac/serial_port_mac.c ${CMAKE_SOURCE_DIR}/src/mac/device_mac.c ${CMAKE_SOURCE_DIR}/src/mac/list_mac.c ${IOKit} ${CoreFoundation})
       endif ()
   endif ()

   find_package(pybind11)
   pybind11_add_module(${PYUSB} bindings/bindings.cpp ${PROGSRC} ${EXTRSRC})
   target_include_directories(${PYUSB} PUBLIC ${PROGINCL} ${EXTRHDR})
   target_link_libraries(${PYUSB} PUBLIC ${EXTRLIBS}) 
endif ()

