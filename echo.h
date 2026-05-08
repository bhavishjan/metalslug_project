
#pragma once
#include <fstream>
#include < iostream > 
#include <cmath>
using namespace std;
#pragma pack(push, 1)
struct WavHeader {//for audio info
	char chunkId[4]; // "RIFF"
	int chunkSize; // Total file size - 8
	char format[4]; // "WAVE"
	char subchunk1Id[4]; // "fmt "
	int subchunk1Size; // 16 for PCM
	short audioFormat; // 1 for PCM
	short numChannels; // 1 for Mono, 2 for Stereo
	int sampleRate; // e.g. 44100
	int byteRate; // sampleRate * numChannels * 2
	short blockAlign; // numChannels * 2
	short bitsPerSample; // 16
	char subchunk2Id[4]; // "data"
	int subchunk2Size; // Size of audio data in bytes
};
#pragma pack(pop)
class whisper;
class Complex {
public:
	double real;
	double imaginary;  // imaginary number

	// constructor
	Complex(double r = 0, double i = 0) {
		real = r;
		imaginary = i;
	}



	// addition 
	Complex operator+(const Complex& other) {
		return Complex(real + other.real,
			imaginary + other.imaginary);
	}

	// subt
	Complex operator-(const Complex& other) {
		return Complex(real - other.real,
			imaginary - other.imaginary);
	}

	//multiplicat 
	Complex operator*(const Complex& other) {
		double r = (real * other.real) - (imaginary * other.imaginary);//a+bi*c+di;
		// real=ac-bd
		 //imag=ad+bc
		double g = (real * other.imaginary) + (imaginary * other.real);
		return Complex(r, g);
	}
};
class Filter {
public:
	float* weights;
	int size;

	// constructor
	Filter(int sz) {
		size = sz;
		weights = new float[size];
		// sab zero se shuru
		for (int i = 0; i < size; i++)
			weights[i] = 0.0f;
	}

	// destructor
	~Filter() {
		delete[] weights;
	}

	// low pass cutoff se neeche rakhna
	void lowPass(int cutoff, int samplerate) {
		int bin = cutoff * size / samplerate;
		for (int i = 0; i < size; i++) {
			if (i <= bin || i >= size - bin)
				weights[i] = 1.0f;  // kkep
			else
				weights[i] = 0.0f;  // remove
		}
	}

	// high pass keep above cutoff 
	void highPass(int cutoff, int samplerate) {
		int bin = cutoff * size / samplerate;
		for (int i = 0; i < size; i++) {
			if (i > bin && i < size - bin)
				weights[i] = 1.0f;  // keep
			else
				weights[i] = 0.0f;  // rermove
		}
	}

	// band pass lower to upper 
	void bandPass(int lower, int upper, int samplerate) {
		int lowerbin = lower * size / samplerate;
		int upperbin = upper * size / samplerate;
		for (int i = 0; i < size; i++) {
			if ((i >= lowerbin && i <= upperbin) || (i >= size - upperbin && i <= size - lowerbin))
				weights[i] = 1.0f;  // keep
			else
				weights[i] = 0.0f;  // remove
		}
	}
};



class Spectrum {
public:
	Complex* frequencybin;  // frequency array
	int size;                // padded size
	int originalsize;        // original size
	int samplerate;          // same as audio

	// constructor
	Spectrum(int sz, int origSz, int sr) {
		size = sz;
		originalsize = origSz;
		samplerate = sr;
		frequencybin = new Complex[size];
		// hint kaunsa type ka array chahiye?
	}

	// destructor
	~Spectrum() {
		delete[] frequencybin;
	}


	void applyFilter(Filter& f) {
		for (int i = 0; i < size; i++) {
			frequencybin[i].real *= f.weights[i];
			frequencybin[i].imaginary *= f.weights[i];
		}
	}
	whisper recouple();
};


class whisper {//file info
public:
	WavHeader header;
	short* audio;//for 2 bytes
	int countofaudio;

	whisper()
	{
		audio = nullptr;
		countofaudio = 0;
	}
	~whisper() {
		delete[]audio;
	}
	void loadingaudio(const char* filenam) {//Load the first 44 raw bytes of the file into this struct.
		ifstream file(filenam, ios::binary);//file opening
		if (!file.is_open()) {//if file not opening 
			cout << "Error: Could not open file!" << endl;
			return;
		}
		cout << "File opened successfully!" << endl;

		file.read((char*)&header, sizeof(WavHeader));

		// Print to check it worked
		cout << "sample rate: " << header.sampleRate << endl;
		cout << "channels: " << header.numChannels << endl;
		cout << "bits/sample: " << header.bitsPerSample << endl;
		cout << "audio size: " << header.subchunk2Size << " bytes" << endl;



		// read raw audio byte
		int bytecount = header.subchunk2Size;//how many bytes of audio exist
		char* rawbytes = new char[bytecount];//new memory to holde raw byes
		file.read(rawbytes, bytecount);//load all bytes into rawbyte
		file.close();
		cout << "audio bytes read: " << bytecount << endl;


		//converting raw bytes int num
		if (header.bitsPerSample == 16) {//16 means 2 bytes
			countofaudio = bytecount / 2;
			audio = new short[countofaudio];//memry to hold sample
			//combine every 2 byte into one short
			for (int i = 0; i < countofaudio; i++) {
				unsigned char byte1 = rawbytes[i * 2];
				unsigned char byte2 = rawbytes[i * 2 + 1];
				audio[i] = (short)(byte1 | (byte2 << 8));//move 2ndbite to left side
			}

			cout << "16-bit audio loaded!" << endl;
			cout << "Total samples: " << countofaudio << endl;
		}
		else if (header.bitsPerSample == 8) {//8 means 1 byte
			countofaudio = bytecount;
			audio = new short[countofaudio];
			for (int i = 0; i < countofaudio; i++) {
				unsigned char sample = rawbytes[i];
				audio[i] = ((short)sample - 128) * 256;//-128 shift range from 0 to 255 to -128 to 127 multiphy256 krke -32768 to 32767
			}
			// update header to say we are now 16-bit
			header.bitsPerSample = 16;
			header.byteRate = header.sampleRate * header.numChannels * 2;
			header.blockAlign = header.numChannels * 2;

			cout << "8-bit audio converted to 16-bit!" << endl;
			cout << "Total samples: " << countofaudio << endl;
		}
		delete[] rawbytes;
		cout << "Loading complete!" << endl;
	}
	//peak normalizatio
	void peaknormalize() {
		//find mac value
		short max = 0;
		for (int i = 0; i < countofaudio; i++) {
			short current = audio[i];

			if (current < 0)
				current = -current;
			// negative numbers ko positive karo

			if (current > max)
				max = current;
		}

		double factorscale = 32767.0 / max;



		for (int i = 0; i < countofaudio; i++) {
			double scaled = audio[i] * factorscale;

			// hard clipping
			if (scaled > 32767)  scaled = 32767;
			if (scaled < -32768) scaled = -32768;//dont go outside range

			audio[i] = (short)scaled;
		}
	}
	//gain
	void operator*=(float gain) {

		for (int i = 0; i < countofaudio; i++) {
			double scaled = audio[i] * gain;

			// hard clipping
			if (scaled > 32767)  scaled = 32767;//dont go outside range
			if (scaled < -32768) scaled = -32768;


			audio[i] = (short)scaled;
		}
	}//reversal
	void reverse() {
		int start = 0;
		int end = countofaudio - 1;

		while (start < end) {
			// swap 
			short temp = audio[start];
			audio[start] = audio[end];
			audio[end] = temp;

			// move next
			start++;
			end--;
		}
	}//extract some object and copy in new obj
	whisper hardSplice(int startsample, int endsample) {

		// new obj
		whisper result;


		result.countofaudio = endsample - startsample;


		// memory allocate
		result.audio = new short[result.countofaudio];

		// copy karo samples
		for (int i = 0; i < result.countofaudio; i++) {
			result.audio[i] = audio[startsample + i];

		}

		// header copy 
		result.header = header;

		return result;
	}

	// load into SFML buffer
	void toSFML(sf::SoundBuffer& buffer) {
		buffer.loadFromSamples(
			audio,
			countofaudio,
			header.numChannels,
			header.sampleRate
		);
	}





	//Track mixing
	whisper operator+(const whisper& other) {

		whisper result;

		// take smaller audio so program cannot crash
		if (countofaudio < other.countofaudio)
			result.countofaudio = countofaudio;
		else
			result.countofaudio = other.countofaudio;

		result.audio = new short[result.countofaudio];//new memory

		for (int i = 0; i < result.countofaudio; i++) {
			double mixed = audio[i] + other.audio[i];

			// hard clipping
			if (mixed > 32767)  mixed = 32767;
			if (mixed < -32768) mixed = -32768;

			result.audio[i] = (short)mixed;
		}

		result.header = header;
		return result;
	}
	//concatenate merge 2 audios 
	void operator+=(const whisper& other) {
		int newcount = countofaudio + other.countofaudio;

		short* newarray = new short[newcount];

		//copy first
		for (int i = 0; i < countofaudio; i++) {
			newarray[i] = audio[i];
		}

		// copy 2nd
		for (int i = 0; i < other.countofaudio; i++) {
			newarray[countofaudio + i] = other.audio[i];

		}

		delete[] audio;
		audio = newarray;
		countofaudio = newcount;

		// header update
		header.subchunk2Size = countofaudio * 2;
	}


	int nextPowerOf2(int n)
	{
		int power = 1;
		while (power < n)
		{
			power = power * 2;
		}
		return power;
	}
	void simpleFFT(Complex* data, int n)
	{
		int reversed = 0;

		// Bit reordering
		for (int i = 1; i < n; i++)
		{
			int half = n / 2;//starting from highest bit pos

			while (reversed >= half)
			{
				reversed = reversed - half;
				half = half / 2;
			}

			reversed = reversed + half;

			if (i < reversed)//only swap when i is less so that double swapping na hojye
			{
				Complex temp = data[i];
				data[i] = data[reversed];
				data[reversed] = temp;
			}
		}

		// FFT stages
		for (int block = 2; block <= n; block *= 2)
		{
			double angle = -2 * 3.14159265 / block;//rotation angle

			Complex rotation(cos(angle), sin(angle));

			for (int start = 0; start < n; start += block)
			{
				Complex factor(1, 0);

				for (int j = 0; j < block / 2; j++)
				{
					Complex upper = data[start + j];//read upper elemment
					Complex lower = factor * data[start + j + block / 2];//multi lower by factor

					data[start + j] = upper + lower;
					data[start + j + block / 2] = upper - lower;

					factor = factor * rotation;
				}
			}
		}
	}
	void inverseFFT(Complex* data, int n)
	{
		int reversed = 0;

		// Bit reversal (same as FFT)
		for (int i = 1; i < n; i++)
		{
			int half = n / 2;

			while (reversed >= half)
			{
				reversed = reversed - half;
				half = half / 2;
			}

			reversed = reversed + half;

			if (i < reversed)
			{
				Complex temp = data[i];
				data[i] = data[reversed];
				data[reversed] = temp;
			}
		}

		// FFT stages positive angle
		for (int block = 2; block <= n; block *= 2)
		{
			double angle = 2 * 3.14159265 / block;

			Complex rotation(cos(angle), sin(angle));

			for (int start = 0; start < n; start += block)
			{
				Complex factor(1, 0);

				for (int j = 0; j < block / 2; j++)
				{
					Complex upper = data[start + j];
					Complex lower = factor * data[start + j + block / 2];

					data[start + j] = upper + lower;
					data[start + j + block / 2] = upper - lower;

					factor = factor * rotation;
				}
			}
		}

		// Divide by n
		for (int i = 0; i < n; i++)
		{
			data[i].real = data[i].real / n;
			data[i].imaginary = data[i].imaginary / n;
		}
	}
	Spectrum decouple()
	{
		int padded = nextPowerOf2(countofaudio);

		Spectrum spec(padded, countofaudio, header.sampleRate);

		// copy samples into complex numbers
		for (int i = 0; i < padded; i++)
		{
			if (i < countofaudio)
				spec.frequencybin[i] = Complex(audio[i], 0);
			else
				spec.frequencybin[i] = Complex(0, 0); // zero padding
		}

		simpleFFT(spec.frequencybin, padded);

		return spec;
	}
	void saveAudio(const char* filename) {
		header.subchunk2Size = countofaudio * 2;
		header.chunkSize = 36 + header.subchunk2Size;

		ofstream file(filename, ios::binary);
		file.write((char*)&header, sizeof(WavHeader));
		file.write((char*)audio, header.subchunk2Size);
		file.close();
		cout << "Audio saved: " << filename << endl;
	}
	/*When the player is submerged into the water in aquatic biomes you must use muffled sounds
	for sound effects and background music.The catch is that you can’t use prepared muffled
		sounds.You must use Q4 of assignment 2 and decompose the sounds into discrete frequencies
		and delete the frequencies at the higher end of the spectrum.This decomposition must
		happen at runtime.*/
	whisper getMuffled() {
		Spectrum spec = decouple();
		Filter f(spec.size);
		f.lowPass(600, spec.samplerate);
		spec.applyFilter(f);
		whisper muffled = spec.recouple();
		muffled.header = header;
		return muffled;
	}
};
whisper Spectrum::recouple() {

	Complex* temp = new Complex[size];
	for (int i = 0; i < size; i++)
		temp[i] = frequencybin[i];

	// bit reversal
	int reversed = 0;
	for (int i = 1; i < size; i++) {
		int half = size / 2;
		while (reversed >= half) {
			reversed = reversed - half;
			half = half / 2;
		}
		reversed = reversed + half;
		if (i < reversed) {//swap
			Complex temp2 = temp[i];
			temp[i] = temp[reversed];
			temp[reversed] = temp2;
		}
	}

	// positive angle = inverse
	for (int block = 2; block <= size; block *= 2) {
		double angle = 2 * 3.14159265 / block;
		Complex rotation(cos(angle), sin(angle));
		for (int start = 0; start < size; start += block) {
			Complex factor(1, 0);
			for (int j = 0; j < block / 2; j++) {
				Complex upper = temp[start + j];
				Complex lower = factor * temp[start + j + block / 2];
				temp[start + j] = upper + lower;
				temp[start + j + block / 2] = upper - lower;//store diff
				factor = factor * rotation;
			}
		}
	}

	// divide by size
	for (int i = 0; i < size; i++) {
		temp[i].real = temp[i].real / size;
		temp[i].imaginary = temp[i].imaginary / size;
	}

	// store in whisper
	whisper result;
	result.countofaudio = originalsize;
	result.audio = new short[originalsize];
	for (int i = 0; i < originalsize; i++) {
		double val = temp[i].real;
		if (val > 32767)  val = 32767;
		if (val < -32768) val = -32768;
		result.audio[i] = (short)val;
	}
	delete[] temp;
	return result;
}




