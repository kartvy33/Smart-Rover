#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <Arduino.h>
#include "esp_camera.h"

bool capturePhoto();

camera_fb_t* captureFrame();

void releaseFrame(camera_fb_t *fb);

#endif
