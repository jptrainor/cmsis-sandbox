//  SPDX-FileCopyrightText: 2024 Jim Trainor <https://github.com/jptrainor/cmsis-sandbox/issues>
//  SPDX-License-Identifier: Apache-2.0

#include "PicoPlatform.h"

namespace platform {

  void init() {
    stdio_init_all();

    // This is a workaround for get_absolute_time() returning zero for
    // all calls if there is not a small delay at startup.
    sleep_ms(100);
  }

  // timestamp in us
  profiling_time_t get_profiling_time() {
    profiling_time_t prof;
    prof.t = get_absolute_time();
    return prof;
  }

  // timestamp delta in us
  unsigned long profiling_time_diff(const profiling_time_t& start, const profiling_time_t& end) {
    return (unsigned int)absolute_time_diff_us(start.t,end.t);
  }
}
