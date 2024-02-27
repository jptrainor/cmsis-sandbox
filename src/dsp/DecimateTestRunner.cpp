//  SPDX-FileCopyrightText: 2024 Jim Trainor <https://github.com/jptrainor/cmsis-sandbox/issues>
//  SPDX-License-Identifier: Apache-2.0

#include "DecimateTestRunner.h"

#include "DecimateTest.h"
#include "CmsisDecimate.h"
#include "CmsisTypeFactory.h"
#include "FirSource.h"
#include "DecimateFIR.h"
#include "Signal.h"
#include "Ex.h"

#include <arm_math.h>

#include <algorithm>

using namespace decimate;

namespace {

  // The arm_fir_decimate_{fast_q15, q31, fast_q31}() functions
  // documents conditions underwhich the filter result can overflow
  // intermediate results (the filter accumulator) and corrupt the
  // result.
  //
  // It says: "In order to avoid overflows completely the input signal
  // must be scaled down by log2(numTaps) bits ...". The caller has to
  // know the range of their q15_5 data, if scaling is necessary, and
  // how much scaling is necessary. This test uses a real test signal
  // with range [-1.0,1.0], therefore uses the full range of the q15
  // fixed point. Hence must be scalled down by the full log2(numBits).
  //
  // The decimated result retains this scaling. So, if the original
  // waveform is scaled down by 32 (5 bits), then the decimate result
  // will have the same scaling.
  //
  // Note that in the Pico Pi uses cases, 12 bit adc values are stored
  // in the least significant bits of 16 bit word. Hence, that use case
  // would only need 1 bit of scaling to avoid decimation overflow.

  class DecimateTestRunner {

    const std::vector<unsigned int> sizes = {512, 1024, 2048, 4096, 8192};
    const std::vector<unsigned int> decimationFactors = {2, 4, 8};

    std::unique_ptr<NameToFactorElapsedTimeMap> resultMap = std::make_unique<NameToFactorElapsedTimeMap>();

    void addResult(unsigned int waveformSize, unsigned int M, const DecimateTestResult& result) {
      resultMap->try_emplace(result.name);
      (*resultMap)[result.name].try_emplace(M);
      (*resultMap)[result.name][M][waveformSize] = result.elapsedTime;
    }
    
    void run(unsigned int waveformSize, unsigned int decimationFactor) {
      unsigned int k = 2*decimationFactor;
      unsigned int M = decimationFactor;

      CmsisTypeFactory firFactory(std::move(createFirSource(std::move(getDecimationFIR(M)))));

      // Scaling for arm_fir_decimate_* that require scaling to avoid
      // overflow.
      const unsigned int numTaps = firFactory.getSource().size();
      const unsigned int rshift = (unsigned int)std::ceil(std::log2(numTaps));

      printf("\ndecimate waveform size %d, filter size %d, M=%d\n", waveformSize, firFactory.getSource().size(), M);
    
      auto signal = std::make_unique<Signal>(waveformSize, (double)k, false);
      CmsisTypeFactory signalFactory(std::move(signal));

      {
	auto decimator = createFloat32Decimate(std::move(firFactory.toFloat32()), std::move(signalFactory.toFloat32()), M);
	addResult( waveformSize, M, executeDecimateTest(k, std::move(decimator)) );
      }

      {
	// See arm_fir_decimate_q31() scaling requirements.
	auto decimator = createQ31Decimate(std::move(firFactory.toQ31()), std::move(signalFactory.toQ31(rshift)), M, false);
	addResult( waveformSize, M, executeDecimateTest(k, std::move(decimator)) );
      }

      {
	// See arm_fir_decimate_fast_q31() scaling requirements.
	auto decimator = createQ31Decimate(std::move(firFactory.toQ31()), std::move(signalFactory.toQ31(rshift)), M, true);
	addResult( waveformSize, M, executeDecimateTest(k, std::move(decimator)) );
      }

      // arm_fir_decimate_q15() has no scaling requirments.
      {
	auto decimator = createQ15Decimate(std::move(firFactory.toQ15()), std::move(signalFactory.toQ15()), M, false);
	addResult( waveformSize, M, executeDecimateTest(k, std::move(decimator)) );
      }

      {
	// See arm_fir_decimate_fast_q15() scaling requirements.
	auto decimator = createQ15Decimate(std::move(firFactory.toQ15()), std::move(signalFactory.toQ15(rshift)), M, true);
	addResult( waveformSize, M, executeDecimateTest(k, std::move(decimator)) );
      }
    }
  
  public:

    std::unique_ptr<NameToFactorElapsedTimeMap> runAll() {
      for(unsigned int size: sizes) {
	for (unsigned int M: decimationFactors) {
	  run(size, M);
	}
      }

      return std::move(resultMap);
    }
  };

} // namespace

std::unique_ptr<NameToFactorElapsedTimeMap> runAllDecimateTests() {
  return DecimateTestRunner().runAll();
}
