//  SPDX-FileCopyrightText: 2024 Jim Trainor <https://github.com/jptrainor/cmsis-sandbox/issues>
//  SPDX-License-Identifier: Apache-2.0

#ifndef PICO_CMSIS_SANDBOX_DECIMATETESTRUNNER_H_INCLUDED
#define PICO_CMSIS_SANDBOX_DECIMATETESTRUNNER_H_INCLUDED

#include <memory>
#include <map>

namespace decimate {
  // map size to elapsed time
  typedef std::map<unsigned int, unsigned long> SizeToElapsedTimeMap;

  // map decimation factor to elapsed time map
  typedef std::map<unsigned int, SizeToElapsedTimeMap> FactorToElapsedTimeMap;

  // map decimation factor to elapsed time map
  typedef std::map<std::string, FactorToElapsedTimeMap> NameToFactorElapsedTimeMap;
}

std::unique_ptr<decimate::NameToFactorElapsedTimeMap> runAllDecimateTests();

#endif
