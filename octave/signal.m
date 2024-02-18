function s = signal(N, k, addNoise = false)

# Usage: signal(N, k, addNoise = false)
#
# Signal is an N sample sine wave with optional added noise. Without
# noise the amplitude of the sine way is 1.0. With noise the sine
# wave amplitude is 0.75 and added noise is distributed over [-0.25,
# 0.25]. Either way, the range of the signal is [-1, 1].
#
# The sine wave is created using the formula sin(pi/k * n) where k is
# the Nyquist frequency divisor and n is the sample index. If k=1
# then the waveform is generated at the Nyquist frequency. If k>1 then
# the wave is generated at a fraction of the Nyquist frequency. For
# example, k=2 is half the Nyquist frequency. To generate one full
# period of the sine wave over N samples use k=N/2.
#
# The value of k is not validated. If k=1 the resulting waveform won't
# be a recognizable sine wave because a discrete sin wave cannot be
# represented at the Nyquist frequency. Values of k<1 produce aliased
# waveforms. Non-integer values of k produce a waveform with unevenly
# sample amplitude. This is particularly noticable when 1<k<2.
#
# This matches the test waveform by the cmsis-sandbox benchmarks.
 
  s = sin([0:1:(N-1)]*pi/k);
  if ( addNoise == true )
    s = 0.7*s + [0.5*rand(1, N) - 0.25];
  endif
endfunction
