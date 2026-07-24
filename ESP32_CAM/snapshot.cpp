#include "snapshot.h"

#include "esp_camera.h"

bool capturePhoto()
{
    camera_fb_t *fb = esp_camera_fb_get();

    if (!fb)
    {
        Serial.println("Snapshot Failed");
        return false;
    }

    Serial.print("Photo Size : ");
    Serial.print(fb->len);
    Serial.println(" Bytes");

    esp_camera_fb_return(fb);

    return true;
}

camera_fb_t *captureFrame()
{
    return esp_camera_fb_get();
}

void releaseFrame(camera_fb_t *fb)
{
    if(fb!=NULL)
        esp_camera_fb_return(fb);
}
