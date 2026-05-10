#include "echo.h"

// Complex implementations
Complex::Complex(double r, double i) {
    real = r;
    imaginary = i;
}

Complex Complex::operator+(const Complex& other) {
    return Complex(real + other.real,
        imaginary + other.imaginary);
}

Complex Complex::operator-(const Complex& other) {
    return Complex(real - other.real,
        imaginary - other.imaginary);
}

Complex Complex::operator*(const Complex& other) {
    double r = (real * other.real) - (imaginary * other.imaginary);
    double g = (real * other.imaginary) + (imaginary * other.real);
    return Complex(r, g);
}

// Filter implementations
Filter::Filter(int sz) {
    size = sz;
    weights = new float[size];
    for (int i = 0; i < size; i++)
        weights[i] = 0.0f;
}

Filter::~Filter() {
    delete[] weights;
}

void Filter::lowPass(int cutoff, int samplerate) {
    int bin = cutoff * size / samplerate;
    for (int i = 0; i < size; i++) {
        if (i <= bin || i >= size - bin) {
            weights[i] = 1.0f;
        }
        else {
            weights[i] = 0.0f;
        }
    }
}

void Filter::highPass(int cutoff, int samplerate) {
    int bin = cutoff * size / samplerate;
    for (int i = 0; i < size; i++) {
        if (i > bin && i < size - bin) {
            weights[i] = 1.0f;
        }
        else {
            weights[i] = 0.0f;
        }
    }
}

void Filter::bandPass(int lower, int upper, int samplerate) {
    int lowerbin = lower * size / samplerate;
    int upperbin = upper * size / samplerate;
    for (int i = 0; i < size; i++) {
        if ((i >= lowerbin && i <= upperbin) || (i >= size - upperbin && i <= size - lowerbin)) {
            weights[i] = 1.0f;
        }
        else {
            weights[i] = 0.0f;
        }
    }
}

// Spectrum implementations
Spectrum::Spectrum(int sz, int origSz, int sr) {
    size = sz;
    originalsize = origSz;
    samplerate = sr;
    frequencybin = new Complex[size];
}

Spectrum::~Spectrum() {
    delete[] frequencybin;
}

void Spectrum::applyFilter(Filter& f) {
    for (int i = 0; i < size; i++) {
        frequencybin[i].real *= f.weights[i];
        frequencybin[i].imaginary *= f.weights[i];
    }
}

whisper Spectrum::recouple() {
    Complex* temp = new Complex[size];
    for (int i = 0; i < size; i++)
        temp[i] = frequencybin[i];

    int reversed = 0;
    for (int i = 1; i < size; i++) {
        int half = size / 2;
        while (reversed >= half) {
            reversed = reversed - half;
            half = half / 2;
        }
        reversed = reversed + half;
        if (i < reversed) {
            Complex temp2 = temp[i];
            temp[i] = temp[reversed];
            temp[reversed] = temp2;
        }
    }

    for (int block = 2; block <= size; block *= 2) {
        double angle = 2 * 3.14159265 / block;
        Complex rotation(cos(angle), sin(angle));
        for (int start = 0; start < size; start += block) {
            Complex factor(1, 0);
            for (int j = 0; j < block / 2; j++) {
                Complex upper = temp[start + j];
                Complex lower = factor * temp[start + j + block / 2];
                temp[start + j] = upper + lower;
                temp[start + j + block / 2] = upper - lower;
                factor = factor * rotation;
            }
        }
    }

    for (int i = 0; i < size; i++) {
        temp[i].real = temp[i].real / size;
        temp[i].imaginary = temp[i].imaginary / size;
    }

    whisper result;
    result.countofaudio = originalsize;
    result.audio = new short[originalsize];
    for (int i = 0; i < originalsize; i++) {
        double val = temp[i].real;
        if (val > 32767) {
            val = 32767;
        }
        if (val < -32768) {
            val = -32768;
        }
        result.audio[i] = (short)val;
    }
    delete[] temp;
    return result;
}

// whisper implementations
whisper::whisper() {
    audio = nullptr;
    countofaudio = 0;
    header.chunkId = new char[4];
    header.format = new char[4];
    header.subchunk1Id = new char[4];
    header.subchunk2Id = new char[4];
    memset(&header, 0, sizeof(WavHeader));
}

whisper::~whisper() {
    delete[] audio;
    delete[] header.chunkId;
    delete[] header.format;
    delete[] header.subchunk1Id;
    delete[] header.subchunk2Id;
}

void whisper::loadingaudio(const char* filenam) {
    ifstream file(filenam, ios::binary);
    if (!file.is_open()) {
        return;
    }

    file.read((char*)&header, sizeof(WavHeader));

    int bytecount = header.subchunk2Size;
    char* rawbytes = new char[bytecount];
    file.read(rawbytes, bytecount);
    file.close();

    if (header.bitsPerSample == 16) {
        countofaudio = bytecount / 2;
        audio = new short[countofaudio];
        for (int i = 0; i < countofaudio; i++) {
            unsigned char byte1 = rawbytes[i * 2];
            unsigned char byte2 = rawbytes[i * 2 + 1];
            audio[i] = (short)(byte1 | (byte2 << 8));
        }
    }
    else if (header.bitsPerSample == 8) {
        countofaudio = bytecount;
        audio = new short[countofaudio];
        for (int i = 0; i < countofaudio; i++) {
            audio[i] = (short)((unsigned char)rawbytes[i] - 128) * 256;
        }
        header.bitsPerSample = 16;
        header.byteRate = header.sampleRate * header.numChannels * 2;
        header.blockAlign = header.numChannels * 2;
    }
    delete[] rawbytes;
}

void whisper::peaknormalize() {
    short max = 0;
    for (int i = 0; i < countofaudio; i++) {
        short current = audio[i];

        if (current < 0) {
            current = -current;
        }

        if (current > max) {
            max = current;
        }
    }

    double factorscale = 32767.0 / max;

    for (int i = 0; i < countofaudio; i++) {
        double scaled = audio[i] * factorscale;

        if (scaled > 32767) {
            scaled = 32767;
        }
        if (scaled < -32768) {
            scaled = -32768;
        }

        audio[i] = (short)scaled;
    }
}

void whisper::operator*=(float gain) {
    for (int i = 0; i < countofaudio; i++) {
        double scaled = audio[i] * gain;

        if (scaled > 32767) {
            scaled = 32767;
        }
        if (scaled < -32768) {
            scaled = -32768;
        }

        audio[i] = (short)scaled;
    }
}

void whisper::reverse() {
    int start = 0;
    int end = countofaudio - 1;

    while (start < end) {
        short temp = audio[start];
        audio[start] = audio[end];
        audio[end] = temp;
        start++;
        end--;
    }
}

whisper whisper::hardSplice(int startsample, int endsample) {
    whisper result;
    result.countofaudio = endsample - startsample;
    result.audio = new short[result.countofaudio];

    for (int i = 0; i < result.countofaudio; i++) {
        result.audio[i] = audio[startsample + i];
    }

    result.header = header;
    return result;
}

void whisper::toSFML(SoundBuffer& buffer) {
    buffer.loadFromSamples(audio, countofaudio, header.numChannels, header.sampleRate);
}

whisper whisper::operator+(const whisper& other) {
    whisper result;

    if (countofaudio < other.countofaudio) {
        result.countofaudio = countofaudio;
    }
    else {
        result.countofaudio = other.countofaudio;
    }

    result.audio = new short[result.countofaudio];

    for (int i = 0; i < result.countofaudio; i++) {
        double mixed = audio[i] + other.audio[i];

        if (mixed > 32767) {
            mixed = 32767;
        }
        if (mixed < -32768) {
            mixed = -32768;
        }

        result.audio[i] = (short)mixed;
    }

    result.header = header;
    return result;
}

void whisper::operator+=(const whisper& other) {
    int newcount = countofaudio + other.countofaudio;
    short* newarray = new short[newcount];

    for (int i = 0; i < countofaudio; i++) {
        newarray[i] = audio[i];
    }

    for (int i = 0; i < other.countofaudio; i++) {
        if (countofaudio + i < newcount) {
            newarray[countofaudio + i] = other.audio[i];
        }
    }

    delete[] audio;
    audio = newarray;
    countofaudio = newcount;
    header.subchunk2Size = countofaudio * 2;
}

int whisper::nextPowerOf2(int n) {
    int power = 1;
    while (power < n) {
        power = power * 2;
    }
    return power;
}

void whisper::simpleFFT(Complex* data, int n) {
    int reversed = 0;

    for (int i = 1; i < n; i++) {
        int half = n / 2;

        while (reversed >= half) {
            reversed = reversed - half;
            half = half / 2;
        }

        reversed = reversed + half;

        if (i < reversed) {
            Complex temp = data[i];
            data[i] = data[reversed];
            data[reversed] = temp;
        }
    }

    for (int block = 2; block <= n; block *= 2) {
        double angle = -2 * 3.14159265 / block;
        Complex rotation(cos(angle), sin(angle));

        for (int start = 0; start < n; start += block) {
            Complex factor(1, 0);

            for (int j = 0; j < block / 2; j++) {
                Complex upper = data[start + j];
                Complex lower = factor * data[start + j + block / 2];

                data[start + j] = upper + lower;
                data[start + j + block / 2] = upper - lower;

                factor = factor * rotation;
            }
        }
    }
}

void whisper::inverseFFT(Complex* data, int n) {
    int reversed = 0;

    for (int i = 1; i < n; i++) {
        int half = n / 2;

        while (reversed >= half) {
            reversed = reversed - half;
            half = half / 2;
        }

        reversed = reversed + half;

        if (i < reversed) {
            Complex temp = data[i];
            data[i] = data[reversed];
            data[reversed] = temp;
        }
    }

    for (int block = 2; block <= n; block *= 2) {
        double angle = 2 * 3.14159265 / block;
        Complex rotation(cos(angle), sin(angle));

        for (int start = 0; start < n; start += block) {
            Complex factor(1, 0);

            for (int j = 0; j < block / 2; j++) {
                Complex upper = data[start + j];
                Complex lower = factor * data[start + j + block / 2];

                data[start + j] = upper + lower;
                data[start + j + block / 2] = upper - lower;

                factor = factor * rotation;
            }
        }
    }

    for (int i = 0; i < n; i++) {
        data[i].real = data[i].real / n;
        data[i].imaginary = data[i].imaginary / n;
    }
}

Spectrum whisper::decouple() {
    int padded = nextPowerOf2(countofaudio);
    Spectrum spec(padded, countofaudio, header.sampleRate);

    for (int i = 0; i < padded; i++) {
        if (i < countofaudio) {
            spec.frequencybin[i] = Complex(audio[i], 0);
        }
        else {
            spec.frequencybin[i] = Complex(0, 0);
        }
    }

    simpleFFT(spec.frequencybin, padded);
    return spec;
}

void whisper::saveAudio(const char* filename) {
    header.subchunk2Size = countofaudio * 2;
    header.chunkSize = 36 + header.subchunk2Size;

    ofstream file(filename, ios::binary);
    file.write((char*)&header, sizeof(WavHeader));
    file.write((char*)audio, header.subchunk2Size);
    file.close();
}

whisper whisper::getMuffled() {
    Spectrum spec = decouple();
    Filter f(spec.size);
    f.lowPass(600, spec.samplerate);
    spec.applyFilter(f);
    whisper muffled = spec.recouple();
    muffled.header = header;
    return muffled;
}
