#pragma once

#include "fftw3.h"
#include <complex>
#include <vector>

class fft
{
private:
    int N;
    fftwf_plan forward_plan;
    fftwf_plan backward_plan;
    fftwf_complex* in;
    fftwf_complex* out;
    void buildPlans();

public:
    fft(int N);
    ~fft();
    void executeForward(std::vector<std::complex<float>> &data);
    void executeBackward(std::vector<std::complex<float>> &data);
};
