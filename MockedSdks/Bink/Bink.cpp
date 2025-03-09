#include "Bink.h"

void BinkSoundUseDirectSound(int)
{
}

HBINK BinkOpen(const char*, int)
{
    // BinkMovie skips
    return nullptr;
}

void BinkClose(HBINK)
{
}

bool BinkWait(HBINK)
{
    return false;
}

void BinkDoFrame(HBINK)
{
}

void BinkNextFrame(HBINK)
{
}

void BinkCopyToBuffer(HBINK, unsigned char*, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int)
{
}
