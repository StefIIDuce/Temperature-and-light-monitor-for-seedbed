/* SevSeg Counter Example
 
 Copyright 2017 Dean Reading
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 
 
 This example demonstrates a very simple use of the SevSeg library with a 4
 digit display. It displays a counter that counts up, showing deci-seconds.
 */

#include "SevSeg.h"
SevSeg sevseg; //Instantiate a seven segment controller object
int zand[] = {0,1,2, A0};
int zandLijn[] = {A1,A2,A3, A4};
int button = A3;
int lm335 = A5; 
int buttstat = 0; 
int toggel = 0;
unsigned long timer = 0;
unsigned long zandloper = 0;
unsigned long zanddelay = 0;
int licht = 0;
void setup() {
    for (int thisPin = 0; thisPin < 6; thisPin++) {
        pinMode(zandLijn[thisPin], OUTPUT);
    }    
    pinMode(lm335, INPUT);
    pinMode(button, INPUT_PULLUP);

    byte numDigits = 3;
    byte digitPins[] = { 4, 5, 6};
    byte segmentPins[] = {7, 8, 9, 10, 11, 12, 13};
    bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
    byte hardwareConfig = COMMON_ANODE; // See README.md for options
    bool updateWithDelays = false; // Default. Recommended
    bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
    bool disableDecPoint = true; // Use 'true' if your decimal point doesn't exist or isn't connected. Then, you only need to specify 7 segmentPins[]    
    sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros, disableDecPoint);
    sevseg.setBrightness(90);
}

void loop() {
    
    if (millis() - timer >= 60000) {
        timer = millis(); 
        int rawvoltage= analogRead(lm335);
        float millivolts= (rawvoltage/1024.0) * 5000;
        float kelvin= (millivolts/10);
        int celsius= kelvin - 273.15;
        sevseg.setNumber(celsius);
    }

    sevseg.refreshDisplay(); // Must run repeatedly
    // zien of de knop word ingedrukt 
    buttstat = digitalRead(button);
    if(buttstat == 1){
        if(toggel == 0){
            toggel=1;
            zandloper =millis() + 300000;
            zanddelay =millis() + 250;
            digitalWrite(zandLijn[0], LOW);
            digitalWrite(zandLijn[1], LOW);
            digitalWrite(zandLijn[2], LOW);
            digitalWrite(zandLijn[3], LOW);

        }
        while(buttstat==1){
            buttstat= digitalRead(button);
        }
    }
    // dit word uitgevoerd voor 15 min na het drukken van de knop 
    if(toggel == 1 ){
        if(zanddelay >= millis() )
        {
            if (licht < 3){
                digitalWrite(zand[licht], LOW);
                licht++; 
                digitalWrite(zand[licht], HIGH);
                zanddelay = millis() + 250;

            }
            else {
                licht =0; 
            }
        }
        if( zandloper - millis() < 75000 ){
            digitalWrite(zandLijn[0],HIGH);
            if(zandloper - millis()<150000){
                digitalWrite(zandLijn[1], HIGH);
                if(zandloper - millis()< 225000){
                    digitalWrite(zandLijn[2],HIGH);
                    if(zandloper-millis() < 290000){
                        digitalWrite(zandLijn[3],  HIGH);
                    }
                }

            }
        }
        // quand les 15 minutes sont passé on arête la sablier
        if(zandloper <= millis() ) {
            toggel =0; 
        }
    }
    else
    {
    }
}
void looplicht(){

}
/// END ///
