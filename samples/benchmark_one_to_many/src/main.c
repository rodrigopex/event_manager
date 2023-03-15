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

LOG_MODULE_DECLARE(app_event_manager, CONFIG_APP_EVENT_MANAGER_LOG_LEVEL);

#if defined(CONFIG_ARCH_POSIX)
#include "native_rtc.h"
#define GET_ARCH_TIME_NS() (native_rtc_gettime_us(RTC_CLOCK_PSEUDOHOSTREALTIME) * NSEC_PER_USEC)
#else
#define GET_ARCH_TIME_NS() (k_cyc_to_ns_near64(sys_clock_cycle_get_32()))
#endif

#define PRODUCER_STACK_SIZE (CONFIG_MAIN_STACK_SIZE + CONFIG_BM_MESSAGE_SIZE)

long int count = 0;

static bool s_cb(const struct app_event_header *aeh)
{
    if (is_measurement_event(aeh)) {
        struct measurement_event *me = cast_measurement_event(aeh);
        (void) me;
        atomic_add(&count, CONFIG_BM_MESSAGE_SIZE);
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
    uint64_t start_ns = GET_ARCH_TIME_NS();
    for (uint64_t internal_count = BYTES_TO_BE_SENT / CONFIG_BM_ONE_TO; internal_count > 0;
         internal_count -= CONFIG_BM_MESSAGE_SIZE) {
        struct measurement_event *event = new_measurement_event();
        memcpy(event->bytes, msg_to_be_sent, CONFIG_BM_MESSAGE_SIZE);
        APP_EVENT_SUBMIT(event);
    }

    uint64_t end_ns = GET_ARCH_TIME_NS();

    uint64_t duration = end_ns - start_ns;
    if (duration == 0) {
        LOG_ERR("Something wrong. Duration is zero!\n");
        k_oops();
    }
    uint64_t i = ((BYTES_TO_BE_SENT * NSEC_PER_SEC) / MB(1)) / duration;
	uint64_t f = ((BYTES_TO_BE_SENT * NSEC_PER_SEC * 100) / MB(1) / duration) % 100;

	LOG_INF("Bytes sent = %lld, received = %lu", BYTES_TO_BE_SENT, atomic_get(&count));
	LOG_INF("Average data rate: %llu.%lluMB/s", i, f);
	LOG_INF("Duration: %llu.%llus", duration / NSEC_PER_USEC, duration % NSEC_PER_USEC);

	printk("\n@%llu\n", duration);
}

K_THREAD_DEFINE(producer_thread_id, PRODUCER_STACK_SIZE, producer_thread,
NULL, NULL, NULL, 5, 0, 0);
