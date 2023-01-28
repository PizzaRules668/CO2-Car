
#define LAUNCH_IN  2  // input pin for the launch button
#define LAUNCH_LED 4  // led for launching
#define LAUNCH_LEN 50 // how long you have to hold the launch button

#define TRIG_PIN  7  // trigger pin for the ending sensor
#define PING_PIN  6  // ping pin for the ending sensor
#define PASS_DIST 10 // distance need to trigger the end

#define END_LED 3 // led for end game

int launchState = 0;
int launched = 0;
int count = 0;

long duration;
int distance;

void setup()
{
    Serial.begin(9600);
    Serial.print("Start");

    pinMode(LAUNCH_IN,  INPUT);
    pinMode(LAUNCH_LED, OUTPUT);

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(PING_PIN, INPUT);

    pinMode(END_LED, OUTPUT);
    digitalWrite(END_LED, LOW);
}

void loop()
{
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(5);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    duration = pulseIn(PING_PIN, HIGH);
    distance = duration * 0.034 / 2;

    launchState = digitalRead(LAUNCH_IN);

    if (launchState == HIGH)
    {
        count += 1;
        Serial.println(count);
        digitalWrite(END_LED, LOW);
    } else {
        count = 0;
        digitalWrite(LAUNCH_LED, LOW);
    }

    if (count == LAUNCH_LEN)
    {
        launched = 1;
        count = 0;

        Serial.println("Launch");
    }

    if (launched == 1)
    {
        digitalWrite(LAUNCH_LED, HIGH);
    }

    if (distance <= PASS_DIST && distance != 0 && launched == 1)
    {
        launched = 0;

        Serial.println("Car Passed by");
        digitalWrite(END_LED, HIGH);
    }
    Serial.println(distance);

    delay(50);
}