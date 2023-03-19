#include <audio.hpp>
#include <lsl_c.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


double timestamp[1024];
float data[10240];

int n=0;

/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int
paCallback(const void *inputBuffer,
           void *outputBuffer,
           unsigned long framesPerBuffer,
           const PaStreamCallbackTimeInfo *timeInfo,
           PaStreamCallbackFlags statusFlags,
           void *userData)
{
    lsl_inlet* inlet = (lsl_inlet*)userData;
    float *out = (float *)outputBuffer;
    //pull chunk of data from lsl stream directly into output buffer
    int err;
    lsl_pull_chunk_f(*inlet, out, NULL, framesPerBuffer, 0.08192, 0.0, &err);
    (void)timeInfo;
    (void)statusFlags;
    return 0;
}

int
main(int argc, char *argv[])
{
    //check if Kistler lsl stream is available
    lsl_streaminfo info;
    std::cout << "Searching for stream" << std::endl;
    if(lsl_resolve_byprop(&info, 1, "type", "measurement", 0, 2)<1)
        std::cout << "error" << std::endl;
    lsl_inlet inlet = lsl_create_inlet(info, 1, 1024, 1);

    //create audio stream
    AudioStream audio;
    audio.config(0, 1, 12500, 1024, paFloat32, paCallback, &inlet);
    audio.start();


    //wait for user input
    std::cout << "Press enter to stop the audio stream" << std::endl;
    std::cin.get();

    //stop audio stream
    audio.stop();

    return 0;
}
