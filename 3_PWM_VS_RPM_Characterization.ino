// -------- RIGHT MOTOR DRIVER --------
const int ENB = 10;
const int IN3 = 9;
const int IN4 = 8;

// -------- RIGHT ENCODER --------
const int C1 = 12;
const int C2 = 13;

// -------- MOTOR --------
const int PPR = 1050;

// -------- VARIABLES --------
volatile long pulseCount = 0;

unsigned long sampleTimer = 0;
unsigned long pwmTimer = 0;

int pwm = 0;

float rpmSum = 0;
int sampleCount = 0;

void setup()
{
    Serial.begin(115200);

    pinMode(ENB, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    pinMode(C1, INPUT_PULLUP);
    pinMode(C2, INPUT_PULLUP);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);

    analogWrite(ENB, pwm);

    attachInterrupt(
        digitalPinToInterrupt(C1),
        countPulse,
        RISING
    );

    sampleTimer = millis();
    pwmTimer = millis();

    Serial.println("PWM\tAverage RPM");
}

void loop()
{
    unsigned long currentTime = millis();

    // Sample RPM every 100 ms
    if (currentTime - sampleTimer >= 100)
    {
        sampleTimer = currentTime;

        noInterrupts();
        long pulses = pulseCount;
        pulseCount = 0;
        interrupts();

        float rpm = (pulses * 600.0) / PPR;

        rpmSum += rpm;
        sampleCount++;
    }

    // Change PWM every 7 seconds
    if (currentTime - pwmTimer >= 7000)
    {
        pwmTimer = currentTime;

        float averageRPM = rpmSum / sampleCount;

        Serial.print(pwm);
        Serial.print("\t");
        Serial.println(averageRPM);

        // Reset for next PWM level
        rpmSum = 0;
        sampleCount = 0;

        pwm += 20;

        if (pwm > 255)
        {
            analogWrite(ENB, 0);

            Serial.println();
            Serial.println("Characterization Complete.");

            while (1);
        }

        analogWrite(ENB, pwm);
    }
}

void countPulse()
{
    pulseCount++;
}