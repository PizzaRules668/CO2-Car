#include <HX711.h> // https://github.com/bogde/HX711
#include <Servo.h>

#define HX711_DT  8 // HX711 Data Pin
#define HX711_SCK 9 // HX711 Clock Pin

#define IGNITION_IN  2    // Input pin for the ignition button
#define IGNITION_LED 4    // LED for ignition
#define IGNITION_LEN 1500 // How long you have to hold the ignition button
#define IGNITION_SERVO 5  // Pin for ignition servo
#define IGNITION_DEG 30   // Angle for ignition servo to move to

#define SAFE_LED 3 // LED to say when safe

Servo ignitionServo;

int ignitionState = 0;

unsigned long ignitionTime;
unsigned long endTime;
float ignitionDuration;

float force;

int count = 0;
int state = 0;
// States
// 0 Pre Ignition  - Waiting for button to be held
// 1 Ignition      - Waiting for force to be < 0
// 2 Post Ignition - Thrust has stopped, waiting to be reset

HX711 loadcell;

void setup()
{
    Serial.begin(57600);
    Serial.println("Start");

    loadcell.begin(HX711_DT, HX711_SCK); // Init HX711 on pins
    loadcell.tare(); // Reset to 0

    ignitionServo.attach(IGNITION_SERVO);
    ignitionServo.write(0);

    calibrate();
}

void calibrate()
{
    Serial.println("Calibrating");

    loadcell.set_scale();
    loadcell.tare();

    Serial.println("Place Weight");
    Serial.print("What is the Weight of the object: ");

    while (Serial.available() == 0) {}

    float knownWeight = Serial.parseFloat();
    float weight = loadcell.get_units(10);

    float scale = weight / knownWeight;
    
    Serial.print("\nScale is ");
    Serial.println(scale);

    loadcell.set_scale(scale);
}

void loop()
{
    if (state == 0)
    {
        // Pre ignition - Waiting for button to be held
        ignitionState = digitalRead(IGNITION_IN); // Read button state

        if (ignitionState == HIGH) // If Button is Pressed
        {
            count += 1; // Increment counter
            delay(1);

            digitalWrite(SAFE_LED, LOW);   // Turn off safe LED
            digitalWrite(IGNITION_LED, LOW); // Turn off fire LED
        } else { // If not pressed
            count = 0; // Reset Counter

            digitalWrite(SAFE_LED, LOW);   // Turn off safe LED
            digitalWrite(IGNITION_LED, LOW); // Turn off fire LED
        }

        if (count == IGNITION_LEN)
        {
            state = 1; // Set state to Ignition State
            count = 0; // Reset Counter

            ignitionTime = millis(); // Record End Time
            digitalWrite(IGNITION_LED, HIGH); // Turn on fire LED
            ignitionServo.write(IGNITION_DEG); // Move servo to ignition degree

            Serial.println("IGNITION");
        }
    } else if (state == 1) {
        // 1 Ignition - Waiting for force to be < 0

        // Get Value
        force = loadcell.get_units(2);

        // Print force it
        Serial.print(force); // Print current force
        Serial.print(", "); // Print , so it can go into csv format
        Serial.print(millis() - ignitionTime); // Print Current time 
        Serial.println(", "); // Print , so it can go into csv format

        if (force < 0)
        {
            endTime = millis(); // Record End Time

            state = 2; // Set State to Post Ignition

            Serial.println("Thrust has stopped");
            digitalWrite(IGNITION_LED, LOW); // Turn off Ignition LED
            digitalWrite(SAFE_LED, HIGH);  // Turn on Safe LED

            ignitionDuration = (endTime - ignitionTime) / 1000.0; // How long did it travel for in seconds

            Serial.print("Took ");
            Serial.print(ignitionDuration);
            Serial.println("s");
        }
    } else if (state == 2) {
        // 2 Post Ignition - Thrust has stopped, waiting to be reset

        if (digitalRead(IGNITION_IN) == HIGH) // If button is pressed
        {
            state = 0; // Set state to Pre ignition State
            
            digitalWrite(SAFE_LED, LOW);   // Turn off Launch LED
            digitalWrite(IGNITION_LED, LOW); // Turn off Safe LED
            ignitionServo.write(0);        // Move servo pre ignition pos

            Serial.println("Reset");
            delay(500); // For safety wait .5 seconds
            Serial.println();
        }
    }
}
