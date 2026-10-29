/*
  PID Speed Controller
  Arduino UNO R4 + N20 Encoder Motor

  Right Motor:
    ENB = D5
    IN3 = D7
    IN4 = D6

  Encoder:
    C1 = D2
    C2 = D3
*/

const int ENB = 5;
const int IN3 = 7;
const int IN4 = 6;

const int C1 = 2;
const int C2 = 3;

volatile long encoderCount = 0;
const int pulsesPerRevolution = 1050;

// ---------- CONTROLLER ----------
const float targetSpeed = 50.0;

const float Kp = 0.5;
const float Ki = 2.5;
const float Kd = 0.001;

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

  // Fixed forward direction
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
    "Time\tTargetRPM\tActualRPM\tError\tP\tI\tD\tPWM"
  );

  startTime = millis();
  lastSampleTime = startTime;
}

void loop() {
  unsigned long now = millis();
  unsigned long elapsed = now - startTime;

  // Stop motor when the test ends
  if (elapsed >= testDuration) {
    analogWrite(ENB, 0);

    while (true) {
      // Motor remains stopped
    }
  }

  if (now - lastSampleTime >= sampleTime) {
    float dt = (now - lastSampleTime) / 1000.0;
    lastSampleTime = now;

    // Read and reset encoder count safely
    noInterrupts();
    long pulses = encoderCount;
    encoderCount = 0;
    interrupts();

    // Calculate RPM; dt is in seconds
    float actualRPM =
      (pulses * 60.0) /
      (pulsesPerRevolution * dt);

    // Step target: 0 RPM, then 50 RPM
    float targetRPM =
      (elapsed < stepTime) ? 0.0 : targetSpeed;

    float error = targetRPM - actualRPM;

    // Proportional contribution
    float P = Kp * error;

    // Integral contribution
    integralOutput += Ki * error * dt;

    // Basic integral limit
    integralOutput = constrain(
      integralOutput, 0.0, 255.0
    );

    // Derivative contribution
    float D =
      Kd * (error - previousError) / dt;

    previousError = error;

    // Combined PID output
    float output = P + integralOutput + D;

    // Forward-only PWM
    int pwm = (int)constrain(
      output, 0.0, 255.0
    );

    analogWrite(ENB, pwm);

    // Serial Plotter data
    Serial.print(elapsed / 1000.0, 2);
    Serial.print('\t');
    Serial.print(targetRPM);
    Serial.print('\t');
    Serial.print(actualRPM);
    Serial.print('\t');
    Serial.print(error);
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