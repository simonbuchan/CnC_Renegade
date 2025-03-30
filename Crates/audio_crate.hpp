#pragma once

#include <memory>

#include "audio_crate.h"

namespace audio
{

struct Manager
{
    std::unique_ptr<AudioManager, decltype(&audio_manager_destroy)> manager;

    explicit Manager(AudioManager* mgr)
        : manager(mgr, audio_manager_destroy)
    {
    }
};



}
