function s = signal(N, k, addNoise = false)

# Usage: signal(N, k, addNoise = false)
#
# Signal is an N sample sine wave with optional added noise. Without
# noise the amplitude of the sine way is 1.0. With noise the sine
# wave amplitude is 0.75 and the noise is distributed over [-0.25,
# 0.25]. Either way, the range of the signal is [-1, 1].
#
# The sine wave is created using the formula sin(pi/k * n) where k is
# the nyquist frequency divisor and n is the sample index. If k=1
# then the waveform is generated at the nyquist frequency. If k>1 then
# the wave is generated at a fraction of the nyquist frequency. For
# example, k=2 is half the nyquist frequency. To generate one full
# period of the sine wave over N samples use k=N/2.
#
# The value of k is not validated. If k=1 the resulting waveform won't
# be a recognizable sine wave because a discreet sin wave cannot be
# represented at the nyquist frequency. Values of k<1 produce aliased
# waveforms. Non integer values of k produce a waveform with unevenly
# sample amplitude. This is particularly noticable when 1<k<2.
 
  s = sin([0:1:(N-1)]*pi/k);
  if ( addNoise == true )
    s = 0.7*s + [0.5*rand(1, N) - 0.25];
  endif
endfunction
