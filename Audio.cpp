#define _USE_MATH_DEFINES

#include <ctime>

#include "Track.h"

#include "Audio.h"

#include "FFT.h"

using namespace std;

#define SONGSCALE 1.0

#define ZOOM 100.0

#define SMOOTH 10

#define NUMCHECKS 2

#define LINEARINCA 0

#define LINEARINCB 32

#define CENERGY2 15

#define CVARIENCE 0

#define beatmin 6

#define beatmax 14

#define MINES 1000

#define BEATSAMPLE 1024.0

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

HRESULT LoadWave(IXAudio2*& pXaudio2, LPCWSTR szFilename, CWaveFile& wav, BYTE*& pbWaveData);
HRESULT InitXAudio2(IXAudio2*& pXAudio2);

/**
Constructor
*/

XAudio2::XAudio2()
{
	InitXAudio2(pXAudio2); // create XAudio2
}

/**
Destructor
*/

XAudio2::~XAudio2()
{
	// delete XAudio2

	SAFE_RELEASE(pXAudio2);
	CoUninitialize();
}

/**
Create XAudio2 loader
*/

HRESULT InitXAudio2(IXAudio2*& pXAudio2)
{
	HRESULT hr;

	// init XAudio2

	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	UINT32 flags = 0;

#ifdef _DEBUG
	flags |= XAUDIO2_DEBUG_ENGINE;
#endif

	// could not create the audio engine

	if (FAILED(hr = XAudio2Create(&pXAudio2, flags)))
	{
		wprintf(L"Failed to init XAudio2 engine: %#X\n", hr);
		CoUninitialize();
		return 0;
	}

	// create a mastering voice

	IXAudio2MasteringVoice* pMasteringVoice = NULL;

	if (FAILED(hr = pXAudio2->CreateMasteringVoice(&pMasteringVoice)))
	{
		wprintf(L"Failed creating mastering voice: %#X\n", hr);
		SAFE_RELEASE(pXAudio2);
		CoUninitialize();
		return 0;
	}

	return S_OK;
}

/**
Load Wave file for XAudio2
*/

void XAudio2::LoadWaveX(LPCWSTR szFilename)
{
	// load a wave

	LoadWave(pXAudio2, szFilename, wav, pbWaveData);

	beats.clear();

	AnalyzeWave();
}

/**
Play the Wave file
*/

HRESULT PlayWave(IXAudio2* pXaudio2, CWaveFile& wav, BYTE* pbWaveData, UINT64* samplesplayed)
{
	HRESULT hr = S_OK;

	// Get format of wave file
	WAVEFORMATEX* pwfx = wav.GetFormat();

	// Calculate how many bytes and samples are in the wave
	DWORD cbWaveSize = wav.GetSize();

	//
	// Play the wave using a XAudio2SourceVoice
	//

	// Create the source voice
	IXAudio2SourceVoice* pSourceVoice;
	if (FAILED(hr = pXaudio2->CreateSourceVoice(&pSourceVoice, pwfx)))
	{
		wprintf(L"Error %#X creating source voice\n", hr);
		SAFE_DELETE_ARRAY(pbWaveData);
		return hr;
	}

	// Submit the wave sample data using an XAUDIO2_BUFFER structure
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.pAudioData = pbWaveData;
	buffer.Flags = XAUDIO2_END_OF_STREAM;  // tell the source voice not to expect any data after this buffer
	buffer.AudioBytes = cbWaveSize;

	if (FAILED(hr = pSourceVoice->SubmitSourceBuffer(&buffer)))
	{
		wprintf(L"Error %#X submitting source buffer\n", hr);
		pSourceVoice->DestroyVoice();
		SAFE_DELETE_ARRAY(pbWaveData);
		return hr;
	}

	hr = pSourceVoice->Start(0);

	clock_t timeplayed = clock(); // the amount of time the song has played for

	// Let the sound play
	BOOL isRunning = TRUE;
	while (SUCCEEDED(hr) && isRunning)
	{
		XAUDIO2_VOICE_STATE state;
		pSourceVoice->GetState(&state);
		isRunning = (state.BuffersQueued > 0) != 0;

		// update the game on the position of the soundtrack
		
		*samplesplayed = (std::clock() - timeplayed) / (double)CLOCKS_PER_SEC * wav.GetFormat()->nSamplesPerSec; // state.SamplesPlayed;
	}

	// remove the audio

	pSourceVoice->DestroyVoice();
	SAFE_DELETE_ARRAY(pbWaveData);

	return hr;
}

/**
Load the Wave file into memory
*/

HRESULT LoadWave(IXAudio2*& pXaudio2, LPCWSTR szFilename, CWaveFile& wav, BYTE*& pbWaveData)
{
	HRESULT hr = S_OK;

	// locate the wave file

	WCHAR strFilePath[MAX_PATH];

	wcscpy_s(strFilePath, MAX_PATH, szFilename);

	// TODO: ADD A FILE CHECKER
	/*if (FAILED(hr = FindMediaFileCch(strFilePath, MAX_PATH, szFilename)))
	{
	wprintf(L"Failed to find media file: %s\n", szFilename);
	return hr;
	}*/

	// read the wave file

	if (FAILED(hr = wav.Open(strFilePath, NULL, WAVEFILE_READ)))
	{
		wprintf(L"Failed reading WAV file: %#X (%s)\n", hr, strFilePath);
		return hr;
	}

	// Get format of wave file
	WAVEFORMATEX* pwfx = wav.GetFormat();

	// Calculate how many bytes and samples are in the wave
	DWORD cbWaveSize = wav.GetSize();
	
	// Read the sample data into memory
	pbWaveData = new BYTE[cbWaveSize];

	if (FAILED(hr = wav.Read(pbWaveData, cbWaveSize, &cbWaveSize)))
	{
		wprintf(L"Failed to read WAV data: %#X\n", hr);
		SAFE_DELETE_ARRAY(pbWaveData);
		return hr;
	}

	return hr;
}

/**
Analyze the Wave file for beats
*/

void XAudio2::AnalyzeWave()
{
	// Based on the http://archive.gamedev.net/archive/reference/programming/features/beatdetection/ article.

	// ONLY FOR: 16 bit mono wave files right now

	// convert the bytes to doubles for the FFT algorithm

	const int samplerate = 32767;

	const double bytesize = wav.GetSize(); // length of the byte array from the wave file

	const int datasize = ceil((bytesize / 2.0) / 1024.0) * 1024.0 * SONGSCALE; // the size of the data after converted from bytes
	
	Complex* datac = new Complex[1024];//datasize];

	Complex* datac2 = new Complex[datasize];

	//bool* energypeak = new bool[(int)bytesize / size + 21];
	
	// load the data to be processed
	
	for (int l = 0; l < datasize / 1024; l++)
	{
		for (unsigned int i = 0; i < 1024; i++)
		{
			if ((l * 1024 + i) * 2 < bytesize)
				datac[i].real((__int16)((__int16)((pbWaveData[(l * 1024 + i) * 2 + 1] << 8) | pbWaveData[(l * 1024 + i) * 2]) / SMOOTH));
			else
				datac[i].real(0);

			datac[i].imag(0);
		}

		CArray data2(datac, 1024);

		fft(data2);

		for (int i = 0; i < 1024; i++)
			datac2[l * 1024 + i] = data2[i];
	}

	double* B = new double[datasize]; // create double to store the amps

	for (unsigned long i = 0; i < datasize; i++)
	{
		B[i] = sqrt((datac2[i].real() * datac2[i].real()) + (datac2[i].imag() * datac2[i].imag())); // mod(x + iy) = sqrt(x^2 + y^2)
	}

	// calculate the number of subbands

	double sum = 1024;

	int SUBBANDS;

	for (SUBBANDS = 1; 0 < sum; SUBBANDS++)
		sum -= (int)(LINEARINCA * SUBBANDS + LINEARINCB);
	
	SUBBANDS -= 1;
	
	double subband = SUBBANDS;

	double* Es = new double[datasize / 1024 * SUBBANDS];
	
	int wi; // width of subband

	// analize for 1024 samples
	
	for (int l = 0; l < datasize / 1024; l++)
	{
		sum = 0;

		for (int i = 0; i < SUBBANDS; i++) // divide into subbands
		{
			wi = i < SUBBANDS - 1 ? LINEARINCA * (i + 1) + LINEARINCB : 1024 - sum; // width of the subband

			Es[l * SUBBANDS + i] = 0; // clear the value

			for (int k = sum; k < sum + wi; k++)
			{
				Es[l * SUBBANDS + i] += B[l * 1024 + k];
			}

			sum += wi; // add the width to the sum

			Es[l * SUBBANDS + i] *= wi / 1024.0;
		}
	}

	// average subband energy

	double subbanddatasize = datasize / 1024 * SUBBANDS;

	double* Ei = new double[datasize / 1024 * SUBBANDS];

	double* V = new double[datasize / 1024 * SUBBANDS];

	sum = 0;

	for (int i = 0; i < 43; i++)
		sum += Es[i];

	Ei[0] = sum / 43.0;

	double maxEi = Ei[0]; // find the highest Ei value for lanes

	double minEi = Ei[0]; // find the lowest Ei value for lanes

	for (int i = 1; i < datasize / 1024 * SUBBANDS - 43; i++)
	{
		// energy calc

		sum += Es[i + 42] - Es[i - 1];

		Ei[i] = sum / 43.0;

		if (maxEi < Ei[i])
			maxEi = Ei[i];

		if (Ei[i] < minEi)
			minEi = Ei[i];
	}

	double lanesize = (maxEi - minEi) / 4.0;

	int lanenum;

	double max = 0;
	
	for (int l = 42; l + BEATSAMPLE < datasize / 1024 * SUBBANDS; l += BEATSAMPLE)
	{
		max = 0;
		
		for (int j = 0; j < BEATSAMPLE; j += NUMCHECKS)
		{
			if (max < Es[l + j] / Ei[l + j - 42])
				max = Es[l + j] / Ei[l + j - 42];
		}

		// round to .5

		max = /*7.f;//*/ floor(max);//(int)(max * 10.0) % 10 < 5 ? floor(max) : floor(max) + .5;
		
		// constrain the bands

		if (max < beatmin)
			max = beatmin;
		else if (beatmax < max)
			max = beatmax;

		// TMP

		int lastlanebeat[NUMLANES]; // the last beat in a lane

		for (register int i = 0; i < NUMLANES; i++)
			lastlanebeat[i] = 0;

		for (int j = 0; j < BEATSAMPLE; j += 1)
		{
			if (Ei[l + j - 42] * max < Es[l + j])
			{
				const int position = (l + j) / SUBBANDS;

				lanenum = round(Ei[l + j - 42] / (lanesize + minEi) + 1);

				if (NUMLANES < lanenum)
					lanenum = NUMLANES;
				else if (lanenum < 1)
					lanenum = 1;
			
				// TMP

				//if (position - lastlanebeat[lanenum - 1] > 10)
				{
					beats.push_back(Beat((l + j) / SUBBANDS, lanenum));
				}

				lastlanebeat[lanenum - 1] = position;
			}
		}
	}

	// clear heap

	free(Ei);

	free(V);

	free(Es);

	free(datac);

	free(datac2);

	free(B);
}

/**
Play the audio in a different thread
*/

DWORD WINAPI playWaveThread(LPVOID pContext)
{
	ThreadPassAudio* tp = (ThreadPassAudio*)pContext;

	PlayWave(tp->pXAudio2, *tp->wav, tp->pbWaveData, tp->samplesplayed);

	delete(tp->samplesplayed);

	delete(tp);

	return 0;
}

/**
Launch the thread to play the wave file
*/

void XAudio2::PlayWaveX()
{
	DWORD dwThreadId = 0;

	ThreadPassAudio* tp = new ThreadPassAudio();

	tp->pbWaveData = pbWaveData;

	tp->pXAudio2 = pXAudio2;

	tp->wav = &wav;

	tp->samplesplayed = &samplesplayed;

	HANDLE hSecondThread = CreateThread(NULL, 0, playWaveThread, (void*)tp, 0, &dwThreadId);
}

// accessor

/**
Get the length of the audio
*/

long XAudio2::getLength()
{
	return wav.GetSize();
}

/**
Get the beats in the song
*/

vector<Beat>* XAudio2::getBeats()
{
	return &beats;
}

/**
Get the number of samples that have been played
*/

UINT64 XAudio2::getSamplesPlayed()
{
	return samplesplayed;
}