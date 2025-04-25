# WWAudio

The Westwood audio API.

Mostly gutted: used Miles Sound System and to access various hardware audio backends like EAX, DirectSound etc., which
don't really exist any more, and therefore most of the code is no longer relevant.

Backend is a new crate in `Crates/audio` that uses `cpal` for cross-platform output and `symphonia` for decoding.
I investigated several existing audio crates, but I couldn't find any with multi-channel output, mono sound data, etc.
so they wouldn't support the 3D audio we will eventually want. For now I have a *very* simple mixer, but there are
some existing crates I can look at later to implement proper positional audio and effects.

# API

(Notes as I go through the code.)

`WWAudioClass` is the main singleton entry point, which owns:
- an `audio::Output` handle which wraps the `cpal` output stream
  and a simple mixer.
- a `SoundSceneClass` that handles positional sources and listeners.
- a "playlist" of currently playing `AudibleSoundClass` objects.
- ...

`SoundBufferClass` (and `StreamSoundBufferClass` subtype) is the cached, decoded sound data for a file.

`AudibleSoundClass` represents a logical sound source (2d or 3d), containing:
- bookkeeping information like type (effect, music, dialog, cinematic), priority used to globally manage sounds.
- the play state, volume, pitch, looping, pan or 3d position, etc.
- the `SoundBufferClass` with the data
- if playing, the `SoundHandleClass` which represents the playing sound handle in the audio system.

`SoundHandleClass` uses it's various `Sound*HandleClass` subtypes to wrap the various Miles handle types
for streaming, 2d etc. sounds and provide a common interface for `Play_Sample()`, `Set_Sample_Pan()` etc., it's
probably not useful any more as the `audio` crate can return a generic handle that does the equivalent directly.

### Walkthrough

To play sound or music, you might use `WWAudioClass::Get_Instance()->Create_Sound(filename)` to get the
`AudibleSoundClass`, which will:
- use `DefinitionMgrClass` to find the definition for the sound data as an `AudibleSoundDefinitionClass`
- use `Create_Sound()` on that to create and initialize the `AudibleSoundClass` from the definition
- including using either `WWAudioClass::Create_3D_Sound()` or `Create_Sound_Effect()` to create the actual
  `AudibleSoundClass`
- that will get a `SoundBufferClass` from the cache or create a new one using `Create_Sound_Buffer()`
- this will create either a `SoundBufferClass` or `StreamSoundBufferClass` depending on the data size, then
- load the raw data using `SoundBufferClass::Load_From_File()`, reading the data from the `File` into a buffer
- then using `SoundBufferClass::Init()` to decode that loaded data with `audio::StaticData::create()`, which will:
  - use `symphonia` to decode the file, then
  - resample to the output sample rate, then
  - store the decoded samples as planar (vec per channel) f32 buffers
- then the `AudibleSoundClassDefinition::Create_Sound()` finishes initializing the `AudibleSoundClass`

Then you would use `AudibleSoundClass::Play()`, which will:
- first add the `AudibleSoundClass` to the `WWAudioClass` "playlist"
- do basic bookeeping, play state, start time and looping.
- use `SoundHandleClass::Start_Sample()` to tell the backend to play
- seek to the current position (if there is a start offset)
- notify any `AudioCallbackClass` the sound is started
- create if needed the `LogicalSoundClass` used by gameplay
- fire if needed a "text notification", used by `AudioTextCallback` to display debug text
 
While playing, `AudibleSoundClass::On_Frame_Update()` will continuously trigger, most importantly including
`Update_Play_Position()` which will look at the current wall-clock time to calculate when the sound has finished (or
looped, etc.)
