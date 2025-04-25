// Mocked out Miles Sound System SDK API

#pragma once

#include <windows.h>
#include <mmreg.h>

typedef INT32 S32;
typedef UINT32 U32;
typedef float F32;

typedef U32 AIL_ERROR;
// AIL_ERROR values checked for
enum { AIL_NO_ERROR, M3D_NOERR };
// Description of the last error
const char* AIL_last_error();

// HPROVIDER is a backend API, e.g. DirectSound, EAX. Mostly obsolete with WASAPI.
// I'd like stronger types but WWAudio defines INVALID_MILES_HANDLE as (MILES_HANDLE)-1
// and MILES_HANDLE as unsigned long.
typedef DWORD HPROVIDER;
enum HPROENUM { HPROENUM_FIRST };
int AIL_enumerate_3D_providers(HPROENUM*, HPROVIDER*, char**);
AIL_ERROR AIL_open_3D_provider(HPROVIDER);
void AIL_close_3D_provider(HPROVIDER);

// HDIGDRIVER, aka the "2D Driver", is effectively the active mixer settings, sample rate etc.
typedef struct MSS_DIGDRIVER* HDIGDRIVER;
AIL_ERROR AIL_waveOutOpen(HDIGDRIVER*, void*, int, LPWAVEFORMAT);
void AIL_waveOutClose(HDIGDRIVER);

typedef struct MSS_STREAM* HSTREAM;

typedef struct MSS_SAMPLE* HSAMPLE;
typedef struct MSS_3DSAMPLE* H3DSAMPLE;
typedef struct MSS_3DPOBJECT* H3DPOBJECT;


#define AILCALLBACK __stdcall

enum
{
    AIL_3D_2_SPEAKER,
    AIL_3D_HEADPHONE,
    AIL_3D_SURROUND,
    AIL_3D_4_SPEAKER,
};

void AIL_set_3D_speaker_type(HPROVIDER, int);

U32 AIL_sample_user_data(HSAMPLE, S32);
void AIL_set_sample_user_data(HSAMPLE, S32, U32);
void AIL_init_sample(HSAMPLE);
void AIL_set_named_sample_file(HSAMPLE, const char*, void*, U32, U32);
void AIL_start_sample(HSAMPLE);
void AIL_stop_sample(HSAMPLE);
void AIL_resume_sample(HSAMPLE);
void AIL_end_sample(HSAMPLE);
S32 AIL_sample_volume(HSAMPLE);
void AIL_set_sample_volume(HSAMPLE, S32);
S32 AIL_sample_pan(HSAMPLE);
void AIL_set_sample_pan(HSAMPLE, S32);
S32 AIL_sample_playback_rate(HSAMPLE);
void AIL_set_sample_playback_rate(HSAMPLE, S32);
U32 AIL_sample_loop_count(HSAMPLE);
void AIL_set_sample_loop_count(HSAMPLE, U32);
void AIL_sample_ms_position(HSAMPLE, S32*, S32*);
void AIL_set_sample_ms_position(HSAMPLE, U32);

enum { DP_FILTER };
void AIL_set_sample_processor(HSAMPLE, U32, HPROVIDER);
void AIL_set_filter_sample_preference(HSAMPLE, const char*, void*);

H3DPOBJECT AIL_3D_open_listener(HPROVIDER);

H3DSAMPLE AIL_allocate_3D_sample_handle(HPROVIDER);
void AIL_release_3D_sample_handle(H3DSAMPLE);
U32 AIL_3D_object_user_data(H3DSAMPLE, S32);
void AIL_set_3D_object_user_data(H3DSAMPLE, S32, U32);
AIL_ERROR AIL_set_3D_sample_file(H3DSAMPLE, void*);
void AIL_start_3D_sample(H3DSAMPLE);
void AIL_stop_3D_sample(H3DSAMPLE);
void AIL_resume_3D_sample(H3DSAMPLE);
void AIL_end_3D_sample(H3DSAMPLE);
S32 AIL_3D_sample_volume(H3DSAMPLE);
void AIL_set_3D_sample_volume(H3DSAMPLE, S32);
void AIL_set_3D_sample_distances(H3DSAMPLE, F32, F32);
void AIL_set_3D_position(H3DSAMPLE, F32, F32, F32);
void AIL_set_3D_orientation(H3DSAMPLE, F32, F32, F32, F32, F32, F32);
void AIL_set_3D_velocity_vector(H3DSAMPLE, F32, F32, F32);
S32 AIL_3D_sample_playback_rate(H3DSAMPLE);
void AIL_set_3D_sample_playback_rate(H3DSAMPLE, S32);
U32 AIL_3D_sample_loop_count(H3DSAMPLE);
void AIL_set_3D_sample_loop_count(H3DSAMPLE, U32);
U32 AIL_3D_sample_offset(H3DSAMPLE);
U32 AIL_3D_sample_length(H3DSAMPLE);
void AIL_set_3D_sample_offset(H3DSAMPLE, U32);
void AIL_set_3D_sample_effects_level(H3DSAMPLE, U32);

int AIL_enumerate_filters(HPROENUM*, HPROVIDER*, char**);

enum { AIL_FILE_SEEK_BEGIN, AIL_FILE_SEEK_CURRENT, AIL_FILE_SEEK_END };

typedef U32 AILCALLBACK AIL_file_open_callback(const char*, U32*);
typedef void AILCALLBACK AIL_file_close_callback(U32);
typedef S32 AILCALLBACK AIL_file_seek_callback(U32, S32, U32);
typedef U32 AILCALLBACK AIL_file_read_callback(U32, void*, U32);

void AIL_set_file_callbacks(
    AIL_file_open_callback,
    AIL_file_close_callback,
    AIL_file_seek_callback,
    AIL_file_read_callback);

struct MSS_DIGDRIVER
{
    BOOL emulated_ds;
};

HSTREAM AIL_open_stream(HDIGDRIVER, const char*, U32);
HSTREAM AIL_open_stream_by_sample(HDIGDRIVER, HSAMPLE, const char*, U32);
void AIL_close_stream(HSTREAM);
void AIL_start_stream(HSTREAM);
void AIL_pause_stream(HSTREAM, U32);
S32 AIL_stream_pan(HSTREAM);
void AIL_set_stream_pan(HSTREAM, S32);
S32 AIL_stream_volume(HSTREAM);
void AIL_set_stream_volume(HSTREAM, S32);
void AIL_set_stream_loop_block(HSTREAM, U32, S32);
U32 AIL_stream_loop_count(HSTREAM);
void AIL_set_stream_loop_count(HSTREAM, U32);
void AIL_stream_ms_position(HSTREAM, S32*, S32*);
void AIL_set_stream_ms_position(HSTREAM, U32);
S32 AIL_stream_playback_rate(HSTREAM);
void AIL_set_stream_playback_rate(HSTREAM, S32);
