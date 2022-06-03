/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _MEASUREMENT_EVENT_H_
#define _MEASUREMENT_EVENT_H_

/**
 * @brief Measurement Event
 * @defgroup measurement_event Measurement Event
 * @{
 */

#include <app_event_manager.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MESSAGE_SIZE 16
struct measurement_event {
    struct app_event_header header;

    uint8_t bytes[MESSAGE_SIZE];
};

APP_EVENT_TYPE_DECLARE(measurement_event);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* _MEASUREMENT_EVENT_H_ */
