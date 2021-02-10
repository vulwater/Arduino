#include <ArduinoOTA.h>
#include <M5StickCPlus.h>
#include <NTPClient.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include <WiFiUdp.h>

void wifiSetup();
void ntpSync();

static const String AP_NAME_PREFIX = "ESP32-AP-";
static const String AP_PASSWORD = "whoknows";

void setup() {
    M5.begin();
    M5.Lcd.begin();
    M5.Lcd.setRotation(1);
    M5.Lcd.setTextWrap(false);
    M5.Lcd.setTextSize(2);

    wifiSetup();

    M5.Lcd.fillScreen(BLACK);

    if (WiFi.isConnected()) {
        M5.Lcd.println(WiFi.localIP());
        ntpSync();
        ArduinoOTA.begin();
    }
}

void loop() {
    ArduinoOTA.handle();
    M5.update();
    if (M5.BtnA.isPressed() && M5.BtnB.isPressed()) {
        WiFiManager wifiManager;
        wifiManager.resetSettings();
        wifiManager.reboot();
    }
}

static void wifiApCallback(WiFiManager* wm) {
    M5.Lcd.qrcode( "WIFI:T:WPA;S:" + wm->getConfigPortalSSID() + ";P:" + AP_PASSWORD + ";;", 55, 4, 130, 3);
}

void wifiSetup() {
    WiFiManager wifiManager;
    wifiManager.setAPCallback(wifiApCallback);
    String mac = WiFi.macAddress();
    int idx;
    while ((idx = mac.indexOf(':')) >= 0) {
        mac.remove(idx, 1);
    }
    String apName = AP_NAME_PREFIX + mac;
    wifiManager.autoConnect(apName.c_str(), AP_PASSWORD.c_str());
}

void ntpSync() {
    WiFiUDP ntpUDP;
    NTPClient timeClient(ntpUDP, "nl.pool.ntp.org", 3600, 600000);
    timeClient.begin();
    timeClient.forceUpdate();
    M5.Lcd.println(timeClient.getFormattedTime());
    RTC_TimeTypeDef TimeStruct;
    TimeStruct.Hours = timeClient.getHours();
    TimeStruct.Minutes = timeClient.getMinutes();
    TimeStruct.Seconds = timeClient.getSeconds();
    M5.Rtc.SetTime(&TimeStruct);
    timeClient.end();
}
