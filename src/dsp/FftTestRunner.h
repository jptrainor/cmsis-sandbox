//  SPDX-FileCopyrightText: 2024 Jim Trainor <https://github.com/jptrainor/cmsis-sandbox/issues>
//  SPDX-License-Identifier: Apache-2.0

#ifndef PICO_CMSIS_SANDBOX_FFTTESTRUNNER_INCLUDED
#define PICO_CMSIS_SANDBOX_FFTTESTRUNNER_INCLUDED

#include <map>
#include <memory>
#include <string>

namespace fft {
  // map size to elapsed time in us
  typedef std::map<unsigned int, unsigned long> SizeToElapsedTimeMap;

  // map name to size/time map
  typedef std::map<std::string, SizeToElapsedTimeMap> NameToElapsedTimeMap;
}

std::unique_ptr<fft::NameToElapsedTimeMap> runAllFftTests();

#endif
