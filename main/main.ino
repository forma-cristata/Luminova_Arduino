#include <FastLED.h>
#include <WiFiS3.h>
#include "index.h"
#include <ArduinoJson.h>

#define NUM_LEDS 22
#define DATA_PIN 6
#define SEED 42
#define LIGHT_COUNT 16
#define COLOR_COUNT 16

WiFiServer server(80);

const char ssid[] = "friskyfishes";
const char pass[] = "allthatjazz";


int delayTime = 0;
int whiteValues[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int brightnessValues[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int effectNumber = 0;
String colors[] = { "#000000", "#000000", "#000000", "#000000", "#000000", "#000000", "#000000", "#000000",
"#000000", "#000000", "#000000", "#000000", "#000000", "#000000", "#000000", "#000000" };

int status = WL_IDLE_STATUS;

void connectToWifi();
void handleWebServer();
void processJsonConfig(const String& jsonString);

void setup() {
	Serial.begin(9600);
	while (!Serial) { delay(100); }

	connectToWifi();

    //lightapp.setup -> ledcontroller.begin()
}

void loop() {
	handleWebServer();
	Serial.println("EffectNumber: " + String(effectNumber));
	Serial.println("DelayTime: " + String(delayTime));
	Serial.println("WhiteValues: ");
	for (int i = 0; i < LIGHT_COUNT; i++) {
		Serial.print(whiteValues[i]);
		Serial.print(" ");
	}
	Serial.println();
	Serial.println("BrightnessValues: ");
	for (int i = 0; i < LIGHT_COUNT; i++) {
		Serial.print(brightnessValues[i]);
		Serial.print(" ");
	}
	Serial.println();
	Serial.println("Colors: ");
	for (int i = 0; i < LIGHT_COUNT; i++) {
		Serial.print(colors[i]);
		Serial.print(" ");
	}
	Serial.println();

}

void ledSetup() {
}

void handleWebServer() {
    WiFiClient client = server.available();

    if (client) {
        Serial.println("New client connected");
        String currentLine = "";
        String requestHeader = "";
        String requestBody = "";
        bool isBody = false;
        int contentLength = 0;
        int bodyBytesRead = 0;

        unsigned long timeout = millis();

        // read header
        while (client.connected() && millis() - timeout < 5000) {
            if (client.available()) {
                timeout = millis();
                char c = client.read();
                requestHeader += c;

                if (c == '\n') {
                    if (currentLine.length() == 0) {
                        // End of headers
                        isBody = true;
                        break; 
                    }
                    else {
                        currentLine = "";
                    }
                }
                else if (c != '\r') {
                    currentLine += c;
                }
            }
        }

        // extract Content-Length
        if (requestHeader.indexOf("Content-Length:") != -1) {
            int start = requestHeader.indexOf("Content-Length:") + 15;
            int end = requestHeader.indexOf("\r\n", start);
            String lengthStr = requestHeader.substring(start, end);
            lengthStr.trim();
            contentLength = lengthStr.toInt();
            Serial.print("Content-Length found: ");
            Serial.println(contentLength);
        }

        // read the body
        if (isBody && contentLength > 0) {
            Serial.print("Reading body, expecting: ");
            Serial.print(contentLength);
            Serial.println(" bytes");

            timeout = millis();
            requestBody = "";

            while (bodyBytesRead < contentLength && client.connected() &&
                millis() - timeout < 10000) { 
                if (client.available()) {
                    timeout = millis();
                    char c = client.read();
                    requestBody += c;
                    bodyBytesRead++;

                    if (bodyBytesRead % 100 == 0 || bodyBytesRead == contentLength) {
                        Serial.print("Body bytes read: ");
                        Serial.print(bodyBytesRead);
                        Serial.print("/");
                        Serial.println(contentLength);
                    }
                }
            }

            Serial.println("Body received:");
            Serial.println(requestBody);
        }

        // Check if we got a complete body
        bool bodyComplete = (bodyBytesRead == contentLength);

        if (requestHeader.indexOf("POST /api/config") != -1) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:application/json");
            client.println("Connection: close");
            client.println("Access-Control-Allow-Origin: *");
            client.println();

            if (bodyComplete && requestBody.length() > 0) {
                Serial.println("Processing JSON config...");
                processJsonConfig(requestBody);
                client.println("{\"status\":\"Configuration updated successfully\"}");
            }
            else {
                Serial.println("Incomplete body received!");
                Serial.print("Expected: ");
                Serial.print(contentLength);
                Serial.print(" bytes, Got: ");
                Serial.print(bodyBytesRead);
                Serial.println(" bytes");
                client.println("{\"error\":\"Incomplete data received\"}");
            }
        }
        else if (requestHeader.indexOf("GET /api/") != -1) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:application/json");
            client.println("Connection: close");
            client.println("Access-Control-Allow-Origin: *");
            client.println();

            if (requestHeader.indexOf("GET /api/led/on") != -1) {
                client.println("{\"status\":\"LED ON\"}");
                Serial.println("LED ON command received");
            }
            else if (requestHeader.indexOf("GET /api/led/off") != -1) {
                client.println("{\"status\":\"LED OFF\"}");
                Serial.println("LED OFF command received");
            }
            else if (requestHeader.indexOf("GET /api/config") != -1) {
                StaticJsonDocument<1024> doc;
                doc["delayTime"] = delayTime;
                doc["effectNumber"] = effectNumber;

                JsonArray whiteArray = doc.createNestedArray("whiteValues");
                JsonArray brightnessArray = doc.createNestedArray("brightnessValues");
                JsonArray colorsArray = doc.createNestedArray("colors");

                for (int i = 0; i < LIGHT_COUNT; i++) {
                    whiteArray.add(whiteValues[i]);
                    brightnessArray.add(brightnessValues[i]);
                    colorsArray.add(colors[i]);
                }

                String jsonResponse;
                serializeJson(doc, jsonResponse);
                client.println(jsonResponse);
            }
            else {
                client.println("{\"error\":\"Unknown command\"}");
            }
        }
        else {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            client.print("Hello");
        }

        // Close the connection
        client.stop();
        Serial.println("Client disconnected");
    }
}


void processJsonConfig(const String& jsonString) {
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, jsonString);

    if (error) {
        Serial.print("JSON parsing failed: ");
        Serial.println(error.c_str());
        return;
    }

    // Update delayTime if present
    if (doc.containsKey("delayTime")) {
        delayTime = doc["delayTime"];
    }

    // Update effectNumber if present
    if (doc.containsKey("effectNumber")) {
        effectNumber = doc["effectNumber"];
    }

    // Update whiteValues if present
    if (doc.containsKey("whiteValues")) {
        JsonArray whiteArray = doc["whiteValues"].as<JsonArray>();
        int index = 0;
        for (JsonVariant value : whiteArray) {
            if (index < LIGHT_COUNT) {
                whiteValues[index++] = value.as<int>();
            }
        }
    }

    // Update brightnessValues if present
    if (doc.containsKey("brightnessValues")) {
        JsonArray brightnessArray = doc["brightnessValues"].as<JsonArray>();
        int index = 0;
        for (JsonVariant value : brightnessArray) {
            if (index < LIGHT_COUNT) {
                brightnessValues[index++] = value.as<int>();
            }
        }
    }

    // Update colors if present
    if (doc.containsKey("colors")) {
        JsonArray colorsArray = doc["colors"].as<JsonArray>();
        int index = 0;
        for (JsonVariant value : colorsArray) {
            if (index < LIGHT_COUNT) {
                colors[index++] = value.as<String>();
            }
        }
    }
}

void connectToWifi() {
	while (status != WL_CONNECTED) {
		Serial.print("Attempting to connect to SSID: ");
		Serial.println(ssid);
		status = WiFi.begin(ssid, pass);
		delay(2000);
	}

	Serial.print("IP Address: ");
	Serial.println(WiFi.localIP());

	server.begin();
}
