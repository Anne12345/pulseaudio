AUTOMAKE_OPTIONS=foreign
NULL =
LDADD =
MAINTAINERCLEANFILES =
bin_PROGRAMS = pulseaudio
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = .

pulseaudio_SOURCES = \
	main.c \
	$(NULL)
pulseaudio_LDFLAGS = \
	$(NULL)
pulseaudio_CFLAGS = \
    $(PULSE_CFLAGS)	\
	$(NULL)
pulseaudio_LDADD = \
	$(PULSE_LIBS)    \
	$(NULL)