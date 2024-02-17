#include "DecimateTest.h"

#include "CmsisDecimate.h"
#include "CmsisFFT.h"
#include "WindowFunction.h"
#include "Ex.h"

#include "Platform.h"

namespace {

  class DecimateTest {

    unsigned int k;
    std::unique_ptr<Decimate> decimator;

    // Return the index of the maximum value in the first half (below
    // the nyquist frequency) of the magnitude vector.
    unsigned int findMaxIndex(const std::vector<float>& mag) {
      float maxValue = mag.at(0);
      int maxIndex = 0;
    
      for(int i = 0; i < mag.size()/2; i++) {
	if (mag.at(i) > maxValue) {
	  maxValue = mag.at(i);
	  maxIndex = i;
	}
      }

      return maxIndex;
    }
  
    void verify() {
      unsigned int M = decimator->getM();
      std::unique_ptr<std::vector<float>> result = decimator->getResult();

      std::unique_ptr<WindowFunction> hanning = createHanningWindow(result->size());
      for(int i = 0; i < result->size(); i++) {
	result->at(i) = result->at(i) * hanning->at(i);
      }
    
      auto fft = createFloat32Fft(std::move(result));
      auto waveform = fft->getNormalizedWaveform();
      fft->execute();
      auto mag = fft->getNormalizedMagnitude();

      // The max frequency doesn't change due to decimation. If it was
      // 1/2k in the original signal (k as defined by the Signal class),
      // then it normalizes to M/k in the decimated waveform. Therefore
      // the the expected index in the decimated waveform is M*Ld/2k
      // where M is the decimation factor, Ld is decimated waveform
      // length, and 1/2k is the frequency of the original signal.
      unsigned int expectedMaxIndex = M * waveform->size() / (2*k);

      // And the actual is the max magnitude component.
      unsigned int actualMaxIndex = findMaxIndex(*mag);

      if ( actualMaxIndex != expectedMaxIndex ) {
	printf("FAIL actualMaxIndex != expectedMaxIndex (%d != %d)\n", actualMaxIndex, expectedMaxIndex);
	throw Fail("actualMaxIndex != expectedMaxIndex");
      }
    }
  
  public:

    DecimateTest(unsigned int k, std::unique_ptr<Decimate> decimator)
      : k(k),
	decimator(std::move(decimator))
    {}
  
    DecimateTestResult execute() {
      platform::profiling_time_t start = platform::get_profiling_time();
      decimator->execute();
      platform::profiling_time_t end = platform::get_profiling_time();
      unsigned long elapsedTime = profiling_time_diff(start,end);

      verify();

      printf("%s %ld us\n", decimator->getName().c_str(), elapsedTime);

      return DecimateTestResult(decimator->getName(), elapsedTime);
    }
  };

} // namespace

DecimateTestResult executeDecimateTest(unsigned int k, std::unique_ptr<Decimate> decimator) {
  return DecimateTest(k, std::move(decimator)).execute();
}

