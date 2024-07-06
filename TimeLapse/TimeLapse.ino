#include <WiFi.h>
#include "time.h"
#include "wifinet.h"
#include "file.h"
#include "camera.h"
#include "lapse.h"

const char* ntpServer = "europe.pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

void startCameraServer();


void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void setup()
{
	Serial.begin(115200);
	Serial.setDebugOutput(true);
	Serial.println();

  Serial.println("Starting FileSystem ...");
	initFileSystem();
  Serial.println("Started FileSystem");
  Serial.println("Starting Camera ...");
	initCamera();
  Serial.println("Started Camera");

  Serial.print("Connecting to WiFi network with name '");
  Serial.print(ssid);
  Serial.println("' ...");
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");

  Serial.println("getting time from time server via NTP");
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  Serial.println("Starting Camera Server");
	startCameraServer();
	Serial.print("Camera server Ready! Use 'http://");
	Serial.print(WiFi.localIP());
	Serial.println("' to connect");
}

void loop()
{
	unsigned long t = millis();
	static unsigned long ot = 0;
	unsigned long dt = t - ot;
	ot = t;
  //Serial.println("Starting processing lapse");
	processLapse(dt);
}
