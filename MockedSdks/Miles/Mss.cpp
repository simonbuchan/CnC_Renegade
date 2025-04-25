#include "Mss.h"

const char* AIL_last_error()
{
    return "Unimplemented";
}

AIL_ERROR AIL_waveOutOpen(HDIGDRIVER* result, void* , int, LPWAVEFORMAT)
{
    return AIL_NO_ERROR;
}

void AIL_waveOutClose(HDIGDRIVER)
{
}

int AIL_enumerate_3D_providers(HPROENUM*, HPROVIDER*, char**)
{
    return 0;
}

AIL_ERROR AIL_open_3D_provider(HPROVIDER)
{
    return AIL_NO_ERROR;
}

void AIL_close_3D_provider(HPROVIDER)
{
}

void AIL_set_3D_speaker_type(HPROVIDER, int)
{
}

U32 AIL_sample_user_data(HSAMPLE, S32)
{
    return 0;
}

void AIL_set_sample_user_data(HSAMPLE, S32, U32)
{
}

void AIL_init_sample(HSAMPLE)
{
}

void AIL_set_named_sample_file(HSAMPLE, const char*, void*, U32, U32)
{
}

void AIL_start_sample(HSAMPLE)
{
}

void AIL_stop_sample(HSAMPLE)
{
}

void AIL_resume_sample(HSAMPLE)
{
}

void AIL_end_sample(HSAMPLE)
{
}

S32 AIL_sample_volume(HSAMPLE)
{
    return 0;
}

void AIL_set_sample_volume(HSAMPLE, S32)
{
}

S32 AIL_sample_pan(HSAMPLE)
{
    return 0;
}

void AIL_set_sample_pan(HSAMPLE, S32)
{
}

S32 AIL_sample_playback_rate(HSAMPLE)
{
    return 0;
}

void AIL_set_sample_playback_rate(HSAMPLE, S32)
{
}

U32 AIL_sample_loop_count(HSAMPLE)
{
    return 0;
}

void AIL_set_sample_loop_count(HSAMPLE, U32)
{
}

void AIL_sample_ms_position(HSAMPLE, S32*, S32*)
{
}

void AIL_set_sample_ms_position(HSAMPLE, U32)
{
}

void AIL_set_sample_processor(HSAMPLE, U32, HPROVIDER)
{
}

void AIL_set_filter_sample_preference(HSAMPLE, const char*, void*)
{
}

H3DPOBJECT AIL_3D_open_listener(HPROVIDER)
{
    return NULL;
}

H3DSAMPLE AIL_allocate_3D_sample_handle(HPROVIDER)
{
    return NULL;
}

void AIL_release_3D_sample_handle(H3DSAMPLE)
{
}

U32 AIL_3D_object_user_data(H3DSAMPLE, S32)
{
    return 0;
}

void AIL_set_3D_object_user_data(H3DSAMPLE, S32, U32)
{
}

AIL_ERROR AIL_set_3D_sample_file(H3DSAMPLE, void*)
{
    return AIL_NO_ERROR;
}

void AIL_start_3D_sample(H3DSAMPLE)
{
}

void AIL_stop_3D_sample(H3DSAMPLE)
{
}

void AIL_resume_3D_sample(H3DSAMPLE)
{
}

void AIL_end_3D_sample(H3DSAMPLE)
{
}

S32 AIL_3D_sample_volume(H3DSAMPLE)
{
    return 0;
}

void AIL_set_3D_sample_volume(H3DSAMPLE, S32)
{
}

void AIL_set_3D_sample_distances(H3DSAMPLE, F32, F32)
{
}

void AIL_set_3D_position(H3DSAMPLE, F32, F32, F32)
{
}

void AIL_set_3D_orientation(H3DSAMPLE, F32, F32, F32, F32, F32, F32)
{
}

void AIL_set_3D_velocity_vector(H3DSAMPLE, F32, F32, F32)
{
}

S32 AIL_3D_sample_playback_rate(H3DSAMPLE)
{
    return 0;
}

void AIL_set_3D_sample_playback_rate(H3DSAMPLE, S32)
{
}

U32 AIL_3D_sample_loop_count(H3DSAMPLE)
{
    return 0;
}

void AIL_set_3D_sample_loop_count(H3DSAMPLE, U32)
{
}

U32 AIL_3D_sample_offset(H3DSAMPLE)
{
    return 0;
}

U32 AIL_3D_sample_length(H3DSAMPLE)
{
    return 0;
}

void AIL_set_3D_sample_offset(H3DSAMPLE, U32)
{
}

void AIL_set_3D_sample_effects_level(H3DSAMPLE, U32)
{
}

int AIL_enumerate_filters(HPROENUM*, HPROVIDER*, char**)
{
    return 0;
}

void AIL_set_file_callbacks(AIL_file_open_callback, AIL_file_close_callback, AIL_file_seek_callback,
                            AIL_file_read_callback)
{
}

HSTREAM AIL_open_stream(HDIGDRIVER, const char*, U32)
{
    return NULL;
}

HSTREAM AIL_open_stream_by_sample(HDIGDRIVER, HSAMPLE, const char*, U32)
{
    return NULL;
}

void AIL_close_stream(HSTREAM)
{
}

void AIL_start_stream(HSTREAM)
{
}

void AIL_pause_stream(HSTREAM, U32)
{
}

S32 AIL_stream_pan(HSTREAM)
{
    return 0;
}

void AIL_set_stream_pan(HSTREAM, S32)
{
}

S32 AIL_stream_volume(HSTREAM)
{
    return 0;
}

void AIL_set_stream_volume(HSTREAM, S32)
{
}

void AIL_set_stream_loop_block(HSTREAM, U32, S32)
{
}

U32 AIL_stream_loop_count(HSTREAM)
{
    return 0;
}

void AIL_set_stream_loop_count(HSTREAM, U32)
{
}

void AIL_stream_ms_position(HSTREAM, S32*, S32*)
{
}

void AIL_set_stream_ms_position(HSTREAM, U32)
{
}

S32 AIL_stream_playback_rate(HSTREAM)
{
    return 0;
}

void AIL_set_stream_playback_rate(HSTREAM, S32)
{
}
