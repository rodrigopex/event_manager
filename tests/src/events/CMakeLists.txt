#
# Copyright (c) 2022 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0
#

target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/data_event.c)

target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/multicontext_event.c)

target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/order_event.c)

target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/sized_events.c)

target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test_events.c)
