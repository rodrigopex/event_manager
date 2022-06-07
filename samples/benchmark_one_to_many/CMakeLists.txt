#
# Copyright (c) 2021 Nordic Semiconductor
#
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project("Application Event Manager sample")

zephyr_compile_definitions(   
    -DBM_MESSAGE_SIZE=${MSG_SIZE}U -DBM_ONE_TO=${ONE_TO}LLU
    )   
# Include application application event headers
zephyr_library_include_directories(src/events)

target_compile_definitions(app PRIVATE   
    -DBM_MESSAGE_SIZE=${MSG_SIZE}U -DBM_ONE_TO=${ONE_TO}LLU -DBM_ASYNC=${ASYNC} -DBM_DYNAMIC_ALLOCATION=${DALLOC}
    )   

# Application sources
# NORDIC SDK APP START
target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/events/measurement_event.c)
# NORDIC SDK APP END
