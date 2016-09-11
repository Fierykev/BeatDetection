#ifndef FFT_H
#define FFT_H

#include <complex>
#include <iostream>
#include <valarray>

typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

void fft(CArray& x);

#endif