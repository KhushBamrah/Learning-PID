/*
  PID Speed Controller with External Tuning
  Arduino UNO R4 + N20 Encoder Motor

  Right Motor:
    ENB = D5
    IN3 = D7
    IN4 = D6

  Encoder:
    C1 = D2
    C2 = D3

  Potentiometers:
    A3 = Kp
    A4 = Ki
    A5 = Kd
*/

const int ENB = 5;
const int IN3 = 7;
const int IN4 = 6;

const int C1 = 2;
const int C2 = 3;

const int KP_POT = A3;
const int KI_POT = A4;
const int KD_POT = A5;

volatile long encoderCount = 0;
const int pulsesPerRevolution = 1050;

// ---------- TARGET ----------
const float targetSpeed = 50.0;

// ---------- GAIN RANGES ----------
// Adjust these maximum values to suit your experiments.
const float KP_MAX = 8.0;
const float KI_MAX = 8.0;
const float KD_MAX = 0.1;

// ---------- PID STATE ----------
float Kp = 1.0;
float Ki = 1.0;
float Kd = 0.02;

float integralOutput = 0.0;
float previousError = 0.0;

const unsigned long sampleTime = 100;
const unsigned long stepTime = 2000;
const unsigned long testDuration = 50000;

unsigned long startTime;
unsigned long lastSampleTime;

void countPulse() {
  encoderCount++;
}

void setup() {
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(C1, INPUT_PULLUP);
  pinMode(C2, INPUT_PULLUP);

  pinMode(KP_POT, INPUT);
  pinMode(KI_POT, INPUT);
  pinMode(KD_POT, INPUT);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 0);

  attachInterrupt(
    digitalPinToInterrupt(C1),
    countPulse,
    RISING
  );

  Serial.begin(115200);
  Serial.println(
    "Time\tTarget\tRPM\tError\tKp\tKi\tKd\tP\tI\tD\tPWM"
  );

  startTime = millis();
  lastSampleTime = startTime;
}

void loop() {
  unsigned long now = millis();
  unsigned long elapsed = now - startTime;

  // Stop after the test duration
  if (elapsed >= testDuration) {
    analogWrite(ENB, 0);

    while (true) {
      // Motor remains stopped
    }
  }

  if (now - lastSampleTime >= sampleTime) {
    float dt = (now - lastSampleTime) / 1000.0;
    lastSampleTime = now;

    // Read potentiometers and map to gain ranges
    Kp = (analogRead(KP_POT) / 1023.0) * KP_MAX;
    Ki = (analogRead(KI_POT) / 1023.0) * KI_MAX;
    Kd = (analogRead(KD_POT) / 1023.0) * KD_MAX;

    // Read and reset encoder count safely
    noInterrupts();
    long pulses = encoderCount;
    encoderCount = 0;
    interrupts();

    // RPM; dt is in seconds
    float actualRPM =
      (pulses * 60.0) /
      (pulsesPerRevolution * dt);

    // Step target: 0 RPM, then 50 RPM
    float targetRPM =
      (elapsed < stepTime) ? 0.0 : targetSpeed;

    float error = targetRPM - actualRPM;

    // PID terms
    float P = Kp * error;

    integralOutput += Ki * error * dt;
    integralOutput = constrain(
      integralOutput, 0.0, 255.0
    );

    float D =
      Kd * (error - previousError) / dt;

    previousError = error;

    float output = P + integralOutput + D;

    int pwm = (int)constrain(output, 0.0, 255.0);

    analogWrite(ENB, pwm);

    // Serial output for Serial Plotter
    Serial.print(elapsed / 1000.0, 2);
    Serial.print('\t');
    Serial.print(targetRPM);
    Serial.print('\t');
    Serial.print(actualRPM);
    Serial.print('\t');
    Serial.print(error);
    Serial.print('\t');
    Serial.print(Kp, 3);
    Serial.print('\t');
    Serial.print(Ki, 3);
    Serial.print('\t');
    Serial.print(Kd, 5);
    Serial.print('\t');
    Serial.print(P);
    Serial.print('\t');
    Serial.print(integralOutput);
    Serial.print('\t');
    Serial.print(D);
    Serial.print('\t');
    Serial.println(pwm);
  }
}