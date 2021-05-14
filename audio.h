/*
 * @Date: 2021-05-14 09:37:38
 * @LastEditors: chenqiaoqiao
 * @LastEditTime: 2021-05-14 15:50:53
 * @FilePath: /pulseaudio/audio.h
 */

#ifndef __AUDIO_H__
#define __AUDIO_H__

void audio_init();
void audio_playback(void *audio, int size);
void audio_stop();

#endif