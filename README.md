<!--
 * @Date: 2021-05-14 17:14:42
 * @LastEditors: chenqiaoqiao
 * @LastEditTime: 2021-06-22 17:06:40
 * @FilePath: /spice-gtk/home/cqq/test/pulseaudio/README.md
-->
# pulseaudio
application which playback audio with pulse audio

./autoreconf -ivf
./configure
make

#usage
thread main loop ---->thread-mainloop.c (reference pulseaudio/src/thread-mainloop.h)
glib main loop ------>main.c (API run in one thread with mainloop, otherwise cause dump. 
                              reference pulseaudio/src/glib-mainloop.h)
#pulseaudio is the official lib