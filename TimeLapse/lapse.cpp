#include "Arduino.h"
#include "camera.h"
#include <stdio.h>
#include "file.h"

#define LED_BUILTIN 4

unsigned long fileIndex = 0;
unsigned long lapseIndex = 0;
unsigned long frameInterval = 5000;
bool mjpeg = true;
bool lapseRunning = true;
unsigned long lastFrameDelta = 0;

void setInterval(unsigned long delta)
{
    frameInterval = delta;
}

bool startLapse()
{
    if(lapseRunning) return true;
    fileIndex = 0;
    char path[32];
    for(; lapseIndex < 10000; lapseIndex++)
    {
        sprintf(path, "/lapse%03d", lapseIndex);
        if (!fileExists(path))
        {
            createDir(path);
            lastFrameDelta = 0;
            lapseRunning = true;
            return true;
        }
    }
	return false;
}

bool stopLapse()
{
    lapseRunning = false;
}

bool processLapse(unsigned long dt)
{
    //Serial.println("in processLapse");

    if(!lapseRunning) return false;

    lastFrameDelta += dt;
    if(lastFrameDelta >= frameInterval)
    {
        //pinMode (LED_BUILTIN, OUTPUT);
        //digitalWrite(LED_BUILTIN, LOW);
        Serial.println("LED turned ON");
        delay(500);
        lastFrameDelta -= frameInterval;
        camera_fb_t *fb = NULL;
        esp_err_t res = ESP_OK;
        fb = esp_camera_fb_get();
        if (!fb)
        {
	        Serial.println("Camera capture failed");
	        return false;
        }
        Serial.println("Camera capture OK");
        char path[32];
        sprintf(path, "/lapse%03d/pic%05d.jpg", lapseIndex, fileIndex);
        delay(500);
        //digitalWrite(LED_BUILTIN, HIGH);
        //Serial.println("LED turned OFF");
        Serial.println(path);
        if(!writeFile(path, (const unsigned char *)fb->buf, fb->len))
        {
            lapseRunning = false;
            return false;
        }
        fileIndex++;
        esp_camera_fb_return(fb);
    }

    return true;
}
