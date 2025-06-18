#ifndef _AUDIO_DEVICE
#define _AUDIO_DEVICE
/*
Enumeration and selection of audio devices.
*/

#include "miniaudio.h"

int audiodevice_get_interactive(ma_context *context,
                                ma_device_info *out_device);

#endif
