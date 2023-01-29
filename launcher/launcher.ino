
#define LAUNCH_IN  2    // input pin for the launch button
#define LAUNCH_LED 4    // led for launching
#define LAUNCH_LEN 1500 // how long you have to hold the launch button

#define TRIG_PIN  7  // trigger pin for the ending sensor
#define PING_PIN  6  // ping pin for the ending sensor
#define PASS_DIST 10 // distance need to trigger the end

#define SAFE_LED 3 // led for end game

#define TRACK_LEN 20 // Track lenght in meters

int launchState = 0;

unsigned long launchTime;
unsigned long endTime;
float launchDuration;

int count = 0;
int state = 0;
// 0 Pre Launch  - Waiting for button to be held
// 1 Launched    - Waiting for car to reach the end
// 2 Post Launch - Car has reached the end, waiting to be reset

long duration;
int distance;

void setup()
{
    Serial.begin(9600);
    Serial.println("Start");

    pinMode(LAUNCH_IN,  INPUT);
    pinMode(LAUNCH_LED, OUTPUT);

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(PING_PIN, INPUT);

    pinMode(SAFE_LED, OUTPUT);
    digitalWrite(SAFE_LED, LOW);
}

void loop()
{
    if (state == 0)
    {
        // Pre Launch - Waiting for button to be held
        launchState = digitalRead(LAUNCH_IN); // Read button state

        if (launchState == HIGH) // If Button is Pressed
        {
            count += 1; // Increment counter
            delay(1);

            digitalWrite(SAFE_LED, LOW);   // Turn off safe LED
            digitalWrite(LAUNCH_LED, LOW); // Turn off fire LED
        } else { // If not pressed
            count = 0; // Reset Counter

            digitalWrite(SAFE_LED, LOW);   // Turn off safe LED
            digitalWrite(LAUNCH_LED, LOW); // Turn off fire LED
        }

        if (count == LAUNCH_LEN)
        {
            state = 1; // Set state to Launched State
            count = 0; // Reset Counter

            launchTime = millis(); // Record End Time
            digitalWrite(LAUNCH_LED, HIGH); // Turn on fire LED

            Serial.println("Launch");
        }
    } else if (state == 1) {
        // Launched - Waiting for car to reach the end

        // Get the distance from HC-SR04
        digitalWrite(TRIG_PIN, LOW);
        delayMicroseconds(5);

        digitalWrite(TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG_PIN, LOW);

        duration = pulseIn(PING_PIN, HIGH);
        distance = duration * 0.034 / 2;

        // If the distance is less than the threshold and not 0
        if (distance <= PASS_DIST && distance != 0)
        {
            endTime = millis(); // Record End Time

            state = 2; // Set State to Post Launch

            Serial.println("Car Passed by");
            digitalWrite(LAUNCH_LED, LOW); // Turn off Launch LED
            digitalWrite(SAFE_LED, HIGH);  // Turn on Safe LED

            launchDuration = (endTime - launchTime) / 1000.0; // How long did it travel for in seconds

            Serial.print("Took ");
            Serial.print(duration);
            Serial.println("ms");

            Serial.print("It went and Average of ");
            Serial.print(TRACK_LEN / (launchDuration));
            Serial.println("m/s");
        }
    } else if (state == 2) {
        // Post Launch - Car has reached the end, waiting to be reset

        if (digitalRead(LAUNCH_IN) == HIGH) // If button is pressed
        {
            state = 0; // Set state to Pre Launched State
            
            digitalWrite(SAFE_LED, LOW);   // Turn off Launch LED
            digitalWrite(LAUNCH_LED, LOW); // Turn off Safe LED

            Serial.println("Reset");
            delay(500); // For safety wait .5 seconds
        }
    }
}