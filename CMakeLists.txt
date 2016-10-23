CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12.2)

PROJECT(irvine-01)

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
# DownloadProject gives you the ability to download external sources from a git
# repository and build it during configure time so it is available for the 
# build.
#
include(DownloadProject)

#
# Google unit test framework
#
download_project(PROJ                googletest
                 GIT_REPOSITORY      https://github.com/google/googletest.git
                 GIT_TAG             release-1.8.0
                 ${UPDATE_DISCONNECTED_IF_AVAILABLE})
                 
#
# eigen library for linear algebra.  Matricies, vectors, numerical solvers
# and algorithms.
#
download_project(PROJ                eigen
                 URL      http://bitbucket.org/eigen/eigen/get/3.2.10.tar.bz2
                 ${UPDATE_DISCONNECTED_IF_AVAILABLE})

# WITH_BUILDROOT allows you to set whether we are compiling as part of the
# build root environment or not.  
option(WITH_BUILDROOT "with(out) buildroot compile" OFF)
if (WITH_BUILDROOT)
else (WITH_BUILDROOT)
  SET(NOT_BUILDROOT_COMPILE ON)
endif(WITH_BUILDROOT)

#
# allow inclusion of headers in the current directory
#
SET(MAKE_INCLUDE_CURRENT_DIR ON)

#
# Unit test framework
#
ADD_SUBDIRECTORY(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

#
# matrix/linear algebra class library
#
ADD_SUBDIRECTORY(${eigen_SOURCE_DIR} ${eigen_BINARY_DIR})

#
# Beacon broadcasting library
#
ADD_SUBDIRECTORY(beacon)

#
# Satcomm configuration file
#
ADD_SUBDIRECTORY(satcomm)

#
# Experimental Version generation -
# I'd like to have the version in this project, but currently the version file,
# /etc/issue is specified in the irvine-01-cfg project due to the way
# the image gets built.
#
ADD_SUBDIRECTORY(version)
