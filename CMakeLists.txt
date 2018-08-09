CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12.2)

PROJECT(irvine-01)

enable_testing()

#
# Set up cmake modules path.  cmake modules allow you to extend
# cmake functionality.  We use it to automatically download
# the google test framework and make it easier to set up unit tests.
#
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

#
# Modules
#

#
# gmock provides add_gmock_test function add_gmock_test(<target> <sources>...)
#
include(gmock)

#
# allow inclusion of headers in the current directory
#
SET(MAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")

#
# DownloadProject gives you the ability to download external sources from a git
# repository and build it during configure time so it is available for the 
# build.
#
include(DownloadProject)
include(ExternalProject)

option(VERBOSE "Turn on Verbose" OFF)

if (VERBOSE)
  set (CMAKE_VERBOSE_MAKEFILE on)
endif(VERBOSE)

# WITH_BUILDROOT allows you to set whether we are compiling as part of the
# build root environment or not.  
option(WITH_BUILDROOT "with(out) buildroot compile" OFF)
if (WITH_BUILDROOT)
else (WITH_BUILDROOT)
  SET(PROJECT_COMPILE ON)
  #
  # Google unit test framework
  #
  download_project(PROJ                googletest
                   GIT_REPOSITORY      https://github.com/google/googletest.git
                   GIT_TAG             release-1.8.0
                   ${UPDATE_DISCONNECTED_IF_AVAILABLE})
  #
  # Unit test framework
  #
  ADD_SUBDIRECTORY(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

                 # i2ctest package
  ExternalProject_Add(i2c-tools
                   GIT_REPOSITORY https://github.com/Distrotech/i2c-tools
                   GIT_TAG V3-1-0
                   PREFIX ${CMAKE_BINARY_DIR}
                   CONFIGURE_COMMAND ""
                   SOURCE_DIR ${CMAKE_BINARY_DIR}/i2c-tools-3.1.0
                   BUILD_COMMAND make -C ${CMAKE_BINARY_DIR}/i2c-tools-3.1.0
                   INSTALL_COMMAND make -C ${CMAKE_BINARY_DIR}/i2c-tools-3.1.0 install DESTDIR=${CMAKE_BINARY_DIR}/i2c-tools-3.1.0 )

              
  #
  # eigen library for linear algebra.  Matricies, vectors, numerical solvers
  # and algorithms.
  #
  #download_project(PROJ                eigen
  #  URL      http://bitbucket.org/eigen/eigen/get/3.2.10.tar.bz2
  #  ${UPDATE_DISCONNECTED_IF_AVAILABLE})

  #
  # matrix/linear algebra class library
  #
  # ADD_SUBDIRECTORY(${eigen_SOURCE_DIR} ${eigen_BINARY_DIR})

endif(WITH_BUILDROOT)
#
# Thread utility library
#
ADD_SUBDIRECTORY(thread)

ADD_SUBDIRECTORY(gpiocpp)

#
# ADCS Sensor service
#
ADD_SUBDIRECTORY(adcs-sensors)

#
# Beacon broadcasting process
#
ADD_SUBDIRECTORY(beacon)

#
# command processing daemon
#
ADD_SUBDIRECTORY(cmdd)

#
# Remote commands
#
ADD_SUBDIRECTORY(cmds)

#
# C-Card controller
#
ADD_SUBDIRECTORY(ccardctl)

#
# Satcomm configuration file
#
ADD_SUBDIRECTORY(satcomm)

#
# Flight scripts
#
ADD_SUBDIRECTORY(flightscripts)

#
# datalogger storage
#
ADD_SUBDIRECTORY(datalogger)

#
# Experimental Version generation -
# I'd like to have the version in this project, but currently the version file,
# /etc/issue is specified in the irvine-01-cfg project due to the way
# the image gets built.
#
ADD_SUBDIRECTORY(version)
#
# Test Utilities for Ground/Flight 
#
ADD_SUBDIRECTORY(utilities)
