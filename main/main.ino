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
int startupdelayTime = 3;
int startupWhiteValues[] = { 
    0, 0, 0, 0, 
    0, 0, 0, 0, 
    0, 0, 0, 0, 
    0, 0, 0, 0 
};
int startupBrightnessValues[] = { 
    255, 255, 255, 255, 
    255, 255, 255, 255, 
    255, 255, 255, 255, 
    255, 255, 255, 255 
};
int startupEffectNumber = 6;
String startupColors[] = { 
    "#ff0000", "#ff4400", "#ff6a00", "#ff9100",
    "#ffee00", "#00ff1e", "#00ff44", "#00ff95",
    "#00ffff", "#0088ff", "#0000ff", "#8800ff",
    "#ff00ff", "#ff00bb", "#ff0088", "#ff0044" 
};

int focal = -1;
bool shelfOn = false;
float delayTime = 0.0;
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
void ThePianoMan();
void FeelTheFunk();
void Decay();
void Cortez();
void Still();
void TheUnderground();
void BerghainBitte();
void LapisLazuli();
void Medusa();
void StateOfTrance();
void focalCheck(float delayTime);

void setup() {
	Serial.begin(9600);
	while (!Serial) { focalCheck(100.0); }
	connectToWifi();
	ledSetup();
}

void loop() {
    currentSettingPrint();
    handleWebServer();

    if (shelfOn) {
        selectEffect(effectNumber);
    }
    else {
        for (int i = 0; i < LIGHT_COUNT; i++) {
            setLed(i, "#000000", 0, 0);
        }
    }
}

void focalCheck(float delayTime) {
    handleWebServer();
    delay(delayTime);
    const byte HALL_PINS[] = { A0, A1, A2, A3, A4 }; 
    const int NUM_PINS = 5;

    const int BASE_READINGS[] = { 502, 510, 506, 500, 515 };
    const float THRESHOLDS[] = { 3.0, 2.9, 2.6, 2.5, 2.5 };             


    float highestVal = 0;
    int highestPin = -1;

    for (int i = 0; i < NUM_PINS; i++) {
        float sum = 0;
        float avgRaw = 0;

        for (int j = 0; j < 3; j++) {
            int rawReading = analogRead(HALL_PINS[i]);
            avgRaw += rawReading;
            float value = abs(rawReading - BASE_READINGS[i]);
            sum += value;
        }

        avgRaw /= 3.0;
        float value = sum / 3.0;

        if (value > THRESHOLDS[i]) {
            if (highestPin == -1 || value > highestVal) {
                highestVal = value;
                highestPin = i;
            }
        }
    }

    int focalPoint = -1;
    if (highestPin >= 0) {
        switch (HALL_PINS[highestPin]) {
        case A4: focalPoint = 2; break;
        case A3: focalPoint = 5; break;
        case A2: focalPoint = 8; break;
        case A1: focalPoint = 12; break;
        case A0: focalPoint = 15; break;
        }
    }
	focal = focalPoint;

    Serial.print("Focal point: ");
    Serial.println(focal);
}

// 11 -> DONE
// TODO: Does not rotate around the colors
void StateOfTrance() {
    int sc1 = 2; 
    int sc2 = 2;
    int ls = 6;
    if (focal == -1) {
        for (int j = 0; j < LIGHT_COUNT; j++) {
            for (int k = 0; k < sc1; k++) {
                if (effectNumber != 11) return;
                for (int i = 0; i < ls; i++) {
                    int li = j + i;
                    setLed((li + 1) % LIGHT_COUNT, colors[li % COLOR_COUNT], whiteValues[li % COLOR_COUNT], brightnessValues[li % COLOR_COUNT]);
                    delay(delayTime * 4);
                    setLed((li + 1) % LIGHT_COUNT, "#000000", 0, 0);
                    delay(delayTime * 4);

                }
            }

            for (int strobe = 0; strobe < sc2; strobe++) {
                if (effectNumber != 11) return;

                for (int i = 0; i < ls; i++) {
                    int li = j + i;
                    setLed((li + 1) % LIGHT_COUNT, colors[li % COLOR_COUNT], whiteValues[li % COLOR_COUNT], brightnessValues[li % COLOR_COUNT]);
                    delay(delayTime * 4);
                    setLed((li + 1) % LIGHT_COUNT, "#000000", 0, 0);
                    delay(delayTime * 4);

                }
       
            }
        }
    }
    else {
        for (int j = 0; j < LIGHT_COUNT; j++) {
            int distance = abs(focal - j);  

            for (int k = 0; k < sc1; k++) {
                if (effectNumber != 11) return;

                for (int i = 0; i < ls; i++) {
                    int li = distance + i;  
                    if (li < LIGHT_COUNT) {     
						setLed(li + 1, colors[li % COLOR_COUNT], whiteValues[li % COLOR_COUNT], brightnessValues[li % COLOR_COUNT]);
                    }
                }

                if ((j % 4 == 0) && (k % 4 == 0)) focalCheck(delayTime);
                else delay(delayTime);

                for (int i = 0; i < ls; i++) {
                    int ledIndex = distance + i;
                    if (ledIndex < LIGHT_COUNT) {
						setLed(ledIndex + 1, "#000000", 0, 0);
                    }
                }
                delay(delayTime);
            }

            for (int strobe = 0; strobe < sc2; strobe++) {
                if (effectNumber != 11) return;

                for (int i = 0; i < ls; i++) {
                    int ledIndex = distance + i;
                    if (ledIndex < LIGHT_COUNT) { 
						setLed(ledIndex + 1, colors[ledIndex % COLOR_COUNT], whiteValues[ledIndex % COLOR_COUNT], brightnessValues[ledIndex % COLOR_COUNT]);
                    }
                }
                delay(delayTime);

                for (int i = 0; i < ls; i++) {
                    int ledIndex = distance + i;
                    if (ledIndex < LIGHT_COUNT) { 
						setLed(ledIndex + 1, "#000000", 0, 0);
                    }
                }
                delay(delayTime);
            }
        }
    }
}

// 10 -> DONE
void Medusa() {
    if (focal != -1) {
        for (int kc = 0; kc < LIGHT_COUNT; kc++) {
            for (int i = 0; i < LIGHT_COUNT; i++) {
                if (effectNumber != 10) return;

                setLed(i, colors[kc], whiteValues[kc], brightnessValues[kc]);
            }

            for (int i = 1; i < COLOR_COUNT; i++) {
                for (int j = 0; j < LIGHT_COUNT / 2; j++) {
                    if (effectNumber != 10) return;

                    int position1 = (focal + 1 + j) % LIGHT_COUNT;
                    int position2 = (16 + focal - j) % LIGHT_COUNT;

                    setLed(position1, colors[i], whiteValues[i], brightnessValues[i]);
                    setLed(position2, colors[i], whiteValues[i], brightnessValues[i]);

                    if ((i % 4 == 0) && (j % 4 == 0)) focalCheck(delayTime);
                    else delay(delayTime);

                    setLed(position1, colors[kc], whiteValues[kc], brightnessValues[kc]);
                    setLed(position2, colors[kc], whiteValues[kc], brightnessValues[kc]);
                }
            }
        }
    }
    else {
		for (int kc = 0; kc < LIGHT_COUNT; kc++) {
			for (int i = 0; i < LIGHT_COUNT; i++) {
				if (effectNumber != 10) return;
				setLed(i, colors[kc], whiteValues[kc], brightnessValues[kc]);
			}
            for (int i = 0; i < COLOR_COUNT; i++) {
                for (int j = 0; j < LIGHT_COUNT; j++) {
                    if (effectNumber != 10) return;

                    setLed(j, colors[i], whiteValues[i], brightnessValues[i]);

                    if ((i % 4 == 0) && (j % 4 == 0)) focalCheck(delayTime * 2);
                    else delay(delayTime * 2);

                    setLed(j, colors[kc], whiteValues[kc], brightnessValues[kc]);
                }
            }                               
        }
    }
}

// 9 -> DONE?
void LapisLazuli() {
    for (int i = 0; i < LIGHT_COUNT; i++) {
		setLed(i, colors[0], whiteValues[0], brightnessValues[0]);
    }

    if (focal == -1) {
        for (int i = 0; i < LIGHT_COUNT; i++) {
            for (int j = 0; j < LIGHT_COUNT / 2; j++) {
                if (effectNumber != 9) return;

                int colorIndex1 = ((i + 1) % (COLOR_COUNT / 2));
                int colorIndex2 = ((i + 2) % COLOR_COUNT);

                int offset = (i + j * 2) % LIGHT_COUNT;
				setLed(offset, colors[colorIndex1], whiteValues[colorIndex1], brightnessValues[colorIndex1]);
                
                delay(delayTime * 2);

                offset = (i + j * 2 + 8) % LIGHT_COUNT;
				setLed(offset, colors[colorIndex2], whiteValues[colorIndex2], brightnessValues[colorIndex2]);
                if (i % 4 == 0) focalCheck(delayTime * 2);
                else delay(delayTime * 2);
            }
        }
    }
    else {
        for (int i = 0; i < COLOR_COUNT; i++) {
            for (int j = 0; j < LIGHT_COUNT / 2; j++) {
                if (effectNumber != 9) return;

                int position1 = (focal + 1 + j) % LIGHT_COUNT;
                int position2 = (LIGHT_COUNT + focal - j) % LIGHT_COUNT;
                int colorIndex1 = (i + 1) % COLOR_COUNT;
                int colorIndex2 = (i + 2) % COLOR_COUNT;
				setLed(position1, colors[colorIndex1], whiteValues[colorIndex1], brightnessValues[colorIndex1]);
				setLed(position2, colors[colorIndex2], whiteValues[colorIndex2], brightnessValues[colorIndex2]);
                if (i % 4 == 0) focalCheck(delayTime);
                else delay(delayTime);
            }
        }
    }
}

// 8 -> DONE
void BerghainBitte() {
	for (int i = 0; i < LIGHT_COUNT; i++) {
		setLed(i, "#000000", 0, 0);
	}
    if (focal == -1) {
        for (int i = 0; i < COLOR_COUNT; i++) {
            int m = (i + 1) % COLOR_COUNT;
            int n = (i + 2) % COLOR_COUNT;
            int o = (i + 3) % COLOR_COUNT;
            int p = (i + 4) % COLOR_COUNT;

            for (int j = 15; j >= 0; j--) {
                int k = (j + 1) % LIGHT_COUNT;
                int l = (j + 2) % LIGHT_COUNT;
                int y = (j + 3) % LIGHT_COUNT;
                int z = (j + 4) % LIGHT_COUNT;

                for (int x = 0; x < 2; x++) {
                    if (effectNumber != 8) return;

					setLed(j, colors[i], whiteValues[i], brightnessValues[i]);
                    delay(delayTime*2);
					setLed(j, "#000000", 0, 0);
					setLed(k, colors[m], whiteValues[m], brightnessValues[m]);
                    delay(delayTime*2);
					setLed(k, "#000000", 0, 0);
					setLed(l, colors[n], whiteValues[n], brightnessValues[n]);
                    delay(delayTime*2);
					setLed(l, "#000000", 0, 0);
					setLed(y, colors[o], whiteValues[o], brightnessValues[o]);
                    delay(delayTime*2);
					setLed(y, "#000000", 0, 0);
					setLed(z, colors[p], whiteValues[p], brightnessValues[p]);
                    focalCheck(delayTime*2);
					setLed(z, "#000000", 0, 0);
                }
            }
        }
        focalCheck(delayTime);
    }
    else {
        for (int i = 4; i < COLOR_COUNT + 4; i++) {
            int m = (i + 1) % COLOR_COUNT;
            int n = (i + 2) % COLOR_COUNT;
            int o = (i + 3) % COLOR_COUNT;
            int p = (i + 4) % COLOR_COUNT;

            int jb = focal;

            for (int j = focal; j >= 0; j--) {

                int k = (j + 1) % LIGHT_COUNT;
                int l = (j + 2) % LIGHT_COUNT;
                int y = (j + 3) % LIGHT_COUNT;
                int z = (j + 4) % LIGHT_COUNT;

                int kb = (jb + 1) % LIGHT_COUNT;
                int lb = (jb + 2) % LIGHT_COUNT;
                int yb = (jb + 3) % LIGHT_COUNT;
                int zb = (jb + 4) % LIGHT_COUNT;

                for (int x = 0; x < 2; x++) {
                    if (effectNumber != 8) return;

					setLed(j, colors[i], whiteValues[i], brightnessValues[i]);
					setLed(jb, colors[i], whiteValues[i], brightnessValues[i]);

                    delay(delayTime * 2);

					setLed(j, "#000000", 0, 0);
					setLed(jb, "#000000", 0, 0);

					setLed(k, colors[m], whiteValues[m], brightnessValues[m]);
					setLed(kb, colors[m], whiteValues[m], brightnessValues[m]);

                    delay(delayTime * 2);

					setLed(k, "#000000", 0, 0);
					setLed(kb, "#000000", 0, 0);

					setLed(l, colors[n], whiteValues[n], brightnessValues[n]);
					setLed(lb, colors[n], whiteValues[n], brightnessValues[n]);

                    delay(delayTime * 2);

					setLed(l, "#000000", 0, 0);
					setLed(lb, "#000000", 0, 0);

					setLed(y, colors[o], whiteValues[o], brightnessValues[o]);
					setLed(yb, colors[o], whiteValues[o], brightnessValues[o]);

                    delay(delayTime * 2);

					setLed(y, "#000000", 0, 0);
					setLed(yb, "#000000", 0, 0);

					setLed(z, colors[p], whiteValues[p], brightnessValues[p]);
					setLed(zb, colors[p], whiteValues[p], brightnessValues[p]);

                    focalCheck(delayTime * 2);

					setLed(z, "#000000", 0, 0);
					setLed(zb, "#000000", 0, 0);
                    jb++;
                }
            }

            for (int j = 0; j < focal; j++) {
                int k = (j + 1) % LIGHT_COUNT;
                int l = (j + 2) % LIGHT_COUNT;
                int y = (j + 3) % LIGHT_COUNT;
                int z = (j + 4) % LIGHT_COUNT;

                int kb = (jb + 1) % LIGHT_COUNT;
                int lb = (jb + 2) % LIGHT_COUNT;
                int yb = (jb + 3) % LIGHT_COUNT;
                int zb = (jb + 4) % LIGHT_COUNT;

                for (int x = 0; x < 2; x++) {
                    if (effectNumber != 8) return;

					setLed(j, colors[i], whiteValues[i], brightnessValues[i]);
					setLed(jb, colors[i], whiteValues[i], brightnessValues[i]);

                    delay(delayTime * 2);

					setLed(j, "#000000", 0, 0);
					setLed(jb, "#000000", 0, 0);

					setLed(k, colors[m], whiteValues[m], brightnessValues[m]);
					setLed(kb, colors[m], whiteValues[m], brightnessValues[m]);

                    delay(delayTime * 2);

					setLed(k, "#000000", 0, 0);
					setLed(kb, "#000000", 0, 0);

					setLed(l, colors[n], whiteValues[n], brightnessValues[n]);
					setLed(lb, colors[n], whiteValues[n], brightnessValues[n]);

                    delay(delayTime * 2);

					setLed(l, "#000000", 0, 0);
					setLed(lb, "#000000", 0, 0);

					setLed(y, colors[o], whiteValues[o], brightnessValues[o]);
					setLed(yb, colors[o], whiteValues[o], brightnessValues[o]);

                    delay(delayTime * 2);

					setLed(y, "#000000", 0, 0);
					setLed(yb, "#000000", 0, 0);

					setLed(z, colors[p], whiteValues[p], brightnessValues[p]);
					setLed(zb, colors[p], whiteValues[p], brightnessValues[p]);

                    delay(delayTime * 2);

					setLed(z, "#000000", 0, 0);
					setLed(zb, "#000000", 0, 0);
                }
                jb++;
            }
        }

    }
}

// 7 -> DONE
void TheUnderground() {
    if (focal == -1) {
        for (int i = 0; i < COLOR_COUNT; i++) {
            for (int j = 0; j < LIGHT_COUNT / 2; j++) {
                int offset = (i + j * 2) % LIGHT_COUNT;
                int colInd1 = ((i + 1) % (COLOR_COUNT / 2));
				int colInd2 = ((i + 2) % COLOR_COUNT);
                if (effectNumber != 7) return;

                delay(delayTime);
                setLed(offset, colors[colInd1], whiteValues[colInd1], brightnessValues[colInd1]);
                delay(delayTime * 2);
                offset = (i + j * 2 + 8) % LIGHT_COUNT;
                setLed(offset, colors[colInd2], whiteValues[colInd2], brightnessValues[colInd2]);
				delay(delayTime * 2);
                }
                focalCheck(0);
            }
        }
    else {
        for (int i = 0; i < COLOR_COUNT; i++) {
            for (int j = 0; j < LIGHT_COUNT / 2; j++) {
                int position1 = (focal + 1 + (i + j * 2) % LIGHT_COUNT) % LIGHT_COUNT;
                int position2 = (16 + focal - (i + j * 2) % LIGHT_COUNT) % LIGHT_COUNT;
                for (int k = 0; k < delayTime * 2; k++) {
                    if (effectNumber != 7) return;

					setLed(position1, "#000000", 0, 0);
					setLed(position2, "#000000", 0, 0);
                    delay(3);
					setLed(position1, colors[i], whiteValues[i], brightnessValues[i]);
					setLed(position2, colors[i], whiteValues[i], brightnessValues[i]);
                    if (j % 4 == 0) focalCheck(3.0);
                    else delay(3);
                }
            }
        }
    }
}

// 6 -> DONE
void Still() {
    for (int i = 0; i < LIGHT_COUNT; i++) {
		setLed(i, colors[i], whiteValues[i], brightnessValues[i]);
        if (i % 4 == 0) focalCheck(delayTime);
    }
    delay(2000);
}

// 5 -> DONE?
void Cortez() {
    if (focal != -1) {
        for (int j = 0; j < COLOR_COUNT; j++) {
            for (int i = 0; i < LIGHT_COUNT / 2; i++) {
                if (effectNumber != 5) return;

                int ledIndex = (focal + i) % LIGHT_COUNT;
                int ledIndex2 = (focal - i + LIGHT_COUNT) % LIGHT_COUNT;

				setLed(ledIndex, colors[j % COLOR_COUNT], whiteValues[j % COLOR_COUNT], brightnessValues[j % COLOR_COUNT]);
				setLed(ledIndex2, colors[j % COLOR_COUNT], whiteValues[j % COLOR_COUNT], brightnessValues[j % COLOR_COUNT]);
            
                delay(delayTime * 2);

                ledIndex = (focal + i + 1) % LIGHT_COUNT;
                ledIndex2 = (focal - i - 1 + LIGHT_COUNT) % LIGHT_COUNT;

				setLed(ledIndex, colors[j % COLOR_COUNT], whiteValues[j % COLOR_COUNT], brightnessValues[j % COLOR_COUNT]);
				setLed(ledIndex2, colors[j % COLOR_COUNT], whiteValues[j % COLOR_COUNT], brightnessValues[j % COLOR_COUNT]);

                if ((j % 4 == 0) && (i % 4 == 0)) focalCheck(delayTime * 2);
                else delay(delayTime * 2);
            }
        }
    }
    else {
        for (int j = 0; j < COLOR_COUNT; j++) {
            for (int i = 0; i < LIGHT_COUNT; i++) {
                if (effectNumber != 5) return;

                int ledIndex = (j + i) % LIGHT_COUNT;
                int ledIndex2 = (j + i + 1) % LIGHT_COUNT;

				setLed(ledIndex, colors[j % COLOR_COUNT], whiteValues[j % COLOR_COUNT], brightnessValues[j % COLOR_COUNT]);
				setLed(ledIndex2, colors[j % COLOR_COUNT], whiteValues[j % COLOR_COUNT], brightnessValues[j % COLOR_COUNT]);

                delay(delayTime);

                ledIndex = (j + i + 1) % LIGHT_COUNT;
                ledIndex2 = (j + i + 2) % LIGHT_COUNT;

				setLed(ledIndex, colors[j % COLOR_COUNT], whiteValues[j % COLOR_COUNT], brightnessValues[j % COLOR_COUNT]);
				setLed(ledIndex2, colors[j % COLOR_COUNT], whiteValues[j % COLOR_COUNT], brightnessValues[j % COLOR_COUNT]);

                if ((j % 4 == 0) && (i % 4 == 0)) focalCheck(delayTime);
                else delay(delayTime);
            }
        }
    }
}

// 4 -> DONE
void Decay() {
    int strobeCount1 = 2;
    int strobeCount2 = 2;
    int ledsPerGroup = LIGHT_COUNT * 3 / 4;
    if (focal == -1) {
        for (int startIdx = LIGHT_COUNT - 1; startIdx >= 0; startIdx--) {
            for (int strobe = 0; strobe < strobeCount1; strobe++) {
                focalCheck(0.0);

                for (int i = 0; i < ledsPerGroup; i++) {
                    if (effectNumber != 4) return;

                    int ledIndex = startIdx + i;
                    for (int ha = 0; ha < 4; ha++) {
                        setLed((ledIndex + 1) % LIGHT_COUNT, colors[ledIndex % COLOR_COUNT], whiteValues[ledIndex % COLOR_COUNT], brightnessValues[ledIndex % COLOR_COUNT]);
                        delay(delayTime / 12);
                        setLed(ledIndex % LIGHT_COUNT, "#000000", 0, 0);
                    }
                }

                for (int i = 0; i < ledsPerGroup; i++) {
                    int ledIndex = startIdx + i;
                    setLed((ledIndex + 1) % LIGHT_COUNT, "#000000", 0, 0);
                }
            }


            for (int strobe = 0; strobe < strobeCount2; strobe++) {
                for (int i = 0; i < ledsPerGroup; i++) {
                    if (effectNumber != 4) return;

                    int ledIndex = startIdx + i;
                    for (int ha = 0; ha < 4; ha++) {
                        setLed((ledIndex + 1) % LIGHT_COUNT, colors[ledIndex % COLOR_COUNT], whiteValues[ledIndex % COLOR_COUNT], brightnessValues[ledIndex % COLOR_COUNT]);
						focalCheck(delayTime / 12);
						setLed(ledIndex % LIGHT_COUNT, "#000000", 0, 0);
                    }
                }
                for (int i = 0; i < ledsPerGroup; i++) {
                    int ledIndex = startIdx + i;
					setLed(ledIndex + 1, "#000000", 0, 0);
                }
            }
        }
        for (int startIdx = 0; startIdx < LIGHT_COUNT; startIdx++) {
            for (int strobe = 0; strobe < strobeCount1; strobe++) {
                for (int i = 0; i < ledsPerGroup; i++) {
                    int ledIndex = startIdx + i;
                    for (int ha = 0; ha < 4; ha++) {
                        if (effectNumber != 4) return;

                        setLed((ledIndex + 1) % LIGHT_COUNT, colors[ledIndex % COLOR_COUNT], whiteValues[ledIndex % COLOR_COUNT], brightnessValues[ledIndex % COLOR_COUNT]);
						delay(delayTime / 12);
                        setLed((ledIndex) % LIGHT_COUNT, "#000000", 0, 0);
                    }
                }
                for (int i = 0; i < ledsPerGroup; i++) {
                    int ledIndex = startIdx + i;
					setLed(ledIndex + 1, "#000000", 0, 0);
                }
            }


            for (int strobe = 0; strobe < strobeCount2; strobe++) {
                for (int i = 0; i < ledsPerGroup; i++) {
                    int ledIndex = startIdx + i;
                    for (int ha = 0; ha < 4; ha++) {
                        if (effectNumber != 4) return;

                        setLed((ledIndex + 1) % LIGHT_COUNT, colors[ledIndex % COLOR_COUNT], whiteValues[ledIndex % COLOR_COUNT], brightnessValues[ledIndex % COLOR_COUNT]);
                        delay(delayTime / 12);
                        setLed(ledIndex % LIGHT_COUNT, "#000000", 0, 0);
                    }
                }
                for (int i = 0; i < ledsPerGroup; i++) {
                    int ledIndex = startIdx + i;
					setLed(ledIndex + 1, "#000000", 0, 0);
                }
            }

        }
    }
    else {
        for (int startIdx = LIGHT_COUNT - 1; startIdx >= focal; startIdx--) {
            for (int startIdx = focal; startIdx < LIGHT_COUNT; startIdx++) {
                for (int strobe = 0; strobe < strobeCount1; strobe++) {
                    focalCheck(0.0);

                    for (int i = 0; i < ledsPerGroup; i++) {
                        int ledIndex = startIdx + i;
                        int ledIndex2 = startIdx + i - 8;
                        for (int ha = 0; ha < 4; ha++) {
                            if (effectNumber != 4) return;

							setLed(ledIndex + 1, colors[ledIndex % COLOR_COUNT], whiteValues[ledIndex % COLOR_COUNT], brightnessValues[ledIndex % COLOR_COUNT]);
							setLed(ledIndex2 + 1, colors[ledIndex % COLOR_COUNT], whiteValues[ledIndex % COLOR_COUNT], brightnessValues[ledIndex % COLOR_COUNT]);
							setLed(ledIndex, "#000000", 0, 0);
							setLed(ledIndex2, "#000000", 0, 0);
                        }
                    }
                    for (int i = 0; i < ledsPerGroup; i++) {
                        int ledIndex = startIdx + i;
                        int ledIndex2 = startIdx - i;
						setLed(ledIndex + 1, "#000000", 0, 0);
						setLed(ledIndex2 + 1, "#000000", 0, 0);
                    }
                }

                focalCheck(delayTime);

                for (int strobe = 0; strobe < strobeCount2; strobe++) {
                    for (int i = 0; i < ledsPerGroup; i++) {
                        int ledIndex = startIdx + i;
                        int ledIndex2 = startIdx - i;
                        for (int ha = 0; ha < 4; ha++) {
                            if (effectNumber != 4) return;

							setLed(ledIndex + 1, colors[ledIndex % COLOR_COUNT], whiteValues[ledIndex % COLOR_COUNT], brightnessValues[ledIndex % COLOR_COUNT]);
							setLed(ledIndex2 + 1, colors[ledIndex % COLOR_COUNT], whiteValues[ledIndex % COLOR_COUNT], brightnessValues[ledIndex % COLOR_COUNT]);
							setLed(ledIndex, "#000000", 0, 0);
							setLed(ledIndex2, "#000000", 0, 0);
                        }
                    }
                    for (int i = 0; i < ledsPerGroup; i++) {
                        int ledIndex = startIdx + i;
                        int ledIndex2 = startIdx - i;
						setLed(ledIndex + 1, "#000000", 0, 0);
						setLed(ledIndex2 + 1, "#000000", 0, 0);
                    }
                }

                focalCheck(delayTime);
            }
        }
    }
}

// 3 -> DONE?
void FeelTheFunk() {
    int strobeCount1 = 12;
    int strobeCount2 = 12;
    int ledsPerGroup = 4;

    if (focal == -1) {
        for(int colorer = 0; colorer < COLOR_COUNT; colorer++) {
            for (int strobe = 0; strobe < strobeCount1; strobe++) {
                delay(delayTime * 12);

                for (int i = 0; i < ledsPerGroup; i++) {
                    if (effectNumber != 3) return;

                    int ledIndex = random(0, LIGHT_COUNT);
                    setLedChill(ledIndex + 1, colors[(ledIndex + colorer) % COLOR_COUNT], whiteValues[(ledIndex + colorer) % COLOR_COUNT], brightnessValues[(ledIndex + colorer) % COLOR_COUNT]);
                    delay(delayTime);
                }

                delay(delayTime * 12);


                for (int i = 0; i < ledsPerGroup; i++) {
                    if (effectNumber != 3) return;
                    int ledIndex = random(0, LIGHT_COUNT);
                    setLedChill(ledIndex + 1, "#000000", 0, 0);
                    delay(delayTime);

                }
            }


            for (int strobe = 0; strobe < strobeCount2; strobe++) {
                delay(delayTime * 12);

                for (int i = 0; i < ledsPerGroup; i++) {
                    if (effectNumber != 3) return;
                    int ledIndex = random(0, LIGHT_COUNT);
                    setLedChill(ledIndex + 1, colors[(ledIndex + colorer) % COLOR_COUNT], whiteValues[(ledIndex + colorer) % COLOR_COUNT], brightnessValues[(ledIndex + colorer) % COLOR_COUNT]);
                    delay(delayTime);

                }

                delay(delayTime * 12);


                for (int i = 0; i < ledsPerGroup; i++) {
                    if (effectNumber != 3) return;
                    int ledIndex = random(0, LIGHT_COUNT);
                    setLedChill(ledIndex + 1, "#000000", 0, 0);
                    delay(delayTime);

                }
            }
        }
        focalCheck(0);
    }
    else {
        focalCheck(delayTime / 12);

        for (int strobe = focal; strobe < strobeCount1; strobe++) {

            for (int i = 0; i < ledsPerGroup; i++) {
                if (effectNumber != 3) return;

                int ledIndex = random(0, LIGHT_COUNT);
                int ledIndex2 = random(0, LIGHT_COUNT);
				setLedChill(ledIndex + 1, colors[ledIndex % COLOR_COUNT], whiteValues[ledIndex % COLOR_COUNT], brightnessValues[ledIndex % COLOR_COUNT]);
                setLedChill(ledIndex2 + 1, colors[ledIndex % COLOR_COUNT], whiteValues[ledIndex % COLOR_COUNT], brightnessValues[ledIndex % COLOR_COUNT]);
            }


            for (int i = focal; i < ledsPerGroup; i++) {
                if (effectNumber != 3) return;

                int ledIndex = random(0, LIGHT_COUNT);
                int ledIndex2 = random(0, LIGHT_COUNT);
                setLedChill(ledIndex + 1, "#000000", 0, 0);
                setLedChill(ledIndex2 + 1, "#000000", 0, 0);
            }
        }

        delay(delayTime / 12);


        for (int strobe = focal + 1; strobe >= 0; strobe--) {

            for (int i = 0; i < ledsPerGroup; i++) {
                if (effectNumber != 3) return;

                int ledIndex = random(0, LIGHT_COUNT);
                int ledIndex2 = random(0, LIGHT_COUNT);
                setLedChill(ledIndex + 1, colors[ledIndex % COLOR_COUNT], whiteValues[ledIndex % COLOR_COUNT], brightnessValues[ledIndex % COLOR_COUNT]);
                setLedChill(ledIndex2 + 1, colors[ledIndex % COLOR_COUNT], whiteValues[ledIndex % COLOR_COUNT], brightnessValues[ledIndex % COLOR_COUNT]);
            }


            for (int i = focal; i < ledsPerGroup; i++) {
                if (effectNumber != 3) return;

                int ledIndex = random(0, LIGHT_COUNT);
                int ledIndex2 = random(0, LIGHT_COUNT);
                setLedChill(ledIndex + 1, "#000000", 0, 0);
                setLedChill(ledIndex2 + 1, "#000000", 0, 0);
            }
        }
    }
}

// 2 -> DONE?
void ThePianoMan() {
    // Create pattern indices that can wrap around the LIGHT_COUNT
    int patternIndices[] = { 1, 2, 3, 2, 4, 3, 2, 1, 0, 1, 2, 1, 3, 2, 1, 0 };
    int pattern2Indices[] = { 7, 8, 9, 8, 10, 9, 8, 7, 6, 7, 8, 7, 9, 8, 7, 6 };
    int pattern3Indices[] = { 13, 14, 15, 14, 16, 15, 14, 13, 12, 13, 14, 13, 15, 14, 13, 12 };

    if (focal == -1) {
        for (int x = 0; x < COLOR_COUNT * 2; x++) {
            focalCheck(0.0);

            for (int i = 0; i < 2; i++) {
                if (effectNumber != 2) return;

                // Apply modulo to ensure LED indices wrap around properly
                int index1 = patternIndices[x % LIGHT_COUNT] % LIGHT_COUNT;
                int index2 = pattern2Indices[x % LIGHT_COUNT] % LIGHT_COUNT;
                int index3 = pattern3Indices[x % LIGHT_COUNT] % LIGHT_COUNT;

                // Handle negative indices by adding LIGHT_COUNT
                if (index1 < 0) index1 += LIGHT_COUNT;
                if (index2 < 0) index2 += LIGHT_COUNT;
                if (index3 < 0) index3 += LIGHT_COUNT;

                setLedChill(index1, colors[x % LIGHT_COUNT], whiteValues[x % LIGHT_COUNT], brightnessValues[x % LIGHT_COUNT]);
                delay(delayTime * 4);
                setLedChill(index1, "#000000", 0, 0);

                setLedChill(index2, colors[x % LIGHT_COUNT], whiteValues[x % LIGHT_COUNT], brightnessValues[x % LIGHT_COUNT]);
                delay(delayTime * 4);
                setLedChill(index2, "#000000", 0, 0);

                setLedChill(index3, colors[x % LIGHT_COUNT], whiteValues[x % LIGHT_COUNT], brightnessValues[x % LIGHT_COUNT]);
                delay(delayTime * 4);
                setLedChill(index3, "#000000", 0, 0);

            }
        }
        focalCheck(delayTime);
    }
    else {
        // Create a new pattern for focal-based operation that wraps properly
        for (int x = 0; x < COLOR_COUNT * 2; x++) {
            for (int i = 0; i < delayTime / 2; i++) {
                if (effectNumber != 2) return;

                // Create a wave pattern from the focal point
                // These offset calculations ensure proper wrapping
                int offset = x % 8; // Use a smaller range for more visible movement

                // Calculate positions relative to focal point with wrapping
                int led1 = (focal + offset) % LIGHT_COUNT;
                int led2 = (focal - offset + LIGHT_COUNT) % LIGHT_COUNT;
                int led3 = (focal + offset + LIGHT_COUNT / 2) % LIGHT_COUNT;
                int led4 = (focal - offset + LIGHT_COUNT / 2 + LIGHT_COUNT) % LIGHT_COUNT;

                setLedChill(led1, colors[x % LIGHT_COUNT], whiteValues[x % LIGHT_COUNT], brightnessValues[x % LIGHT_COUNT]);
                setLedChill(led2, colors[(x + 2) % LIGHT_COUNT], whiteValues[(x + 2) % LIGHT_COUNT], brightnessValues[(x + 2) % LIGHT_COUNT]);
                setLedChill(led3, colors[(x + 4) % LIGHT_COUNT], whiteValues[(x + 4) % LIGHT_COUNT], brightnessValues[(x + 4) % LIGHT_COUNT]);
                setLedChill(led4, colors[(x + 6) % LIGHT_COUNT], whiteValues[(x + 6) % LIGHT_COUNT], brightnessValues[(x + 6) % LIGHT_COUNT]);

                setLedChill(led1, "#000000", 0, 0);
                setLedChill(led2, "#000000", 0, 0);
                setLedChill(led3, "#000000", 0, 0);
                setLedChill(led4, "#000000", 0, 0);

                focalCheck(2);
            }
        }
    }
}

// 1 -> DONE
void Smolder() {
    if (focal == -1) {
        for (int xy = 0; xy < COLOR_COUNT; xy++) {
            for (int j = 0; j < LIGHT_COUNT; j += 2) {
                if (effectNumber != 1) return;

                setLed(j % LIGHT_COUNT, colors[xy], whiteValues[xy], brightnessValues[xy]);

                int f = 0;
                if (j == 8) {
                    f = (xy + 1) % COLOR_COUNT;
                    focalCheck(delayTime / 16);
                    setLed(j % LIGHT_COUNT, colors[f], whiteValues[f], brightnessValues[f]);
                }

                if (j == 12) {
                    f = (xy + 2) % COLOR_COUNT;
                    delay(delayTime / 16);
                    setLed(j % LIGHT_COUNT, colors[f], whiteValues[f], brightnessValues[f]);
                }

                f = (xy + 3) % COLOR_COUNT;
                int nextLed = (j + 1) % LIGHT_COUNT;
                delay(delayTime / 16);
                setLed(nextLed, colors[f], whiteValues[f], brightnessValues[f]);
            }

            for (int j = 1; j < LIGHT_COUNT; j += 2) {
                if (effectNumber != 1) return;
				delay(delayTime / 16);
                setLed(j % LIGHT_COUNT, colors[xy], whiteValues[xy], brightnessValues[xy]);
                int f = (xy + 3) % COLOR_COUNT;

                int prevLed = (j - 1 + LIGHT_COUNT) % LIGHT_COUNT;
                delay(delayTime / 16);

                setLed(prevLed, colors[f], whiteValues[f], brightnessValues[f]);
            }
        }
    }
    else {
        for (int xy = 0; xy < COLOR_COUNT; xy++) {
            int f = 0;

            for (int j = focal; j < focal + LIGHT_COUNT; j += 2) {
                if (effectNumber != 1) return;

                int currentLed = j % LIGHT_COUNT;
                delay(delayTime / 16);

                setLed(currentLed, colors[xy], whiteValues[xy], brightnessValues[xy]);

                if (currentLed == 8) {
                    f = (xy + 1) % COLOR_COUNT;
                    focalCheck(delayTime / 16);
                    setLed(currentLed, colors[f], whiteValues[f], brightnessValues[f]);
                }

                if (currentLed == 12) {
                    f = (xy + 2) % COLOR_COUNT;
                    delay(delayTime / 16);
                    setLed(currentLed, colors[f], whiteValues[f], brightnessValues[f]);
                }

                f = (xy + 3) % COLOR_COUNT;
                int nextLed = (j + 1) % LIGHT_COUNT;
                delay(delayTime / 16);

                setLed(nextLed, colors[f], whiteValues[f], brightnessValues[f]);
            }

            for (int j = focal; j > focal - LIGHT_COUNT; j -= 2) {
                if (effectNumber != 1) return;

                int currentLed = (j + LIGHT_COUNT) % LIGHT_COUNT;
                delay(delayTime / 16);

                setLed(currentLed, colors[xy], whiteValues[xy], brightnessValues[xy]);

                if (currentLed == 8) {
                    f = (xy + 1) % COLOR_COUNT;
                    delay(delayTime / 16);
                    setLed(currentLed, colors[f], whiteValues[f], brightnessValues[f]);
                }

                if (currentLed == 12) {
                    f = (xy + 2) % COLOR_COUNT;
                    delay(delayTime / 16);
                    setLed(currentLed, colors[f], whiteValues[f], brightnessValues[f]);
                }

                f = (xy + 3) % COLOR_COUNT;
                int nextLed = ((j + 1) + LIGHT_COUNT) % LIGHT_COUNT;
                delay(delayTime / 16);

                setLed(nextLed, colors[f], whiteValues[f], brightnessValues[f]);
            }

            for (int offset = 0; offset < LIGHT_COUNT; offset++) {
                if (effectNumber != 1) return;

                int forwardLed = (focal + offset) % LIGHT_COUNT;
                if (offset % 2 == 0) {
                    delay(delayTime / 16);

                    setLed(forwardLed, colors[xy], whiteValues[xy], brightnessValues[xy]);
                    f = (xy + 3) % COLOR_COUNT;
                    int prevLed = (forwardLed - 1 + LIGHT_COUNT) % LIGHT_COUNT;
                    delay(delayTime / 16);

                    setLed(prevLed, colors[f], whiteValues[f], brightnessValues[f]);
                }
            }
        }
    }
}

// 0 -> DONE
void StuckInABlender() {
    unsigned long currentTime = millis();
	int colorOffset = (currentTime / 100) % COLOR_COUNT;

    if (focal == -1) {
		for (int i = 0; i < LIGHT_COUNT; i++) {
            if (effectNumber != 0) return;

            int colorIndex = (i + colorOffset) % COLOR_COUNT;
            delay(delayTime);

			setLed(i, colors[colorIndex], whiteValues[colorIndex], brightnessValues[colorIndex]);
		}
    }
    else {
		for (int i = 0; i < LIGHT_COUNT/2; i++) {
            if (effectNumber != 0) return;

			int led1 = focal - i;
			int led2 = focal + i;

			if (led1 < 0) led1 = LIGHT_COUNT + led1;
			if (led2 >= LIGHT_COUNT) led2 = led2 - LIGHT_COUNT;

			int colorIndex = (i + colorOffset) % COLOR_COUNT;
			setLed(led1, colors[colorIndex], whiteValues[colorIndex], brightnessValues[colorIndex]);
			setLed(led2, colors[colorIndex], whiteValues[colorIndex], brightnessValues[colorIndex]);
			focalCheck(delayTime / 8);
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
        ThePianoMan(); // COMFORT SONG originally
		break;
    case 3: 
        FeelTheFunk(); // FUNKY originally
        break;
    case 4: 
        Decay(); // MOLD originally
		break;
    case 5:
        Cortez(); // PROGRESSIVE originally
		break;
	case 6:
        Still(); // STILL
		break;
	case 7:
        TheUnderground(); // STROBE CHANGE originally
		break;
	case 8:
        BerghainBitte(); // TECHNO originally
        break;
	case 9:
        LapisLazuli(); // TRACE MANY originally
        break;
    case 10:
        Medusa(); // TRACE ONE originally
        break;
    case 11:
        StateOfTrance(); // TRANCE originally
		break;
    default: 
		Serial.println("Invalid effect number");
		break;
    }
}

void setLedChill(int L, String hex, int newW, int Brightness) {
    // hex to rgb
	int R = (int)strtol(hex.substring(1, 3).c_str(), nullptr, 16);
	int G = (int)strtol(hex.substring(3, 5).c_str(), nullptr, 16);
	int B = (int)strtol(hex.substring(5, 7).c_str(), nullptr, 16);

    // Configure brightness 
    R = (R * Brightness) / 255;
    G = (G * Brightness) / 255;
    B = (B * Brightness) / 255;
    int W = (newW * Brightness) / 255;

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

void setLed(int L, String hex, int newW, int Brightness) {
    // hex to rgb
    int newR = (int)strtol(hex.substring(1, 3).c_str(), nullptr, 16);
    int newG = (int)strtol(hex.substring(3, 5).c_str(), nullptr, 16);
    int newB = (int)strtol(hex.substring(5, 7).c_str(), nullptr, 16);
    int R = 0;
	int G = 0;
	int B = 0;
	int W = 0;

    for(int i = 0; i < 2; i ++) {
        R = 0;
        G = 0;
        B = 0;
        W = 0;

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

        R = (newR * Brightness) / 255;
        G = (newG * Brightness) / 255;
        B = (newB * Brightness) / 255;
        W = (newW * Brightness) / 255;
        

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

	
}

void ledSetup() {
	FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

    // Set master values to startup settings
	shelfOn = startupShelfOn;
	focal = startupFocal;
	delayTime = startupdelayTime;
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
        Serial.println("delayTime: " + String(delayTime));
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
				shelfOn = true;
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
            client.stop();
            return;
        }
        else if (requestHeader.indexOf("GET /api/status") != -1) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:application/json");
            client.println("Connection: close");
            client.println("Access-Control-Allow-Origin: *");
            client.println();

            StaticJsonDocument<128> doc;
            doc["shelfOn"] = shelfOn;

            String jsonResponse;
            serializeJson(doc, jsonResponse);
            client.println(jsonResponse);

            client.stop();
            return;
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
                client.stop();
                return;
            }
            else if (requestHeader.indexOf("GET /api/led/off") != -1) {
                client.println("{\"status\":\"LED OFF\"}");
                Serial.println("LED OFF command received");
				shelfOn = false;
                client.stop();
                return;
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

                client.stop();
                return;
            }
            else {
                client.println("{\"error\":\"Unknown command\"}");
            }
            client.stop();
            return;
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
            // TODO Restart loop() - Need to look up how
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
		focalCheck(2000.0);
	}

	Serial.print("IP Address: ");
	Serial.println(WiFi.localIP());

	server.begin();
}