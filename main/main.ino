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

CRGB leds[NUM_LEDS];

int startupFocal = -1;
bool startupShelfOn = true;
// TODO, set these to start up rainbow
int startupDelayTime = 0;
int startupWhiteValues[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int startupBrightnessValues[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int startupEffectNumber = 0;
String startupColors[] = { "#000000", "#000000", "#000000", "#000000", "#000000", "#000000", "#000000", "#000000",
"#000000", "#000000", "#000000", "#000000", "#000000", "#000000", "#000000", "#000000" };

int focal = -1;
bool shelfOn = false;
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
void currentSettingPrint();
void ledSetup();
void setLed(int L, String hex, int W, int Brightness);
void selectEffect(int effectNumber);
void StuckInABlender();
void Smolder();

void setup() {
	Serial.begin(9600);
	while (!Serial) { delay(100); }
	connectToWifi();
	ledSetup();
}

void loop() {
	handleWebServer();
    currentSettingPrint();

    if (!shelfOn) {
        // Set all of colors to black
		for (int i = 0; i < LIGHT_COUNT; i++) {
			setLed(i, "#000000", 0, 0);
		}
    }
    else {
        // TODO: Check focal point
    // magsensor.check()
    // magsensor.getFocalPoint()
        selectEffect(effectNumber);
    }

}

void Smolder() {
    if (focal == -1) {
        delay(delayTime);

        for (int xy = 0; xy < COLOR_COUNT; xy++) {
            for (int j = 0; j < 18; j += 2) {
                setLed(j, colors[xy], whiteValues[xy], brightnessValues[xy]);
                int f = 0;
                if (j == 8) {
                    f = (xy + 1) % COLOR_COUNT;
                    setLed(j, colors[f], whiteValues[f], brightnessValues[f]);
                }
                if (j == 12) {
                    f = (xy + 2) % COLOR_COUNT;
                    setLed(j, colors[f], whiteValues[f], brightnessValues[f]);
                }
                f = (xy + 3) % COLOR_COUNT;
                setLed(j + 1, colors[f], whiteValues[f], brightnessValues[f]);
            }

            for (int j = 1; j < LIGHT_COUNT + 3; j += 2) {
                setLed(j, colors[xy], whiteValues[xy], brightnessValues[xy]);
                int f = (xy + 3) % COLOR_COUNT;
                setLed(j - 1, colors[f], whiteValues[f], brightnessValues[f]);
            }
        }
    }
    else {
        delay(delayTime);
        for (int xy = 0; xy < COLOR_COUNT; xy++) {
            int f = 0;

            for (int j = focal; j < LIGHT_COUNT; j += 2) {
                setLed(j, colors[xy], whiteValues[xy], brightnessValues[xy]);
                if (j == 8) {
                    f = (xy + 1) % COLOR_COUNT;
					setLed(j, colors[f], whiteValues[f], brightnessValues[f]);
                }

                if (j == 12) {
                    f = (xy + 2) % COLOR_COUNT;
					setLed(j, colors[f], whiteValues[f], brightnessValues[f]);
                }
                f = (xy + 3) % COLOR_COUNT;
                int led = (j + 1) % LIGHT_COUNT;
				setLed(led, colors[f], whiteValues[f], brightnessValues[f]);
            }

            for (int j = focal; j >= 0; j -= 2) {
				setLed(j, colors[xy], whiteValues[xy], brightnessValues[xy]);
                if (j == 8) {
                    f = (xy + 1) % COLOR_COUNT;
					setLed(j, colors[f], whiteValues[f], brightnessValues[f]);
                }
                if (j == 12) {
                    f = (xy + 2) % COLOR_COUNT;
					setLed(j, colors[f], whiteValues[f], brightnessValues[f]);
                }
                int led = (j + 1) % LIGHT_COUNT;
                f = (xy + 3) % COLOR_COUNT;
				setLed(led, colors[f], whiteValues[f], brightnessValues[f]);
            }

            for (int j = focal; j < LIGHT_COUNT + 1; j += 2) {
				setLed(j, colors[xy], whiteValues[xy], brightnessValues[xy]);
                f = (xy + 3) % COLOR_COUNT;
				setLed(j - 1, colors[f], whiteValues[f], brightnessValues[f]);
            }

            for (int j = focal; j >= 0; j -= 2) {
				setLed(j, colors[xy], whiteValues[xy], brightnessValues[xy]);
				f = (xy + 3) % COLOR_COUNT;
				setLed(j - 1, colors[f], whiteValues[f], brightnessValues[f]);
            }
        }
        
    }
}

void StuckInABlender() {
    unsigned long currentTime = millis();
	int colorOffset = (currentTime / 100) % COLOR_COUNT;

    if (focal == -1) {
		for (int i = 0; i < LIGHT_COUNT; i++) {
            int colorIndex = (i + colorOffset) % COLOR_COUNT;
			setLed(i, colors[colorIndex], whiteValues[colorIndex], brightnessValues[colorIndex]);
            delay(delayTime);

		}
    }
    else {
		for (int i = 0; i < LIGHT_COUNT/2; i++) {
			int led1 = focal - i;
			int led2 = focal + i;

			if (led1 < 0) led1 = LIGHT_COUNT + led1;
			if (led2 >= LIGHT_COUNT) led2 = led2 - LIGHT_COUNT;

			int colorIndex = (i + colorOffset) % COLOR_COUNT;
			setLed(led1, colors[colorIndex], whiteValues[colorIndex], brightnessValues[colorIndex]);
			setLed(led2, colors[colorIndex], whiteValues[colorIndex], brightnessValues[colorIndex]);
			delay(delayTime);
		}
    }

}

void selectEffect(int effectNumber) {
    switch (effectNumber) {
    case 0: 
		StuckInABlender(); // BLENDER originally
        break;
    case 1: 
        Smolder(); // CHRISTMAS originally
		break;
    case 2:
        // The Piano Man / Comfort Song
		break;
    case 3: 
        // Funky / ??
        break;
    case 4: 
        // Mold / ??
		break;
    case 5:
        // Progressive / Iterative
		break;
	case 6:
		// Still
		break;
	case 7:
        // Iterative Strobe / Strobe Change
		break;
	case 8:
		// Techno
        break;
	case 9:
        // Iterative Tracer / Trace Many / ?
        break;
    case 10:
        // Snake / TraceOne
        break;
    case 11:
        // State of Trance
		break;
    default: 
		// Invalid effect number
		Serial.println("Invalid effect number");
		break;
    }
}


void setLed(int L, String hex, int W, int Brightness) {
    // hex to rgb
	int R = (int)strtol(hex.substring(1, 3).c_str(), nullptr, 16);
	int G = (int)strtol(hex.substring(3, 5).c_str(), nullptr, 16);
	int B = (int)strtol(hex.substring(5, 7).c_str(), nullptr, 16);

    // Configure brightness 
    R = (R * Brightness) / 255;
    G = (G * Brightness) / 255;
    B = (B * Brightness) / 255;
    W = (W * Brightness) / 255;

    // Set LED to color passed in.
    switch ((L + 1) % 3) {
    case 1:
        switch (L + 1) {
        case 1:
            leds[0].r = R;
            FastLED.show();
            leds[0].g = G;
            FastLED.show();
            leds[0].b = B;
            FastLED.show();
            leds[1].g = W;
            FastLED.show();
            break;
        case 4:
            leds[4].r = R;
            FastLED.show();
            leds[4].g = G;
            FastLED.show();
            leds[4].b = B;
            FastLED.show();
            leds[5].g = W;
            FastLED.show();
            break;
        case 7:
            leds[8].r = R;
            FastLED.show();
            leds[8].g = G;
            FastLED.show();
            leds[8].b = B;
            FastLED.show();
            leds[9].g = W;
            FastLED.show();
            break;
        case 10:
            leds[12].r = R;
            FastLED.show();
            leds[12].g = G;
            FastLED.show();
            leds[12].b = B;
            FastLED.show();
            leds[13].g = W;
            FastLED.show();
            break;
        case 13:
            leds[16].r = R;
            FastLED.show();
            leds[16].g = G;
            FastLED.show();
            leds[16].b = B;
            FastLED.show();
            leds[17].g = W;
            FastLED.show();
            break;
        case 16:
            leds[20].r = R;
            FastLED.show();
            leds[20].g = G;
            FastLED.show();
            leds[20].b = B;
            FastLED.show();
            leds[21].g = W;
            FastLED.show();
            break;
        }
        break;
    case 2:
        switch (L + 1) {
        case 2:
            leds[1].r = G;
            FastLED.show();
            leds[1].b = R;
            FastLED.show();
            leds[2].r = W;
            FastLED.show();
            leds[2].g = B;
            FastLED.show();
            break;
        case 5:
            leds[5].b = R;
            FastLED.show();
            leds[5].r = G;
            FastLED.show();
            leds[6].g = B;
            FastLED.show();
            leds[6].r = W;
            FastLED.show();
            break;
        case 8:
            leds[9].r = G;
            FastLED.show();
            leds[9].b = R;
            FastLED.show();
            leds[10].r = W;
            FastLED.show();
            leds[10].g = B;
            FastLED.show();
            break;
        case 11:
            leds[13].r = G;
            FastLED.show();
            leds[13].b = R;
            FastLED.show();
            leds[14].r = W;
            FastLED.show();
            leds[14].g = B;
            FastLED.show();
            break;
        case 14:
            leds[17].b = R;
            FastLED.show();
            leds[17].r = G;
            FastLED.show();
            leds[18].g = B;
            FastLED.show();
            leds[18].r = W;
            FastLED.show();
            break;
        }
        break;
    case 0:
        switch (L + 1) {
        case 3:
            leds[3].r = B;
            FastLED.show();
            leds[3].g = R;
            FastLED.show();
            leds[3].b = W;
            FastLED.show();
            leds[2].b = G;
            FastLED.show();
            break;
        case 6:
            leds[7].r = B;
            FastLED.show();
            leds[7].g = R;
            FastLED.show();
            leds[7].b = W;
            FastLED.show();
            leds[6].b = G;
            FastLED.show();
            break;
        case 9:
            leds[11].g = R;
            FastLED.show();
            leds[10].b = G;
            FastLED.show();
            leds[11].r = B;
            FastLED.show();
            leds[11].b = W;
            FastLED.show();
            break;
        case 12:
            leds[15].r = B;
            FastLED.show();
            leds[15].g = R;
            FastLED.show();
            leds[15].b = W;
            FastLED.show();
            leds[14].b = G;
            FastLED.show();
            break;
        case 15:
            leds[19].r = B;
            FastLED.show();
            leds[19].g = R;
            FastLED.show();
            leds[19].b = W;
            FastLED.show();
            leds[18].b = G;
            FastLED.show();
            break;
        }
        break;
    }
	
}

void ledSetup() {
	FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

    // Set master values to startup settings
	shelfOn = startupShelfOn;
	focal = startupFocal;
	delayTime = startupDelayTime;
    effectNumber = startupEffectNumber;
	for (int i = 0; i < LIGHT_COUNT; i++) {
		whiteValues[i] = startupWhiteValues[i];
		brightnessValues[i] = startupBrightnessValues[i];
		colors[i] = startupColors[i];
	}
}

void currentSettingPrint() {
	if (!shelfOn) {
		Serial.println("Shelf is OFF");
	}
    else {
		Serial.println("Focal Point: " + String(focal));
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
				shelfOn = true;
            }
            else if (requestHeader.indexOf("GET /api/led/off") != -1) {
                client.println("{\"status\":\"LED OFF\"}");
                Serial.println("LED OFF command received");
				shelfOn = false;
            }
            else if (requestHeader.indexOf("GET /api/config") != -1) {
                shelfOn = true;
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
		if (effectNumber != doc["effectNumber"]) {
			for (int i = 0; i < LIGHT_COUNT; i++) {
				setLed(i, "#000000", 0, 0);
			}
		}
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
