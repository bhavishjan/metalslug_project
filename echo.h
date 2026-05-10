#pragma once

#include <fstream>
#include <iostream> 
#include <cmath>
#include <SFML/Audio.hpp>
using namespace std;
using namespace sf;
#pragma pack(push, 1)
struct WavHeader {
	char* chunkId;
	int chunkSize;
	char* format;
	char* subchunk1Id;
	int subchunk1Size;
	short audioFormat;
	short numChannels;
	int sampleRate;
	int byteRate;
	short blockAlign;
	short bitsPerSample;
	char* subchunk2Id;
	int subchunk2Size;
};
#pragma pack(pop)
class whisper;
class Complex {
public:
	double real;
	double imaginary;

	Complex(double r = 0, double i = 0);
	Complex operator+(const Complex& other);
	Complex operator-(const Complex& other);
	Complex operator*(const Complex& other);
};
class Filter {
public:
    float* weights;
    int size;

    Filter(int sz);
    ~Filter();
    void lowPass(int cutoff, int samplerate);
    void highPass(int cutoff, int samplerate);
    void bandPass(int lower, int upper, int samplerate);
};



class Spectrum {
public:
    Complex* frequencybin;
    int size;
    int originalsize;
    int samplerate;

    Spectrum(int sz, int origSz, int sr);
    ~Spectrum();
    void applyFilter(Filter& f);
    whisper recouple();
};


class whisper {
public:
    WavHeader header;
    short* audio;
    int countofaudio;

    whisper();
    ~whisper();
    void loadingaudio(const char* filenam);
    void peaknormalize();
    void operator*=(float gain);
    void reverse();
    whisper hardSplice(int startsample, int endsample);
    void toSFML(SoundBuffer& buffer);
    whisper operator+(const whisper& other);
    void operator+=(const whisper& other);
    int nextPowerOf2(int n);
    void simpleFFT(Complex* data, int n);
    void inverseFFT(Complex* data, int n);
    Spectrum decouple();
    void saveAudio(const char* filename);
    whisper getMuffled();
};
