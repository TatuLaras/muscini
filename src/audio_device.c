#include "audio_device.h"
#include <stdio.h>

int audiodevice_get_interactive(ma_context *context,
                                ma_device_info *out_device) {

    ma_device_info *pCaptureDeviceInfos;
    ma_uint32 captureDeviceCount;
    ma_result result = ma_context_get_devices(
        context, 0, 0, &pCaptureDeviceInfos, &captureDeviceCount);

    if (result != MA_SUCCESS) {
        printf("Failed to enumerate audio devices.\n");
        return 1;
    }

    printf("Audio device not given via the '-d' flag (-d0, -d1, ...), choosing "
           "one interactively.\n");
    for (size_t i = 0; i < captureDeviceCount; ++i) {
        printf("%zu: %s\n", i, pCaptureDeviceInfos[i].name);
    }

    printf("Select device [0-%u]:\n", captureDeviceCount - 1);
    int user_char = getc(stdin);
    if (user_char == EOF)
        return 1;

    if (user_char < '0' || user_char > '9') {
        return 1;
    }

    size_t choice = user_char - '0';
    if (choice >= captureDeviceCount) {
        fprintf(stderr, "ERROR: Choice is out of range.\n");
        return 1;
    }

    if (out_device)
        *out_device = pCaptureDeviceInfos[choice];

    return 0;
}

int audiodevice_get_by_index(ma_context *context, size_t index,
                             ma_device_info *out_device) {

    ma_device_info *pCaptureDeviceInfos;
    ma_uint32 captureDeviceCount;
    ma_result result = ma_context_get_devices(
        context, 0, 0, &pCaptureDeviceInfos, &captureDeviceCount);

    if (result != MA_SUCCESS) {
        printf("Failed to retrieve device information.\n");
        return 1;
    }

    if (index >= captureDeviceCount) {
        fprintf(stderr, "ERROR: Device index is out of range.\n");
        return 1;
    }

    if (out_device)
        *out_device = pCaptureDeviceInfos[index];

    return 0;
}
