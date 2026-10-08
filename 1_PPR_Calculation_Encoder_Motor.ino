// -------- RIGHT ENCODER --------
const int C1 = 2;
const int C2 = 3;

volatile long pulseCount = 0;

void setup()
{
    Serial.begin(115200);

    pinMode(C1, INPUT_PULLUP);
    pinMode(C2, INPUT_PULLUP);

    attachInterrupt(
        digitalPinToInterrupt(C1),
        countPulse,
        RISING
    );

    Serial.println("--------------------------------");
    Serial.println("PPR Measurement");
    Serial.println("Rotate the OUTPUT shaft exactly ONE revolution.");
    Serial.println("--------------------------------");
}

void loop()
{
    static long lastCount = -1;

    if (pulseCount != lastCount)
    {
        lastCount = pulseCount;

        Serial.print("Pulse Count : ");
        Serial.println(pulseCount);
    }
}

void countPulse()
{
    pulseCount++;
}