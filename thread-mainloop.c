/*
 * @Date: 2021-06-17 09:41:53
 * @LastEditors: chenqiaoqiao
 * @LastEditTime: 2021-06-21 17:44:37
 * @FilePath: /spice-gtk/home/cqq/test/pulseaudio/thread-mainloop.c
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <pulse/simple.h>
#include <pulse/error.h>

#include <pthread.h>

#define BUFSIZE 1024

int main(int argc, char*argv[]) {

    /* The Sample format to use */
    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 2
    };

    pa_simple *s = NULL;
    int ret = 1;
    int error;
    
    /* replace STDIN with the specified file if needed */
    // if (argc > 1) {
    //     int fd;

    //     if ((fd = open(argv[1], O_RDONLY)) < 0) {
    //         fprintf(stderr, __FILE__": open() failed: %s\n", strerror(errno));
    //         goto finish;
    //     }

    //     if (dup2(fd, STDIN_FILENO) < 0) {
    //         fprintf(stderr, __FILE__": dup2() failed: %s\n", strerror(errno));
    //         goto finish;
    //     }

    //     close(fd);
    // }
    FILE *fp = NULL;
    fp = fopen("/home/test.pcm", "r");
    if(!fp) {
        printf("pcm文件打开失败\n");
    }
    
    /* Create a new playback stream */
    if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        goto finish;
    }

    for (;;) {
        uint8_t buf[BUFSIZE];
        ssize_t r;

#if 0
        pa_usec_t latency;

        if ((latency = pa_simple_get_latency(s, &error)) == (pa_usec_t) -1) {
            fprintf(stderr, __FILE__": pa_simple_get_latency() failed: %s\n", pa_strerror(error));
            goto finish;
        }

        fprintf(stderr, "%0.0f usec    \r", (float)latency);
#endif
        /*data restart*/
        if(feof(fp))
            fseek(fp, 0, SEEK_SET);
        /* Read some data ... */
        if ((r = fread(buf, sizeof(buf), 1, fp)) <= 0) {
            // if (r == 0) /* EOF */
            //     fseek(fp, 0, SEEK_SET);

            fprintf(stderr, __FILE__": read() failed: %s\n", strerror(errno));
            // goto finish;
        }

        /* ... and play it */
        if (pa_simple_write(s, buf, (size_t) 1024, &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
            goto finish;
        }
    }

    /* Make sure that every single sample was played */
    if (pa_simple_drain(s, &error) < 0) {
        fprintf(stderr, __FILE__": pa_simple_drain() failed: %s\n", pa_strerror(error));
        goto finish;
    }

    ret = 0;

finish:

    if (s)
        pa_simple_free(s);

    return ret;
}