#pragma once

#include <memory>
#include <optional>
#include <span>

#include "audio_crate.h"

namespace audio
{
    class StaticData;

    class Output
    {
        struct Destroy
        {
            void operator()(AudioOutput* ptr) const { if (ptr) audio_output_destroy(ptr); }
        };

        std::unique_ptr<AudioOutput, Destroy> ptr;

        explicit Output(AudioOutput* ptr)
            : ptr(ptr)
        {
        }

    public:
        static std::optional<Output> create()
        {
            const auto ptr = audio_output_create();
            return ptr ? std::optional(Output(ptr)) : std::nullopt;
        }

        uint32_t sample_rate()
        {
            return audio_output_sample_rate(ptr.get());
        }

        void play(StaticData& data, float gain);
    };

    class StaticData
    {
        friend Output;

        struct Destroy
        {
            void operator()(AudioStaticData* ptr) const { if (ptr) audio_static_data_destroy(ptr); }
        };

        std::unique_ptr<AudioStaticData, Destroy> ptr;

        explicit StaticData(AudioStaticData* ptr)
            : ptr(ptr)
        {
        }

    public:
        static std::optional<StaticData> create(std::span<uint8_t> data, uint32_t output_sample_rate)
        {
            const auto ptr = audio_static_data_create(data.data(), data.size_bytes(), output_sample_rate);
            return ptr ? std::optional{StaticData(ptr)} : std::nullopt;
        }

        AudioStaticDataInfo info()
        {
            return audio_static_data_info(ptr.get());
        }
    };

    inline void Output::play(StaticData& data, float gain)
    {
        audio_output_play(ptr.get(), data.ptr.get(), gain);
    }
}
