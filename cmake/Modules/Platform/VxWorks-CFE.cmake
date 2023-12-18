#*********************************************************************************************************
# Export Control Marking
#
# EAR ECCN 9D515.a, 9E515.a, License Exception GOV 740.11 (b)(2)(ii):
# This document contains data within the purview of the U.S. Export Administration Regulations (EAR),
# 15 CFR 730-774, as is classified as ECCN 9E515.a. These items are controlled by the U.S. Government 
# and are authorized for export by NASA only to fulfill responsibilities of the parties or of a 
# Cooperating Agency of a NASA Gateway program partner (CSA, ESA, JAXA) and their contractors using 
# License Exception GOV 740.11 (b)(2)(ii) in furtherance of the ISS Intergovernmental Agreement and 
# Gateway MOUs. They may not be resold, transferred, or otherwise disposed of, to any other country
# or to any person other than the authorized ultimate consignee or end-user(s), either in their
# original form or after being incorporated into other items, without first obtaining approval from
# the U.S. government or as otherwise authorized by U.S. law and regulations.
#*********************************************************************************************************

# -----------------------------------------------------------------
#  CFE-specific CMake Module for VxWorks
#
# The cmake distribution by default may contain a module for VxWorks,
# but different link rules are needed in order to build loadable CFE apps.
# -----------------------------------------------------------------

# Note - this is not building "shared libs" in the traditional sense,
# This property is set true which allows one to use the CMake shared library logic
# But the code is otherwise built as static -- no PIC flags 
set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)
set(CMAKE_EXECUTABLE_SUFFIX ".exe")
set(CMAKE_SHARED_LIBRARY_SUFFIX ".o")
set(CMAKE_SHARED_MODULE_SUFFIX ".o")

# Setting all these to empty string defeats the default behavior
# of adding an -fPIC option to shared library/module code.
set(CMAKE_DL_LIBS "")
set(CMAKE_C_COMPILE_OPTIONS_PIC "")
set(CMAKE_CXX_COMPILE_OPTIONS_PIC "")

set(CMAKE_SHARED_LIBRARY_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_CXX_FLAGS "")
set(CMAKE_SHARED_MODULE_C_FLAGS "")
set(CMAKE_SHARED_MODULE_CXX_FLAGS "")


# This creates a simple relocatable object file, not a shared library
set(CMAKE_SHARED_OBJECT_LINKER_FLAGS -r)
set(CMAKE_C_CREATE_SHARED_MODULE "<CMAKE_LINKER> <LINK_FLAGS> -o <TARGET> ${CMAKE_SHARED_OBJECT_LINKER_FLAGS} <OBJECTS> <LINK_LIBRARIES>")
set(CMAKE_CXX_CREATE_SHARED_MODULE ${CMAKE_C_CREATE_SHARED_MODULE})
set(CMAKE_C_CREATE_SHARED_LIBRARY ${CMAKE_C_CREATE_SHARED_MODULE})
set(CMAKE_CXX_CREATE_SHARED_LIBRARY ${CMAKE_C_CREATE_SHARED_MODULE})
set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_LINKER> <LINK_FLAGS> -o <TARGET> ${CMAKE_SHARED_OBJECT_LINKER_FLAGS} <OBJECTS> <LINK_LIBRARIES>")

set(VXWORKS_TARGET_PATH "${WIND_HOME}/vxworks-${CMAKE_SYSTEM_VERSION}/target")

set(CMAKE_FIND_ROOT_PATH "${VXWORKS_TARGET_PATH}")

set(CMAKE_SYSTEM_PREFIX_PATH "${VXWORKS_TARGET_PATH}")
set(CMAKE_SYSTEM_INCLUDE_PATH "${VXWORKS_TARGET_PATH}/h")
    
set(CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES ${CMAKE_SYSTEM_INCLUDE_PATH})
set(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES ${CMAKE_SYSTEM_INCLUDE_PATH})
#set(CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${VXWORKS_TARGET_PATH}/lib")
#set(CMAKE_SYSTEM_LIBRARY_PATH ${CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES})

#set(CMAKE_C_FLAGS_INIT "${VXWORKS_BSP_C_FLAGS}")
#set(CMAKE_CXX_FLAGS_INIT "${VXWORKS_BSP_CXX_FLAGS}")

