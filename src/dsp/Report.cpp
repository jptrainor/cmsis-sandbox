#include "Report.h"

#include <set>

// Table of fft execution times.
void reportFftResults(const fft::NameToElapsedTimeMap& fftResultMap) {
  std::set<unsigned int> sizes;

  for (auto const& [name, sizeMap] : fftResultMap) {
    for (auto const& [size, elapsedTime] : sizeMap) {
      sizes.insert(size);
    }
  }

  printf("\nfft execution time (us)\n\n");
  printf("%10s", "");
  for (auto size: sizes) {
    printf("%7d", size);
  }
  printf("\n");
  
  for (auto const& [name, sizeMap] : fftResultMap) {
    printf("%10s", name.c_str());
    for (auto const& [size, elapsedTime] : sizeMap) {
      printf("%7d", elapsedTime);
    }
    printf("\n", name.c_str());
  }
}

// Table of decimation execution times.
void reportDecimateResults(const decimate::NameToFactorElapsedTimeMap& decimateResultMap) {

  std::set<unsigned int> sizes;
  std::set<unsigned int> factors;

  // gather sizes and decimation factors
  for(auto const& [name, factorMap]: decimateResultMap) {
    for(auto const& [M, elapsedTimeMap]: factorMap) {
      factors.insert(M);
      for(auto const& [size, elapsedTime]: elapsedTimeMap) {
	sizes.insert(size);
      }
    }
  }

  printf("\ndecimation execution time (us)\n\n");

  for (const unsigned int M: factors) {
    printf("M=%d\n", M);
    printf("%10s", "");
    for (unsigned int size: sizes) {
      printf("%7d", size);
    }
    printf("\n");
    
    for(const auto& [name, factorMap]: decimateResultMap) {
      printf("%10s", name.c_str());
      for(const auto& [size, elapsedTime]: factorMap.at(M)) {
	printf("%7d", elapsedTime);
      }
      printf("\n");
    }
    printf("\n");
  }
}
