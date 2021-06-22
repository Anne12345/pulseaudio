/*
 * @Date: 2021-06-17 13:38:15
 * @LastEditors: chenqiaoqiao
 * @LastEditTime: 2021-06-22 17:01:26
 * @FilePath: /spice-gtk/home/cqq/test/pulseaudio/main.c
 */
#pragma GCC diagnostic ignored "-Wstrict-prototypes"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <glib.h>

#include <pulse/context.h>
#include <pulse/pulseaudio.h>
#include <pulse/glib-mainloop.h>

pa_context *ctxt;
pa_glib_mainloop *m;

pa_stream *stream;
pa_sample_spec spec;

typedef unsigned char uint8_t;
int ready = 0;

static void context_state_callback(pa_context *c, void *userdata);

static void connect(void) {
    int r;

    ctxt = pa_context_new(pa_glib_mainloop_get_api(m), NULL);
    g_assert(ctxt);

    r = pa_context_connect(ctxt, NULL, PA_CONTEXT_NOAUTOSPAWN|PA_CONTEXT_NOFAIL, NULL);
    g_assert(r == 0);

    pa_context_set_state_callback(ctxt, context_state_callback, NULL);
}

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

    stream = pa_stream_new(ctxt, "playback", &spec, NULL);
    flags = PA_STREAM_ADJUST_LATENCY | PA_STREAM_AUTO_TIMING_UPDATE;
    pa_stream_set_state_callback(stream, stream_state_callback, NULL);
    if (pa_stream_connect_playback(stream,
                                NULL, NULL, flags, &vol, NULL) < 0) {
        printf("pa_stream_connect_playback() failed: %s",
                pa_strerror(pa_context_errno(ctxt)));
    }
}

static void context_state_callback(pa_context *c, void *userdata) {
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
                      pa_strerror(pa_context_errno(ctxt)));
            }   
            break;
        default:
            break;
    }
}

FILE *faudio = NULL;
uint8_t *pcm = NULL;

gboolean thread_run(gpointer data)
{

    if(feof(faudio)) {
        fseek(faudio, 0, SEEK_SET);
    }
    if(ready == 1) {
        fread(pcm, 1024, 1, faudio);
        audio_playback(pcm, 1024);
    }
    return TRUE;
}

int main(int argc, char *argv[]) {

    GtkWidget *window;
    GThread *thread;

    pcm = calloc(1024, sizeof(uint8_t));

    faudio = fopen("/home/test.pcm", "r");
    if(!faudio) {
        printf("pcm文件打开失败\n");
    }
    
    gtk_init(&argc, &argv);

    g_set_application_name("This is a test");
    gtk_window_set_default_icon_name("foobar");
    g_setenv("PULSE_PROP_media.role", "phone", TRUE);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW (window), g_get_application_name());
    gtk_widget_show_all(window);
    
    m = pa_glib_mainloop_new(NULL);
    g_assert(m);

    connect();

    g_timeout_add(10, thread_run, NULL);
    gtk_main();

    pa_context_unref(ctxt);
    pa_glib_mainloop_free(m);

    return 0;
}