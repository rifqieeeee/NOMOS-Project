#define FREQUENCY_HZ 40
#define INTERVAL_MS (1000 / (FREQUENCY_HZ))

void setup() {
Serial.begin(115200);
Serial.println("Started");
}

void loop() {
static unsigned long last_interval_ms = 0;
float light;
if (millis() > last_interval_ms + INTERVAL_MS) {
last_interval_ms = millis();
light = analogRead(WIO_LIGHT);
Serial.println(light);
//Serial.print('\t');

}
}
