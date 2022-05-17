#include <alsa/asoundlib.h>
#include <iostream>
#include <lsl_cpp.h>
#include <stdio.h>
#include <stdlib.h>

snd_pcm_t *
init(snd_pcm_stream_t mode,  unsigned int actualRate)
{
    int err;
    snd_pcm_t *playback_handle;
    snd_pcm_hw_params_t *hw_params;
   

    if((err = snd_pcm_open(&playback_handle, "default", mode, 0)) < 0)
        throw std::string("cannot open audio device ") + snd_strerror(err);

    if((err = snd_pcm_hw_params_malloc(&hw_params)) < 0)
        throw std::string("cannot allocate hw param. ") + snd_strerror(err);

    if((err = snd_pcm_hw_params_any(playback_handle, hw_params)) < 0)
        throw std::string("cannot initialize hw param. ") + snd_strerror(err);

    if((err = snd_pcm_hw_params_set_access(playback_handle, hw_params,
                                           SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
        throw std::string("cannot set access type ") + snd_strerror(err);

    if((err = snd_pcm_hw_params_set_format(playback_handle, hw_params,
                                           SND_PCM_FORMAT_S16_LE)) < 0)
        throw std::string("cannot set sample format ") + snd_strerror(err);

    if((err = snd_pcm_hw_params_set_rate_near(playback_handle, hw_params,
                                              &actualRate, 0)) < 0)
        throw std::string("cannot set sample rate ") + snd_strerror(err);

    if((err = snd_pcm_hw_params_set_channels(playback_handle, hw_params, 2)) <
       0)
        throw std::string("cannot set channel count ") + snd_strerror(err);

    if((err = snd_pcm_hw_params(playback_handle, hw_params)) < 0)
        throw std::string("cannot set parameters ") + snd_strerror(err);

    snd_pcm_hw_params_free(hw_params);

    if((err = snd_pcm_prepare(playback_handle)) < 0)
        throw std::string("cannot prepare interface ") + snd_strerror(err);

    return playback_handle;
};

int
main(int argc, char *argv[])
{
    int err;
    short buf[256];
    unsigned int actualRate = 44100;
    unsigned int actualPeriod = 1000000/actualRate;
    snd_pcm_t *capture_handle = init(SND_PCM_STREAM_CAPTURE, actualRate);
    

    try
    {

        int nb_ch = 1;
	int s_chunk = 64;
        lsl::stream_info info_sample("audio", "sample", nb_ch, 0, lsl::cf_int16);
        lsl::stream_outlet outlet_sample(info_sample);
        std::cout << "[INFOS] Now sending data... " << std::endl;

        for(int t=0;;t++)
        {
            if((err = snd_pcm_readi(capture_handle, buf, s_chunk)) != s_chunk)
                throw std::string("read from audio  failed ") +
                    snd_strerror(err);
	    

            for(int j = 0; j < s_chunk; j++)
	      {
                outlet_sample.push_numeric_raw((&buf[2*j]));
		//usleep(actualPeriod);
	      }
	    //std::cout << t << std::endl;

        }
    }
    catch(std::exception &e)
    {
        std::cerr << "[ERROR] Got an exception: " << e.what() << std::endl;
    }

    snd_pcm_close(capture_handle);
    return 0;
}
