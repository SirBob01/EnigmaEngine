#include <sndfile.hh>

#include <Asset/Sound.hpp>
#include <Sound/DSP/Resample.hpp>
#include <Utils/Log.hpp>

namespace Dynamo::Asset {
    Sound::Buffer load_sound(const std::string filepath) {
        SndfileHandle file(filepath.c_str(), SFM_READ, 0, 1, 0);
        if (file.error()) {
            Log::error("Could not load sound file `{}`: {}", filepath, file.strError());
        }

        unsigned frames = file.frames();
        unsigned channels = file.channels();
        unsigned sample_rate = file.samplerate();

        // Read raw PCM samples
        std::vector<Sound::WaveSample> interleaved(frames * channels);
        file.readf(interleaved.data(), interleaved.size());

        // Deinterleave the data
        Sound::Buffer raw(frames, channels);
        for (unsigned f = 0; f < frames; f++) {
            for (unsigned c = 0; c < channels; c++) {
                raw[c][f] = interleaved[f * channels + c];
            }
        }

        // Resample the signal
        double scale = Sound::STANDARD_SAMPLE_RATE / sample_rate;
        Sound::Buffer resampled(frames * scale, channels);
        for (unsigned c = 0; c < channels; c++) {
            Sound::resample_signal(raw[c], resampled[c], 0, frames, sample_rate, Sound::STANDARD_SAMPLE_RATE);
        }

        return resampled;
    }
} // namespace Dynamo::Asset