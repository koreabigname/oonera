#define PIN_RIGHT_MOTOR_IN1   4
#define PIN_RIGHT_MOTOR_IN2   5
#define PIN_RIGHT_MOTOR_VREF  6
#define PIN_LEFT_MOTOR_IN1    7
#define PIN_LEFT_MOTOR_IN2    8
#define PIN_LEFT_MOTOR_VREF   9

char inputSerialBuffer[10];
char incomingSerialData;
int mapOutputValue = 0;

void setup() {
  Serial.begin(19200);
  pinMode(PIN_RIGHT_MOTOR_IN1, OUTPUT);
  pinMode(PIN_RIGHT_MOTOR_IN2, OUTPUT);
  pinMode(PIN_LEFT_MOTOR_IN1, OUTPUT);
  pinMode(PIN_LEFT_MOTOR_IN2, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    incomingSerialData = Serial.read();
    switch(incomingSerialData) {
      case 'L':  // Left motor
        outputPulseToMotor(PIN_LEFT_MOTOR_IN1, PIN_LEFT_MOTOR_IN2, PIN_LEFT_MOTOR_VREF);
        break;
      case 'R':  // Right motor
        outputPulseToMotor(PIN_RIGHT_MOTOR_IN1, PIN_RIGHT_MOTOR_IN2, PIN_RIGHT_MOTOR_VREF);
        break;
    }
  }
}

int outputPulseToMotor( int pin_in1, int pin_in2, int pin_vref ) {
  Serial.readBytes(inputSerialBuffer, 5);
  mapOutputValue  = atoi(inputSerialBuffer) - 10000;  
  switch(getDirection(mapOutputValue)) {
    case -1:  // Backward
      digitalWrite(pin_in1, HIGH);
      digitalWrite(pin_in2, LOW);
      analogWrite(pin_vref, 255 - map(mapOutputValue, 0, 99, 80, 255) + 80);
      break;
    case 0:  // Stop
      digitalWrite(pin_in1, LOW);
      digitalWrite(pin_in2, LOW);
      break;
    case 1:  // Forward
      digitalWrite(pin_in1, LOW);
      digitalWrite(pin_in2, HIGH);
      analogWrite(pin_vref, map(mapOutputValue, 150, 255, 80, 255));
      break;
  }
  Serial.write("Ack");
}

int getDirection( int inputValue ) {
  if (inputValue >= 100 && inputValue < 150) {
    return 0;  // Stop
  } else if (inputValue < 100) {
    return -1;  // Backward
  } else {
    return 1;  // Forward
  }
}

