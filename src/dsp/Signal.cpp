#include "Signal.h"

#include "Ex.h"

#include <cmath>

/*
Signal is an N sample sine wave with optional added noise. Without
noise the amplitude of the sine way is 1.0. With noise the sine wave
amplitude is 0.75 and the noise is distributed between -0.25 and
0.25. Either way, the range of the signal is [-1, 1].

The sine wave is created using sin(pi/k * n) where k is the nyquist
frequencey divisor and n is the sample index. If k=1 then the waveform
is generated at the nyquist frequency. If k>1 then the wave is
generated at a fraction of the nyquist frequency. For example, k=2 is
half the nyquist frequency. To generate one full period of the sine
wave use k=N/2.

The added noise is uniformly distributed over the range [-0.25, 0.25].
  
Given N samples per T seconds:

a) sampling frequency is:
fs=N/T

b) nyquist frequency is:
fq=N/(2*T)

c) waveform frequency is:
fw=N/(2*T*k) where k > 1

d) deltaT is:
deltaT = 1/fs
deltaT = T/N

e) time t as function of sample number is:
t = n*deltaT
t = n*T/N

f) sine wave as function of time is:
s(t) = sin(2*pi*fw*t)
s(t) = sin(2*pi*N/(2*T*k)*t)
s(t) = sin(pi*N/(T*k)*t)

g) sine wave as a function of normalized time is:
s(n*T/N) = sin(pi*N/(T*k)*n*(T/N))
s(n*T/N) = sin(pi/k * n)

h) at one cycle of s(t) over T:
T = 2*pi
s(T) = sin(pi*N/k)
s(2*pi) = sin(pi*N/k)
2*pi = pi*N/k
2 = N/k
k = N/2 produces one period of s(t) in N samples

i) At half the nyquist frequency:
fw = fq/2
fw = N/(4*T) (substitue fq from b)
fw = N/(2*T*k) (where k > 1 (from fw from c)
N/(2*T*k) = N/(4*T*k) (equality of the two fw relationships)
k = 2 produces a waveform at half the nyquist frequency
*/

namespace {

  static double clampK(int N, double k) {
    if (k < 1.0d) {
      return 1.0d;
    }
    else if (k > N/2.0d) {
      return N/2.0d;
    }
    else {
      return k;
    }
  }

} // namespace

Signal::Signal(unsigned int numSamples)
  : Signal(numSamples, 2.0, true)
{}

Signal::Signal(int numSamples, bool addNoise)
  : Signal(numSamples, 2.0, addNoise)
{}

Signal::Signal(int numSamples, double k, bool addNoise)
  : k(clampK(numSamples, k)),
    numSamples(numSamples),
    addNoise(addNoise),
    amplitude(addNoise ? 0.75 : 1.0)
{}
  
unsigned int Signal::size() const {
  return numSamples;
}

double Signal::getNuquistDivider() const {
  return k;
}

double Signal::getFrequency() const {
  return numSamples / (2.0 * k);
}

double Signal::getAmplitude() const {
  return amplitude;
}

bool Signal::getAddNoise() const {
  return addNoise;
}

double Signal::next() {
  if (isEnd()) {
    throw new Ex("end of signal");
  }

  double signal = amplitude*sin((n++)*M_PI/k);

  if (addNoise) {
    std::uniform_real_distribution<double>::param_type range(-0.25, 0.25);
    return signal + dist(rand,range);
  }
  else {
    return signal;
  }
}

bool Signal::isEnd() const {
  return n == numSamples;
}

void Signal::reset() {
  n = 0;
}
