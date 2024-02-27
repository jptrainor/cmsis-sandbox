//  SPDX-FileCopyrightText: 2024 Jim Trainor <https://github.com/jptrainor/cmsis-sandbox/issues>
//  SPDX-License-Identifier: Apache-2.0

#ifndef PICO_CMSIS_SANDBOX_SIGNAL_INCLUDED
#define PICO_CMSIS_SANDBOX_SIGNAL_INCLUDED

#include "Source.h"

#include <memory>
#include <random>

class Signal : public Source {

  // nyquist divider (1 < k <= N/2)
  const double k;

  const unsigned int numSamples;

  const bool addNoise;

  // sine amplitude
  const float amplitude;

  // noise source
  std::uniform_real_distribution<double> dist;
  std::default_random_engine rand;

  // sample counter
  unsigned int n=0;
  
 public:

  virtual ~Signal() {};
  
  // Construct a numSamples waveform that is a sine wave at half the
  // nyquist frequency. Uniformly distributed noise with range [-1,1]
  // is added to the sine wave.
  Signal(unsigned int numSamples);

  // Construct a numSamples waveform that is a sine wave at half the
  // nyquist frequency. Optionally add uniformly distributed noise
  // with range [-1,1] is added to the sine wave.
  Signal(int numSamples, bool addNoise);

  // Construct a waveform: sin(n*M_PI/k) Where n is the sample index
  // from 0 to numSamples-1.  And k divides the nyquist frequency.
  // The k value is clamped in the range 1 <= k <= N/2. The value
  // k=N/2 produces a wave form of one full period over
  // numSamples. The value k=1 is exactly the nyquist rate. Optionally
  // add uniformly distributed noise with range [-1,1] is added to the
  // sine wave.
  Signal(int numSamples, double k, bool addNoise);
  
  // Number of samples
  virtual unsigned int size() const;

  // Nyquist divider
  double getNuquistDivider() const;

  // Waveform frequency in normalized time: N/(2*k)
  double getFrequency() const;

  // The sine wave amplitude.
  double getAmplitude() const;

  bool getAddNoise() const;

  // Return the next sample. Throws and exception if called more than
  // numSamples times.
  virtual double next();

  // True after next() called numSamples times.
  virtual bool isEnd() const;

  // Reset the sample counter to zero.
  virtual void reset();
};

#endif
