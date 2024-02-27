//  SPDX-FileCopyrightText: 2024 Jim Trainor <https://github.com/jptrainor/cmsis-sandbox/issues>
//  SPDX-License-Identifier: Apache-2.0

#include "FftTestRunner.h"

#include "Signal.h"
#include "CmsisTypeFactory.h"
#include "CmsisFft.h"
#include "FftTest.h"

#include <vector>

using namespace fft;

namespace {

  class FftTestRunner {

    const FftTestParams::Tolerance withNoiseTestTolerance = FftTestParams::Tolerance(0.5, 10.0, 20.0);
    const FftTestParams::Tolerance withoutNoiseTestTolerance = FftTestParams::Tolerance(0.1, 0.1, 0.05);
  
    const std::vector<unsigned int> sizes = {32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};

    std::unique_ptr<NameToElapsedTimeMap> resultMap = std::make_unique<NameToElapsedTimeMap>();
    
    void addResult( unsigned int fftSize, bool addNoise, const FftTestResult& result ) {
      std::string key = (addNoise ? "noisy_" : "clean_") + result.name;
      resultMap->try_emplace(key);
      (*resultMap)[key][fftSize] = result.elapsedTime;
    }
    
    void run(unsigned int fftSize, bool addNoise) {

      printf("\nfft size %d\n", fftSize);

      auto signal = std::make_unique<Signal>(fftSize, addNoise);
      double amplitude = signal->getAmplitude();
      CmsisTypeFactory waveform(std::move(signal));

      FftTestParams params;
      if (addNoise) {
	params = FftTestParams(amplitude, withNoiseTestTolerance);
      }
      else {
	params = FftTestParams(amplitude, withoutNoiseTestTolerance);
      }

      if (fftSize < 8192) {
	addResult( fftSize, addNoise, executeFftTest(params, std::move(createFloat64Fft(waveform.toFloat64()))) );
	addResult( fftSize, addNoise, executeFftTest(params, std::move(createFloat32Fft(waveform.toFloat32()))) );
      }

      addResult( fftSize, addNoise, executeFftTest(params, std::move(createQ31Fft(waveform.toQ31()))) );
      addResult( fftSize, addNoise, executeFftTest(params, std::move(createQ15Fft(waveform.toQ15()))) );
    }
  
  public:  

    std::unique_ptr<NameToElapsedTimeMap> runAll() {
      printf("\nwithout noise:\n");
      for(unsigned int size: sizes) {
	run(size, false);
      }

      printf("\nwith noise:\n");
      for(unsigned int size: sizes) {
	run(size, true);
      }

      return std::move(resultMap);
    }
  };
} // namespace

std::unique_ptr<NameToElapsedTimeMap> runAllFftTests() {
  return FftTestRunner().runAll();
}
