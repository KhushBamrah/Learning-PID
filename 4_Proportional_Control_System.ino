// ---------- MOTOR PINS ----------
const int ENA = 10;
const int IN1 = 9;
const int IN2 = 8;

// ---------- ENCODER ----------
const int encoderPin = 12;

volatile long pulseCount = 0;

// ---------- CONTROL ----------
float targetRPM = 150;

float Kp = 0.45;

int pwmValue = 0;

// ---------- MOTOR SPECS ----------
const int pulsesPerRevolution = 1050;
// CHANGE THIS according to your encoder

unsigned long previousTime = 0;

void setup()
{
    Serial.begin(9600);

    pinMode(ENA, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

    pinMode(encoderPin, INPUT_PULLUP);

    attachInterrupt(
        digitalPinToInterrupt(encoderPin),
        countPulse,
        RISING
    );

    // Motor forward
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
}

void loop()
{
    unsigned long currentTime = millis();

    // Run every 100 ms
    if (currentTime - previousTime >= 1000)
    {
        previousTime = currentTime;

        // Copy pulse count safely
        noInterrupts();
        long pulses = pulseCount;
        pulseCount = 0;
        interrupts();

        // Calculate RPM
        float rpm = (pulses * 600.0) / pulsesPerRevolution;

        // Calculate error
        float error = targetRPM - rpm;

        // P Controller
        pwmValue = pwmValue + (Kp * error);

        // Limit PWM
        pwmValue = constrain(pwmValue, 0, 255);

        // Send PWM
        analogWrite(ENA, pwmValue);

        // Debug
        Serial.print("RPM: ");
        Serial.print(rpm);

        Serial.print("   Error: ");
        Serial.print(error);

        Serial.print("   PWM: ");
        Serial.println(pwmValue);
    }
}

// Interrupt function
void countPulse()
{
    pulseCount++;
}