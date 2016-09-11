#ifndef AUDIO_H
#define AUDIO_H

#include <windows.h>
#include <xaudio2.h>
#include <strsafe.h>
#include <shellapi.h>
#include <mmsystem.h>
#include <conio.h>
#include <math.h>
#include <inttypes.h>

#include <limits>

#include <bitset>

#include <vector>

#include "SDKwavefile.h"

using namespace std;

struct ThreadPassAudio
{
	IXAudio2* pXAudio2;

	CWaveFile* wav;

	BYTE* pbWaveData;

	UINT64* samplesplayed;
};

struct Beat
{
	int beat;

	int lane;

	Beat(int beatp, int lanep)
	{
		beat = beatp;

		lane = lanep;
	}
};

class XAudio2
{
public:
	XAudio2();

	~XAudio2();

	void LoadWaveX(LPCWSTR szFilename);

	void PlayWaveX();

	void AnalyzeWave();

	vector<Beat>* getBeats();

	long getLength();

	UINT64 getSamplesPlayed();

private:

	vector<Beat> beats;

	IXAudio2* pXAudio2;

	CWaveFile wav;

	BYTE* pbWaveData;

	UINT64 samplesplayed;
};

#endif