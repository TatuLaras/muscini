#ifndef _PULSEAUDIO_INIT
#define _PULSEAUDIO_INIT

// Initializes pulseaudio as a audio data source for analysis.
int pulseaudio_init(int device_index);

// Cleans up device context etc.
void pulseaudio_deinit(void);

#endif
