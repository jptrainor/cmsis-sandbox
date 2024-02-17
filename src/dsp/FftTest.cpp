#include "FFTTest.h"

#include "CmsisFFT.h"
#include "Ex.h"

#include "Platform.h"

#include <cmath>
#include <memory>
#include <algorithm>
#include <vector>

#include <stdio.h>

namespace {

  class FftTest {
    private:

    FftTestParams params;
    std::unique_ptr<FFT> fft;

    FftTest();

    float sumsq(const std::vector<float>& v) {
      float sum = 0.0;
      std::for_each(v.begin(), v.end(), [&](float x) {  sum += x*x; });
      return sum;
    }
  
    void verifyParsevalEquality(float waveformPower, float fftPower) {
      // sane power values are greater than zero
      if ( !(waveformPower > 0.0 && fftPower > 0.0) ) {
	throw Fail("parsevalEqualityVerify power sanity");
      }

      // compare power within params.tolerance
      float error = 100.0*std::fabs((waveformPower - fftPower)/waveformPower);
      if ( error > params.tolerance.power ) {	
	printf("fail: parsevalEqualityVerify error=%.9g%% (%f vs %f)\n", error, waveformPower, fftPower);
	throw Fail("parsevalEqualityVerify error");
      }
    }

    void verifyFrequencyPeaks(const std::vector<float>& mag) {
      float expectedPeak = params.amplitude * mag.size() / 2.0;

      int peakIndexA = mag.size() / 4;
      float peakA = mag.at(peakIndexA);
      float errorA = 100.0*std::fabs((peakA - expectedPeak)/expectedPeak);
      if ( errorA > params.tolerance.amplitude ) {
	printf("fail: verifyFrequencyPeaks A error=%.9g%% (%f vs %f)\n", errorA, peakA, expectedPeak);
	throw Fail("verifyFrequencyPeaks A error");
      }

      int peakIndexB = 3*peakIndexA;
      float peakB = mag.at(peakIndexB);
      float errorB = 100.0*std::fabs((peakB - expectedPeak)/expectedPeak);
      if ( errorB > params.tolerance.amplitude ) {
	printf("fail: verifyFrequencyPeaks B error=%.9g%% (%f vs %f)\n", errorB, peakB, expectedPeak);
	throw Fail("verifyFrequencyPeaks B error");
      }

      // All other values should be zero. There is rounding noise neari
      // epsilon in the floating point conversion and there is more
      // significant rounding error at zero frequency and the nyquist
      // frequensy in the fixed point conversion. Therefore use a
      // tolerance for the zero compare.
      for(int i = 0; i < mag.size(); i++) {
	if ( i != peakIndexA && i != peakIndexB ) {
	  float zeroError = 100.0*std::fabs(mag.at(i)/expectedPeak);
	  if ( zeroError > params.tolerance.zero ) {
	    printf("fail: verifyFrequencyPeaks zero error at sample %d (%.9g%%)\n", i, zeroError);
	    throw Fail("verifyFrequencyPeaks zero error");
	  }
	}
      }
    }
  
  public:

    FftTest(FftTestParams params, std::unique_ptr<FFT> fft)
      :fft(std::move(fft)),
       params(params)
    {}

    ~FftTest() {};

    FftTestResult execute() {

      // Do this first because the fft modifies the waveform in place.
      float waveformPower = sumsq(*fft->getNormalizedWaveform());
        
      platform::profiling_time_t start = platform::get_profiling_time();
      fft->execute();
      platform::profiling_time_t end = platform::get_profiling_time();
      unsigned long elapsedTime = profiling_time_diff(start,end);

      // don't need the waveform anymore, get the memory back
      fft->deleteWaveform();

	// Get the normalized fft magnitude.
      std::unique_ptr<std::vector<float>> normMag = fft->getNormalizedMagnitude();
      
      float fftPower= sumsq(*normMag)/fft->getLength();    
      verifyParsevalEquality(waveformPower, fftPower);

      verifyFrequencyPeaks(*normMag);

      printf("%s %d us\n", fft->getName().c_str(), (unsigned long)elapsedTime);

      return FftTestResult(fft->getName(), elapsedTime);
    }
  };

} // end namespace

FftTestResult executeFftTest(FftTestParams params, std::unique_ptr<FFT> fft) {
  return FftTest( params, std::move(fft) ).execute();
}
