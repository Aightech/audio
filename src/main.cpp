#include <alsa/asoundlib.h>
#include <iostream>
#include <lsl_cpp.h>
#include <stdio.h>
#include <stdlib.h>

snd_pcm_t *
init(snd_pcm_stream_t mode)
{
    int err;
    snd_pcm_t *playback_handle;
    snd_pcm_hw_params_t *hw_params;

    if((err = snd_pcm_open(&playback_handle, "default", mode, 0)) < 0)
    {
        fprintf(stderr, "cannot open audio device %s (%s)\n", "default",
                snd_strerror(err));
        exit(1);
    }

    if((err = snd_pcm_hw_params_malloc(&hw_params)) < 0)
    {
        fprintf(stderr, "cannot allocate hardware parameter structure (%s)\n",
                snd_strerror(err));
        exit(1);
    }

    if((err = snd_pcm_hw_params_any(playback_handle, hw_params)) < 0)
    {
        fprintf(stderr, "cannot initialize hardware parameter structure (%s)\n",
                snd_strerror(err));
        exit(1);
    }

    if((err = snd_pcm_hw_params_set_access(playback_handle, hw_params,
                                           SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
    {
        fprintf(stderr, "cannot set access type (%s)\n", snd_strerror(err));
        exit(1);
    }

    if((err = snd_pcm_hw_params_set_format(playback_handle, hw_params,
                                           SND_PCM_FORMAT_S16_LE)) < 0)
    {
        fprintf(stderr, "cannot set sample format (%s)\n", snd_strerror(err));
        exit(1);
    }
    unsigned int actualRate = 44100;
    if((err = snd_pcm_hw_params_set_rate_near(playback_handle, hw_params,
                                              &actualRate, 0)) < 0)
    {
        fprintf(stderr, "cannot set sample rate (%s)\n", snd_strerror(err));
        exit(1);
    }

    if((err = snd_pcm_hw_params_set_channels(playback_handle, hw_params, 2)) <
       0)
    {
        fprintf(stderr, "cannot set channel count (%s)\n", snd_strerror(err));
        exit(1);
    }

    if((err = snd_pcm_hw_params(playback_handle, hw_params)) < 0)
    {
        fprintf(stderr, "cannot set parameters (%s)\n", snd_strerror(err));
        exit(1);
    }

    snd_pcm_hw_params_free(hw_params);

    if((err = snd_pcm_prepare(playback_handle)) < 0)
    {
        fprintf(stderr, "cannot prepare audio interface for use (%s)\n",
                snd_strerror(err));
        exit(1);
    }
    return playback_handle;
};

int
main(int argc, char *argv[])
{
    int i;
    int err;
    short buf[128];
    snd_pcm_t *playback_handle = init(SND_PCM_STREAM_PLAYBACK);
    snd_pcm_t *capture_handle = init(SND_PCM_STREAM_CAPTURE);

    try
    {

        int nb_ch = 1;
        lsl::stream_info info_sample("audio", "sample", nb_ch, 0, lsl::cf_int8);
        lsl::stream_outlet outlet_sample(info_sample);
        std::vector<char> sample(1);
        std::cout << "[INFOS] Now sending data... " << std::endl;

        for(i = 0; i < 5000; ++i)
        {
            if((err = snd_pcm_readi(capture_handle, buf, 128)) != 128)
            {
                fprintf(stderr, "read from audio interface failed (%s)\n",
                        snd_strerror(err));
                exit(1);
            }
            for(int i = 0; i < 128; i++)
	      {    
	       sample[0] =buf[i];
	       outlet_sample.push_sample(sample);
	      }
            
            // if((err = snd_pcm_writei(playback_handle, buf, 128)) != 128)
            // {
            //     fprintf(stderr, "write to audio interface failed (%s)\n",
            //             snd_strerror(err));
            //     exit(1);
            // }
        }
    }
    catch(std::exception &e)
    {
        std::cerr << "[ERROR] Got an exception: " << e.what() << std::endl;
    }

    snd_pcm_close(playback_handle);
    return 0;
}
