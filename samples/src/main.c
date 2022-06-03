/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <app_event_manager.h>
#include <logging/log.h>
#include <stdint.h>
#include <zephyr.h>
#include "measurement_event.h"
#define BYTES_TO_BE_SENT 256000LLU

uint64_t count;


static bool app_event_handler(const struct app_event_header *aeh)
{
    if (is_measurement_event(aeh)) {
        struct measurement_event *me = cast_measurement_event(aeh);
        (void) me;
        count += MESSAGE_SIZE;
    } else {
        printk("Wrong event came!");
        k_oops();
    }
    return false;
}

APP_EVENT_LISTENER(consumer, app_event_handler);
APP_EVENT_SUBSCRIBE(consumer, measurement_event);


void main(void)
{
    if (app_event_manager_init()) {
        printk("Application Event Manager not initialized");
    }

    printk("Benchmark sample started, version %u.%u-%u!\n", 0, 1, 0);
}

uint8_t msg_data[MESSAGE_SIZE] = {0};

void producer_thread(void)
{
    for (uint64_t i = (MESSAGE_SIZE - 1); i > 0; --i) {
        msg_data[i] = i;
    }
    uint64_t current_message_size = MESSAGE_SIZE;
    uint32_t start                = k_uptime_get_32();
    for (uint64_t internal_count = BYTES_TO_BE_SENT; internal_count > 0;
         internal_count -= current_message_size) {
        struct measurement_event *event = new_measurement_event();
        memcpy(event->bytes, msg_data, MESSAGE_SIZE);
        APP_EVENT_SUBMIT(event);
        // printk("i = %llu\n", internal_count);
    }
    uint32_t duration = (k_uptime_get_32() - start);
    uint64_t i        = (BYTES_TO_BE_SENT * 1000) / duration;
    uint64_t f        = ((BYTES_TO_BE_SENT * 100000) / duration) % 100;
    printk("\n *** Bytes sent = %lld, received = %llu in %ums\n     Average data rate: "
           "%llu.%lluB/s\n\n\n",
           BYTES_TO_BE_SENT, count, duration, i, f);
}

K_THREAD_DEFINE(producer_thread_id, 1024, producer_thread, NULL, NULL, NULL, 5, 0, 10);
