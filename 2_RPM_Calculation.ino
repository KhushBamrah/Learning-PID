// -------- RIGHT MOTOR DRIVER --------
const int ENB = 10;    // PWM    
const int IN3 = 9;    // Direction 1
const int IN4 = 8;    // Direction 2
//Left Motor
//const int ENB = 10 
//const int IN3 = 9;    // Direction 1
//const int IN4 = 8;    // Direction 2


// -------- RIGHT ENCODER --------
const int C1 = 12;
const int C2 = 13;

// -------- MOTOR --------
const int PPR = 1050;

// -------- USER SETTINGS --------
const int PWM_VALUE = 200;      // Change this to test different speeds
const int SAMPLE_TIME = 1000;    // milliseconds

// -------- VARIABLES --------
volatile long pulseCount = 0;
unsigned long previousTime = 0;

void setup()
{
    Serial.begin(115200);

    pinMode(ENB, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    pinMode(C1, INPUT_PULLUP);
    pinMode(C2, INPUT_PULLUP);

    // Motor Forward
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);

    analogWrite(ENB, PWM_VALUE);

    // Count encoder pulses
    attachInterrupt(
        digitalPinToInterrupt(C1),
        countPulse,
        RISING
    );

    Serial.println("Time(ms)\tRPM");
}

void loop()
{
    unsigned long currentTime = millis();

    if (currentTime - previousTime >= SAMPLE_TIME)
    {
        previousTime = currentTime;

        // Safely copy pulse count
        noInterrupts();
        long pulses = pulseCount;
        pulseCount = 0;
        interrupts();

        // Calculate RPM
        float rpm = (pulses * 60000.0) / (PPR * SAMPLE_TIME);

        // Print for Serial Plotter or Serial Monitor
        Serial.print(currentTime);
        Serial.print("\t");
        Serial.println(rpm);
    }
}

void countPulse()
{
    pulseCount++;
}