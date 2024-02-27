//  SPDX-FileCopyrightText: 2024 Jim Trainor <https://github.com/jptrainor/cmsis-sandbox/issues>
//  SPDX-License-Identifier: Apache-2.0

#ifndef PICO_CMSIS_SANDBOX_FFTTEST_INCLUDED
#define PICO_CMSIS_SANDBOX_FFTTEST_INCLUDED

#include <memory>
#include <string>

class FFT;

class FftTestParams {
public:
  struct Tolerance {
    float power = 0.0;
    float amplitude = 0.0;
    float zero = 0.0;
    
    Tolerance(const Tolerance& other)
      : Tolerance(other.power, other.amplitude, other.zero)
    {}
    
      Tolerance()
    {}
    
    Tolerance(float power, float amplitude, float zero)
      : power(power), amplitude(amplitude), zero(zero)
    {}

  };

  float amplitude = 0.0;
  Tolerance tolerance;
  
  FftTestParams()
  {}
  
  FftTestParams(const FftTestParams& params)
    : FftTestParams(params.amplitude, Tolerance(params.tolerance))
  {}
  
  FftTestParams(float amplitude, Tolerance tolerance)
    : amplitude(amplitude),
      tolerance(tolerance)
  {}
};

struct FftTestResult {
  const std::string name;
  const unsigned long elapsedTime;

  FftTestResult(const std::string& name, unsigned long elapsedTime)
    :  name(name),
       elapsedTime(elapsedTime)
  {}
};

FftTestResult executeFftTest(FftTestParams params, std::unique_ptr<FFT> fft);

#endif
