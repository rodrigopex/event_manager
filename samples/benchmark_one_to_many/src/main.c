/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <app_event_manager.h>
#include <zephyr/logging/log.h>
#include <stdint.h>
#include <zephyr/kernel.h>
#include "measurement_event.h"
#define BYTES_TO_BE_SENT (256 * 1024LLU)

uint64_t count = 0;

#if (BM_DYNAMIC_ALLOCATION == 0)
struct measurement_event static_event_holder;

void *app_event_manager_alloc(size_t size)
{
    return &static_event_holder;
}

void app_event_manager_free(void *addr)
{
    return;
}
#endif

static bool s_cb(const struct app_event_header *aeh)
{
    if (is_measurement_event(aeh)) {
        struct measurement_event *me = cast_measurement_event(aeh);
        (void) me;
        count += CONFIG_BM_MESSAGE_SIZE;
    } else {
        printk("Wrong event came!");
        k_oops();
    }
    return false;
}

APP_EVENT_LISTENER(s1, s_cb);
APP_EVENT_SUBSCRIBE(s1, measurement_event);
#if (CONFIG_BM_ONE_TO >= 2LLU)
APP_EVENT_LISTENER(s2, s_cb);
APP_EVENT_SUBSCRIBE(s2, measurement_event);
#if (CONFIG_BM_ONE_TO > 2LLU)
APP_EVENT_LISTENER(s3, s_cb);
APP_EVENT_SUBSCRIBE(s3, measurement_event);
APP_EVENT_LISTENER(s4, s_cb);
APP_EVENT_SUBSCRIBE(s4, measurement_event);
#if (CONFIG_BM_ONE_TO > 4LLU)
APP_EVENT_LISTENER(s5, s_cb);
APP_EVENT_SUBSCRIBE(s5, measurement_event);
APP_EVENT_LISTENER(s6, s_cb);
APP_EVENT_SUBSCRIBE(s6, measurement_event);
APP_EVENT_LISTENER(s7, s_cb);
APP_EVENT_SUBSCRIBE(s7, measurement_event);
APP_EVENT_LISTENER(s8, s_cb);
APP_EVENT_SUBSCRIBE(s8, measurement_event);
#if (CONFIG_BM_ONE_TO > 8LLU)
APP_EVENT_LISTENER(s9, s_cb);
APP_EVENT_SUBSCRIBE(s9, measurement_event);
APP_EVENT_LISTENER(s10, s_cb);
APP_EVENT_SUBSCRIBE(s10, measurement_event);
APP_EVENT_LISTENER(s11, s_cb);
APP_EVENT_SUBSCRIBE(s11, measurement_event);
APP_EVENT_LISTENER(s12, s_cb);
APP_EVENT_SUBSCRIBE(s12, measurement_event);
APP_EVENT_LISTENER(s13, s_cb);
APP_EVENT_SUBSCRIBE(s13, measurement_event);
APP_EVENT_LISTENER(s14, s_cb);
APP_EVENT_SUBSCRIBE(s14, measurement_event);
APP_EVENT_LISTENER(s15, s_cb);
APP_EVENT_SUBSCRIBE(s15, measurement_event);
APP_EVENT_LISTENER(s16, s_cb);
APP_EVENT_SUBSCRIBE(s16, measurement_event);
#endif
#endif
#endif
#endif

void main(void)
{
    if (app_event_manager_init()) {
        printk("Application Event Manager not initialized");
    }

    printk(" Benchmark 1 to %d: SYNC transmission and message size %u\n", CONFIG_BM_ONE_TO, CONFIG_BM_MESSAGE_SIZE);
}


void producer_thread(void)
{
    uint8_t msg_to_be_sent[CONFIG_BM_MESSAGE_SIZE] = {0};
    for (uint64_t i = (CONFIG_BM_MESSAGE_SIZE - 1); i > 0; --i) {
        msg_to_be_sent[i] = i;
    }
    uint32_t start = k_uptime_get_32();
    for (uint64_t internal_count = BYTES_TO_BE_SENT / CONFIG_BM_ONE_TO; internal_count > 0;
         internal_count -= CONFIG_BM_MESSAGE_SIZE) {
        struct measurement_event *event = new_measurement_event();
        memcpy(event->bytes, msg_to_be_sent, CONFIG_BM_MESSAGE_SIZE);
        APP_EVENT_SUBMIT(event);
    }
    uint32_t duration = (k_uptime_get_32() - start);
    if (duration == 0) {
        printk("Duration error!\n");
        k_oops();
    }
    uint64_t i = (BYTES_TO_BE_SENT * 1000) / duration;
    uint64_t f = ((BYTES_TO_BE_SENT * 100000) / duration) % 100;
    printk(" - Bytes sent = %lld, received = %llu \n - Average data rate: "
           "%llu.%lluB/s\n - Duration: %ums\n",
           BYTES_TO_BE_SENT, count, i, f, duration);
    printk("\n@%u\n", duration);
}

K_THREAD_DEFINE(producer_thread_id, 1024, producer_thread, NULL, NULL, NULL, 5, 0, 10);
