//  SPDX-FileCopyrightText: 2024 Jim Trainor <https://github.com/jptrainor/cmsis-sandbox/issues>
//  SPDX-License-Identifier: Apache-2.0

#ifndef PICO_CMSIS_SANDBOX_PICOPLATFORM_H_INCLUDED
#define PICO_CMSIS_SANDBOX_PICOPLATFORM_H_INCLUDED

#include "pico/stdlib.h"

namespace platform {
  struct profiling_time_t {
    absolute_time_t t;
  };

  // platform dependent init, return value 0 is success
  void init();

  // timestamp in us
  profiling_time_t get_profiling_time();

  // timestamp delta in us
  unsigned long profiling_time_diff(const profiling_time_t& start, const profiling_time_t& end); 
}

#endif
