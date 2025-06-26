#ifndef _AUDIO_DEVICE
#define _AUDIO_DEVICE
/*
Enumeration and selection of audio devices.
*/

#include "miniaudio.h"

// Enumerate and print available input audio devices, and collect user input to
// choose one of them, which will be written to the buffer in `out_device`.
int audiodevice_get_interactive(ma_context *context,
                                ma_device_info *out_device);
// Get device info by the index with which it appears in the output of
// audiodevice_get_interactive(), which will be written to the buffer in
// `out_device`.
int audiodevice_get_by_index(ma_context *context, size_t index,
                             ma_device_info *out_device);

#endif
