#include <Arduino.h>
#include <BleGamepad.h>

#define SWITCH_1_PIN 25
#define SWITCH_2_PIN 13
#define SWITCH_3_PIN 14

#define READ_DELAY_MILLIS 100

BleGamepad bleGamepad("Sequential Shifter", "Skye Harris", 100);

class Switch {
   private:
    byte pin;
    byte button;
    byte lastState;
    uint lastReadTime;

   public:
    Switch(byte pin, byte assignedButton) {
        this->pin = pin;
        this->button = assignedButton;
        this->lastReadTime = 0;
        this->lastState = 0;

        pinMode(pin, INPUT_PULLUP);
    }

    bool checkAndReport() {
        byte switchState = 1 - digitalRead(pin);
        int now = millis();

        if (switchState != lastState && now >= lastReadTime + READ_DELAY_MILLIS) {
            Serial.printf("Button %i changed to %i\n", button, switchState);
            lastState = switchState;
            lastReadTime = now;

            if (bleGamepad.isConnected()) {
                if (switchState) {
                    bleGamepad.press(button);
                } else {
                    bleGamepad.release(button);
                }

                return true;
            }
        }

        return false;
    }
};

Switch switch1(SWITCH_1_PIN, 1);
Switch switch2(SWITCH_2_PIN, 2);
Switch switch3(SWITCH_3_PIN, 3);

void setup() {
    Serial.begin(9600);

    bleGamepad.setAutoReport(false);
    bleGamepad.setControllerType(CONTROLLER_TYPE_GAMEPAD);
    bleGamepad.begin(3, 0, false, false, false, false, false, false, false, false, false, false, false, false, false);
}

void loop() {
    bool shouldReport = false;

    shouldReport = switch1.checkAndReport() || shouldReport;
    shouldReport = switch2.checkAndReport() || shouldReport;
    shouldReport = switch3.checkAndReport() || shouldReport;

    if (shouldReport) {
        bleGamepad.sendReport();
    }
}