# Benchmarking

Benchmarking [CMSIS-DSP](https://www.keil.com/pack/doc/CMSIS/DSP/html/index.html)
on the [Rasberry Pi
Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/).

CMSIS-DSP is a digital signal processing library optimized for ARM
processors. It supports floating point and fixed point data types.

Raspberry Pi Pico is a microcontroller development board with a
[RP2040](https://www.raspberrypi.com/products/rp2040/), both designed
by Raspberry Pi.

The RP2040 is a dual core [ARM
Cortext-M0+](https://developer.arm.com/Processors/Cortex-M0-Plus)
microcontroller. The Cortex-M0+ processor has no floating point
support and no vector processing support. The RP2040, however, does
have optimized "fast floating point" software built into its
bootrom. See the [RP2040 data
sheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
for more information.

This project benchmarks the performance of CMSIS-DSP:

* FFT
* FIR decimation

Using the following CMSIS-DSP data types:

* 64 bit float (`float32_t`)
* 32 bit float (`float64_t`)
* Q1.31 bit fixed point (`q31_t`)
* Q1.15 bit fixed point (`q15_t`)

The cmsis-sandbox project is implemented in C++ and also serves as a
working example of wrapping the CMSIS-DSP C library in C++.

# FFT Benchmark

The FFT benchmark times the performance of all the FFT sizes and types
supported by CMSIS-DSP. The benchmark computes the real fft and the
magnitude of the complex result up to the Nyquist frequency. The
CMSIS-DSP functions are:

* arm_rfft_fast_{f64,f32}, and arm_rfft_{q31,q15}
* arm_cmplx_mag_{f64,f32,q31,q15}

The input waveform is a clean single frequency sine wave at half the
Nyquist frequency, and a noisy version of the same signal. The
benchmarks perform simple tests to verify the sanity of results of the
operations. The tests are not part of the profiled code. The tests
are:

* Verify that the sum squared power of the input and output signals
  match (Parseval's Identify).
* Verify the peak frequency location matches the frequency of the
  single frequency sine wave.
* Verify that all other frequencies are zero (or near zero, in the
  noise case).

## FFT Results

````
fft execution time (us)

               32     64    128    256    512   1024   2048   4096   8192
 clean_f32    608   1103   1973   4265   9125  18588  40787  87312
 clean_f64    890   1855   3810   8461  17878  38877  81487 176504
 clean_q15    259    331    511    869   1557   3116   6139  12889  26259
 clean_q31    382    549    927   1816   3336   7072  13826  29803  59605
 noisy_f32    610   1112   1957   4204   8936  18158  39838  85399
 noisy_f64    903   1808   3690   8019  16911  36769  77231 167515
 noisy_q15    351    535    947   1741   3321   6629  13208  27003  54500
 noisy_q31    481    764   1380   2697   5101  10625  20885  43919  87871
````

# Decimation Benchmark

CMSIS-DSP has optimized [FIR
decimation](https://www.keil.com/pack/doc/CMSIS/DSP/html/group__FIR__decimate.html)
functions. The CMSIS-DSP functions are:

* arm_fir_decimate_f32
* arm_fir_decimate_{q15,q31}
* arm_fir_decimate_fast_{q15,q31}

The benchmark times decimation using a 31 tap decimation filter. The
decimation filters were create using the [GNU
Octave](https://octave.org/) command `fir1(30, M)` where M is the
decimation factor.

The input waveform is a clean single frequency sine wave at half the
output (decimated) Nyquist frequency. Pre-scaling of the fixed point
waveforms is done outside of the the profiled decimation calls. The
decimation result is verified to ensure that the input waveform
frequency appears at the expected frequency in the decimated output
waveform.

## Decimation Results

```
decimation execution time (us)

M=2
              512   1024   2048   4096   8192
       f32   9957  19736  39345  78650 157233
       q15   1629   3089   6087  12099  24074
  q15_fast    736   1368   2637   5175  10252
       q31   3345   6493  12846  25553  50948
  q31_fast   3620   7028  13913  27696  55225

M=4
              512   1024   2048   4096   8192
       f32   5069  10048  19996  39884  79639
       q15    876   1653   3184   6290  12465
  q15_fast    416    764   1462   2816   5538
       q31   1750   3349   6560  12967  25772
  q31_fast   1881   3616   7101  14057  27931

M=8
              512   1024   2048   4096   8192
       f32   2607   5159  10258  20393  40672
       q15    478    883   1682   3241   6398
  q15_fast    256    428    793   1513   2919
       q31    966   1775   3424   6713  13262
  q31_fast   1015   1901   3689   7253  14351
```

# Build

Clone the Raspberry Pi Pico SDK repository

```
git clone --depth 1 --branch "1.5.1"  https://github.com/raspberrypi/pico-sdk.git
git submodule update --init  # for optional tinyUSB support
export PICO_SDK_PATH=.../pico-sdk
```

Clone and build this project. Note that the cmsis-sandbox fetches the
CMSIS-DSP automatically as part of its build.

````
git clone https://github.com/jptrainor/cmsis-sandbox.git
mkdir build
cd build
cmake ../cmsis-sandbox/src
make -j4
````

## Load via OpenOCD and monitor with the UART serial port.

Execute the code using the [Rasberry Pi Debug
Probe](https://www.raspberrypi.com/products/debug-probe/) and [OpenOCD
for RP2040](https://www.raspberrypi.com/products/debug-probe/).

You'll need to build OpenOcd with RP2040 support:

```
# build OpenOcd with RP2040 support
git clone --depth 1 -b rp2040-v0.12.0 https://github.com/raspberrypi/openocd.git
./configure --preefix=<openocd_instal_dir>
make -j4

# Download the cmsis-sandbox.elf file
<openocd_install_dir>/bin/openocd  -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program cmsis-sandbox.elf verify reset exit"

# Connect to the Pico's UART serial port via the Pi debug probe
minicom -b 115200 -D /dev/cu.usbmodem14644202  # This is the MacOS serial device. Yours may vary.
```

## Load via USB and monitor with the USB serial port.

Alternatively, use the USB port. To do this

# usb serial port
# describe cmake file change
drag and drop utf file
minicom -D /dev/cu.usbmodem14644201  # This is the  MacOS serial device. Yours may vary.
```

USB support is disabled by default. It has to be enabled in the
project's `CMakeLists.txt` file. Look for the
`pico_enable_stdio_{uart,usb}` calls:

```
% grep pico_enable_stdio  ../cmsis-sandbox/src/CMakeLists.txt
pico_enable_stdio_uart(cmsis-sandbox 1) # set to one to enable (default)
pico_enable_stdio_usb(cmsis-sandbox 0)  # set to one to enable
```

Note that USB won't work unless TinyUSB is enabled in the pico-sdk
build. That requires executing `git submodule update --init` in the
pico-sdk repository before building it (as noted above).

# Addendum

I use MacOS and [MacPorts](https://www.macports.org/) for
development. I tested using the MacPorts distribution of the
`arm-non-eabi-gcc` compiler and related tools.

```
sudo port install  arm-none-eabi-gcc arm-none-eabi-binutils arm-none-eabi-gdb
```

