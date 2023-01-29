#include <HX711.h> // https://github.com/bogde/HX711

#define HX711_DT  8 // HX711 Data Pin
#define HX711_SCK 9 // HX711 Clock Pin

HX711 loadcell;

void setup()
{
    Serial.begin(9600);
    Serial.println("Start");

    loadcell.begin(HX711_DT, HX711_SCK); // Init HX711 on pins
    loadcell.tare(); // Reset to 0
}

void calibrate()
{
    loadcell.set_scale();
    loadcell.tare();

    Serial.print("Weight: ");

    float knownWeight = Serial.parseFloat();
    float weight = loadcell.get_units(10);

    float scale = weight / knownWeight;
    loadcell.set_scale(scale);
}

void loop()
{
    // Get Value, and print it
    Serial.print(loadcell.get_units(10));
    Serial.print(", "); // Print , so it can go into csv format
}