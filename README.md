# Linux audio layer wrapper

Simple wrapper to use audio recording/playing functions from alsa/sounds library.

# PortAudio lib
To build the project you need to have portaudio lib on you computer.
```bash
sudo apt install libasound-dev portaudio19-dev
```
# Building source code

To build the project run:
```bash
cd audio
mkdir build && cd build
cmake .. && make
```

# Demonstration app

When the project have been built, you can run:
```bash
./audio -h
```
to get the demonstration app usage.

# Example
Open the ![main.cpp](cpp:src/main.cpp) file to get an example how to use the lib.
