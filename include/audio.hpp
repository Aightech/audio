#ifndef __AUDIO_HPP
#define __AUDIO_HPP

//#include "pa_asio.h"
#include "portaudio.h"
#include <strANSIseq.hpp>


class AudioStream: virtual public ESC::CLI
{

    public:
    AudioStream(int verbose = 0): ESC::CLI(verbose, "AudioStream")
    {
        m_err = Pa_Initialize();
        check_error();
    }

    ~AudioStream()
    {
        Pa_Terminate();
    }

    void
    config(int nb_input_channels,
           int nb_output_channels,
           int sample_rate,
           int frames_per_buffer,
           PaSampleFormat format,
           int (*callback)(const void *,
                           void *,
                           unsigned long,
                           const PaStreamCallbackTimeInfo *,
                           PaStreamCallbackFlags,
                           void *),
           void *userData)
    {
        m_callback = callback;
        m_userData = userData;
        m_err = Pa_OpenDefaultStream(&m_stream, nb_input_channels,
                                     nb_output_channels, format, sample_rate,
                                     frames_per_buffer, callback, userData);
        check_error();
    }

    void
    start()
    {
        m_err = Pa_StartStream(m_stream);
        check_error();
    }

    void stop()
    {
        m_err = Pa_StopStream(m_stream);
        check_error();
        m_err = Pa_CloseStream(m_stream);
        check_error();
    }

    void
    check_error()
    {
        if(m_err != paNoError)
        {
            Pa_Terminate();
            logln(log_error("An error occurred while using the portaudio stream"));
            logln(log_error("Error number: " + std::to_string(m_err)));
            logln(log_error("Error message: " + std::string(Pa_GetErrorText(m_err))));
        }
    }

    void
    set_callback(int (*callback)(const void *inputBuffer,
                                 void *outputBuffer,
                                 unsigned long framesPerBuffer,
                                 const PaStreamCallbackTimeInfo *timeInfo,
                                 PaStreamCallbackFlags statusFlags,
                                 void *userData),
                 void *userData)
    {
        m_callback = callback;
        m_userData = userData;
    }

    private:
    PaStream *m_stream;
    PaError m_err;
    //callback function
    int (*m_callback)(const void *inputBuffer,
                      void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo *timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void *userData) = nullptr;
    void *m_userData = nullptr;
};

#endif