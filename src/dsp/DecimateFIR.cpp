//  SPDX-FileCopyrightText: 2024 Jim Trainor <https://github.com/jptrainor/cmsis-sandbox/issues>
//  SPDX-License-Identifier: Apache-2.0

#include "DecimateFir.h"

#include "Ex.h"

#include <array>

namespace {

  // Half bandwidth decimation filter created using Octave command:
  // fir1(30, 1/2)
  const std::array<double, 31> decimationFIR_2 = {
    -1.6991e-03,
    8.8078e-05,
    2.9356e-03,
    -1.6417e-04,
    -6.7271e-03,
    3.0320e-04,
    1.4089e-02,
    -4.8113e-04,
    -2.6779e-02,
    6.6719e-04,
    4.9092e-02,
    -8.2921e-04,
    -9.6929e-02,
    9.3918e-04,
    3.1560e-01,
    4.9980e-01,
    3.1560e-01,
    9.3918e-04,
    -9.6929e-02,
    -8.2921e-04,
    4.9092e-02,
    6.6719e-04,
    -2.6779e-02,
    -4.8113e-04,
    1.4089e-02,
    3.0320e-04,
    -6.7271e-03,
    -1.6417e-04,
    2.9356e-03,
    8.8078e-05,
    -1.6991e-03
  };

  // Quarter bandwidth decimation filter created using Octave command:
  // fir1(30, 1/4)
  const std::array<double, 31> decimationFIR_4 = {
    -1.2584e-03,
    -2.0521e-03,
    -1.9955e-03,
    1.6439e-04,
    4.9238e-03,
    9.8927e-03,
    9.7002e-03,
    -4.8176e-04,
    -1.9368e-02,
    -3.6289e-02,
    -3.4225e-02,
    8.3030e-04,
    6.9261e-02,
    1.5326e-01,
    2.2277e-01,
    2.4974e-01,
    2.2277e-01,
    1.5326e-01,
    6.9261e-02,
    8.3030e-04,
    -3.4225e-02,
    -3.6289e-02,
    -1.9368e-02,
    -4.8176e-04,
    9.7002e-03,
    9.8927e-03,
    4.9238e-03,
    1.6439e-04,
    -1.9955e-03,
    -2.0521e-03,
    -1.2584e-03
  };

  // Eigth bandwidth decimation filter created using Octave command:
  // fir1(30, 1/8)
  const std::array<double, 31> decimationFIR_8 = {
    -7.1969e-04,
    -1.5055e-03,
    -2.7462e-03,
    -4.4400e-03,
    -6.1043e-03,
    -6.7454e-03,
    -5.0127e-03,
    4.7926e-04,
    1.0737e-02,
    2.5997e-02,
    4.5466e-02,
    6.7304e-02,
    8.8863e-02,
    1.0715e-01,
    1.1941e-01,
    1.2373e-01,
    1.1941e-01,
    1.0715e-01,
    8.8863e-02,
    6.7304e-02,
    4.5466e-02,
    2.5997e-02,
    1.0737e-02,
    4.7926e-04,
    -5.0127e-03,
    -6.7454e-03,
    -6.1043e-03,
    -4.4400e-03,
    -2.7462e-03,
    -1.5055e-03,
    -7.1969e-04
  };
  
} // namespace

std::unique_ptr<std::vector<double>> getDecimationFIR(unsigned int M) {
  switch(M) {
  case 2:
    return std::make_unique<std::vector<double>>(decimationFIR_2.cbegin(), decimationFIR_2.cend());
  case 4:
    return std::make_unique<std::vector<double>>(decimationFIR_4.cbegin(), decimationFIR_4.cend());
  case 8:
    return std::make_unique<std::vector<double>>(decimationFIR_8.cbegin(), decimationFIR_8.cend());
  default:
    throw Ex("unsupported decimation factor");
  }
}
