// Mocked Bink SDK
#pragma once

struct BINK
{
    unsigned int Width;
    unsigned int Height;
    unsigned int Frames;
    unsigned int FrameNum;
    unsigned int FrameRate;
    unsigned int FrameRateDiv;
};

typedef BINK* HBINK;

enum
{
    BINKCOPYNOSCALING = 1 << 0,
    BINKSURFACE565 = 1 << 1,
};

void BinkSoundUseDirectSound(int);
HBINK BinkOpen(const char*, int);
void BinkClose(HBINK);
bool BinkWait(HBINK);
void BinkDoFrame(HBINK);
void BinkNextFrame(HBINK);
void BinkCopyToBuffer(HBINK, unsigned char*, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
