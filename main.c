/*
 * @Date: 2021-05-14 09:36:31
 * @LastEditors: chenqiaoqiao
 * @LastEditTime: 2021-05-14 16:09:26
 * @FilePath: /pulseaudio/main.c
 */

#include <stdlib.h>
#include <stdio.h>
#include "audio.h"

typedef unsigned char uint8_t;
extern int ready;

int main()
{
    FILE *faudio = NULL;
    uint8_t *pcm = NULL;

    pcm = g_malloc0(256 * 4);
    
    audio_init();

    faudio = fopen("/home/troila/Downloads/test.pcm", "r");
    if(!faudio) {
        printf("pcm文件打开失败\n");
    }

    while(!feof(faudio)) {
        if(ready == 0)
            continue;
        fread(pcm, 256 * 4, 1, faudio);
        audio_playback(pcm, 256 * 4);
        usleep(10000);
    }

    fclose(faudio);
    faudio = NULL;

    audio_stop();
    
    return 1;
}