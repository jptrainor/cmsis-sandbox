#ifndef PICO_CMSIS_SANDBOX_CMSISFFT_H_INCLUDED
#define PICO_CMSIS_SANDBOX_CMSISFFT_H_INCLUDED

#include "arm_math.h"

#include <memory>
#include <string>
#include <vector>

/**
The CMSIS-DSP real fft functions are: arm_rfft_fast_f{32,64}, and
arm_rfft_q{15,31}. Where the "f" functions are floating point, and the
"q" functions are fixed point.

arm_rfft_q{15,31} takes a real valued fixed point input waveform
(array) with N samples and produces an fft output with N complex pairs
packed into an array of size 2*N values. The supported fft lengths are
32, 64, 128, 256, 512, 1024, 2048, 4096, and 8192.

arm_rfft_fast_f{32,64} takes a real valued floating point input
waveform (array) with N samples and produces an fft output with N/2
complex pairs packed into an array of size N values. The supported fft
lengths are 32, 64, 128, 256, 512, 1024, 2048, and 4096.

The complex output values are packed in the output array such that
even array indices are real valued, and the following odd array entry
is the imaginary value of one complex pair.

Note that the input waveform data is modifed by the
arm_rfft_fast_f{32,64}, and arm_rfft_q{15,31} functions hence is not
useful after executing FFT processing.

Note that first pair of values in the fft output produced by
arm_rfft_fast_f{32,64} is a special case. This pair of values is not a
complex pair. The FFT output value at index 0 is the real valued zero
frequency component of the output, and the complex value is implicitly
zero. The output value at index 1 is the real value of the N/2
frequency component (i.e. the Nyquist frequency) and the complex value
is implicitly zero. This wrapper explicitly overwrites the index 1
value with a zero. The prior value is saved to
"nyquistFrequencyComponent", and this is available via a getter.
*/


class FFT {

public:

  virtual ~FFT() {}

  // execute fft processing (note, waveform is modified)
  virtual void execute() = 0;

  // the name of the FFT implementation
  virtual const std::string& getName() const = 0;

  // the waveform length (valid before and after execute())
  virtual unsigned int getLength() const = 0;

  // get the normalized waveform, fixed point values are scaled to
  // real (before execute)
  virtual std::unique_ptr<std::vector<float>> getNormalizedWaveform() const = 0;

  // get the fft magnitude normalized such that fixed point magnitude
  // matches scale of the real fftover the full frequency band (after
  // execute())
  virtual std::unique_ptr<std::vector<float>> getNormalizedMagnitude() const = 0;

  // Delete the waveform to free memory (optionally, after execute());
  virtual void deleteWaveform() = 0;
  
  // dump the fft magnitudes (after execute())
  virtual void dump() const = 0;
};

std::unique_ptr<FFT> createFloat64Fft(std::unique_ptr<std::vector<float64_t>> waveform);
std::unique_ptr<FFT> createFloat32Fft(std::unique_ptr<std::vector<float32_t>> waveform);
std::unique_ptr<FFT> createQ31Fft(std::unique_ptr<std::vector<q31_t>> waveform);
std::unique_ptr<FFT> createQ15Fft(std::unique_ptr<std::vector<q15_t>> waveform);

#endif
