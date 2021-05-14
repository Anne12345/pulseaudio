/*
 * @Date: 2021-05-14 09:37:45
 * @LastEditors: chenqiaoqiao
 * @LastEditTime: 2021-05-14 17:10:04
 * @FilePath: /pulseaudio/audio.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <pulse/pulseaudio.h>
#include <pulse/glib-mainloop.h>

pa_stream *stream;
pa_sample_spec spec;

pa_context *context;
pa_mainloop_api *mainloop;

typedef unsigned char uint8_t;

int ready = 0;

static void stream_state_callback(pa_stream *s, void *userdata)
{
    printf("stream state:%d\n", pa_stream_get_state(s));
    switch (pa_stream_get_state(s)) {
        case PA_STREAM_CREATING:
        case PA_STREAM_TERMINATED:
            break;
        case PA_STREAM_READY: {
            ready = 1;
            break;
        }
        case PA_STREAM_FAILED:
        default:
            printf("Stream error: %s", pa_strerror(pa_context_errno(pa_stream_get_context(s))));
    }
}

static void create_stream()
{
    pa_stream_flags_t flags;
    pa_cvolume vol;
    spec.format   = PA_SAMPLE_S16LE;
    spec.rate     = 44100;
    spec.channels = 1;
    
    pa_cvolume_set(&vol, spec.channels, PA_VOLUME_NORM);

    stream = pa_stream_new(context, "playback", &spec, NULL);
    flags = PA_STREAM_ADJUST_LATENCY | PA_STREAM_AUTO_TIMING_UPDATE;
    pa_stream_set_state_callback(stream, stream_state_callback, NULL);
    if (pa_stream_connect_playback(stream,
                                NULL, NULL, flags, &vol, NULL) < 0) {
        printf("pa_stream_connect_playback() failed: %s",
                pa_strerror(pa_context_errno(context)));
    }
}

static void context_state_callback(pa_context *c, void *userdata)
{
   printf("state:%d\n", pa_context_get_state(c));
   switch (pa_context_get_state(c)) {
        case PA_CONTEXT_CONNECTING:
        case PA_CONTEXT_AUTHORIZING:
        case PA_CONTEXT_SETTING_NAME:
        case PA_CONTEXT_UNCONNECTED:
            break;
        case PA_CONTEXT_READY: {
            //stream
            if(stream == NULL) {
                create_stream();
            }
            break;
        }
        default:
            break;
   }
}

void audio_init()
{
    //context init
    // mainloop = pa_glib_mainloop_new(NULL);
    mainloop = pa_threaded_mainloop_new();
    context = pa_context_new(
              pa_threaded_mainloop_get_api(mainloop), "pulseaudio"); //pa_glib_mainloop_get_api
    pa_context_set_state_callback(context, context_state_callback, NULL);
    if (pa_context_connect(context, NULL, 0, NULL) < 0) {
        printf("pa_context_connect() failed: %s",
            pa_strerror(pa_context_errno(context)));
    }
    // pa_mainloop_run(mainloop, NULL);
    int ret = pa_threaded_mainloop_start(mainloop);
}

void audio_playback(void *audio, int size)
{
    pa_stream_state_t state;
    state = pa_stream_get_state(stream);
    switch (state) {
        case PA_STREAM_CREATING:
            printf("stream creating, dropping data\n");
            break;
        case PA_STREAM_READY:
            if (pa_stream_write(stream, audio, size, NULL, 0, PA_SEEK_RELATIVE) < 0) {
            printf("pa_stream_write() failed: %s",
                      pa_strerror(pa_context_errno(context)));
            }   
            break;
        default:
            break;
    }
}

void audio_stop()
{
    if (stream) {
        pa_stream_disconnect(stream);
        pa_stream_unref(stream);
    }

    if (context) {
        pa_context_disconnect(context);
        pa_context_unref(context);
    }

    if (mainloop != NULL) {
        pa_threaded_mainloop_stop(mainloop);
        pa_threaded_mainloop_free(mainloop);
    }
}