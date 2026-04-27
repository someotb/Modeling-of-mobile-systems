#include "fft.hpp"

fft::fft(int N) : N(N)
{
    in = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * N);
    out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * N);
    buildPlans();
}

void fft::buildPlans()
{
    forward_plan = fftwf_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    backward_plan = fftwf_plan_dft_1d(N, in, out, FFTW_BACKWARD, FFTW_ESTIMATE);
}

void fft::executeForward(std::vector<std::complex<float>> &data)
{
    for (size_t i = 0; i < N; ++i)
    {
        in[i][0] = data[i].real();
        in[i][1] = data[i].imag();
    }
    fftwf_execute(forward_plan);
    for (size_t i = 0; i < N; ++i)
        data[i] = std::complex<float>(out[i][0], out[i][1]);
}

void fft::executeBackward(std::vector<std::complex<float>> &data)
{
    for (size_t i = 0; i < N; ++i)
    {
        in[i][0] = data[i].real();
        in[i][1] = data[i].imag();
    }
    fftwf_execute(backward_plan);
    for (size_t i = 0; i < N; ++i)
        data[i] = std::complex<float>(out[i][0] / N, out[i][1] / N);
}

fft::~fft()
{
    fftwf_destroy_plan(forward_plan);
    fftwf_destroy_plan(backward_plan);
    fftwf_free(in);
    fftwf_free(out);
}