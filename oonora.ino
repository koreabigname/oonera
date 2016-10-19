#define PIN_RIGHT_MOTOR_IN1   4
#define PIN_RIGHT_MOTOR_IN2   5
#define PIN_RIGHT_MOTOR_VREF  6
#define PIN_LEFT_MOTOR_IN1    7
#define PIN_LEFT_MOTOR_IN2    8
#define PIN_LEFT_MOTOR_VREF   9
#define PIN_RIGHT_LED_RED     10
#define PIN_RIGHT_LED_YELLOW  11
#define PIN_LEFT_LED_RED      12
#define PIN_LEFT_LED_YELLOW   13
#define BUFFER_MAX            2

char serialBuffer[BUFFER_MAX];    // Serial Data Buffer
int index = 0;                    // Index of Serial Data Buffer
char incomingSerialData;          // 1Byte Serial Data
char driveDirection;              // Motor drive direction

void setup() {
  Serial.begin(19200);
  pinMode(PIN_RIGHT_MOTOR_IN1, OUTPUT);
  pinMode(PIN_RIGHT_MOTOR_IN2, OUTPUT);
  pinMode(PIN_LEFT_MOTOR_IN1, OUTPUT);
  pinMode(PIN_LEFT_MOTOR_IN2, OUTPUT);
  pinMode(PIN_RIGHT_LED_RED, OUTPUT);
  pinMode(PIN_RIGHT_LED_YELLOW, OUTPUT);
  pinMode(PIN_LEFT_LED_RED, OUTPUT);
  pinMode(PIN_LEFT_LED_YELLOW, OUTPUT);
  digitalWrite(PIN_LEFT_LED_RED, LOW);
  digitalWrite(PIN_LEFT_LED_YELLOW, LOW);
  digitalWrite(PIN_RIGHT_LED_RED, LOW);
  digitalWrite(PIN_RIGHT_LED_YELLOW, LOW);
}

void loop() {
  if (Serial.available() > 0) {
    incomingSerialData = Serial.read();
    switch(incomingSerialData) {
      case 'J':  // Start serial data (Left motor)
        driveDirection = 'L';
        break;
      case 'L':  // Start serial data (Right motor)
        driveDirection = 'R';
        break;
      default:
        if (isHexadecimalDigit(incomingSerialData)) {
          serialBuffer[index] = incomingSerialData;
          index = index + 1;
          if (index >= BUFFER_MAX) {
            sendPulseToMotor();
            index = 0;            
            driveDirection = 'W';  // Wait
          }
        }
        break;
    }
  }  

  if (Serial.available() < 1) {
    digitalWrite(PIN_LEFT_LED_RED, LOW);
    digitalWrite(PIN_LEFT_LED_YELLOW, LOW);
    digitalWrite(PIN_RIGHT_LED_RED, LOW);
    digitalWrite(PIN_RIGHT_LED_YELLOW, LOW);
  }
}

void sendPulseToMotor() {
  switch(driveDirection) {
    case 'L':  // Left motor
      outputPulseToMotor(PIN_LEFT_MOTOR_IN1, PIN_LEFT_MOTOR_IN2, PIN_LEFT_MOTOR_VREF, PIN_LEFT_LED_RED, PIN_LEFT_LED_YELLOW);
      break;
    case 'R':  // Right motor
      outputPulseToMotor(PIN_RIGHT_MOTOR_IN1, PIN_RIGHT_MOTOR_IN2, PIN_RIGHT_MOTOR_VREF, PIN_RIGHT_LED_RED, PIN_RIGHT_LED_YELLOW);
      break;
  }
}

int hexToDec(String hexString) {
  int decValue = 0;
  int nextInt;  
  for (int i = 0; i < BUFFER_MAX; i++) { 
    nextInt = int(hexString.charAt(i));
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    nextInt = constrain(nextInt, 0, 15);    
    decValue = (decValue * 16) + nextInt;
  }  
  return decValue;
}

int outputPulseToMotor( int pin_in1, int pin_in2, int pin_vref, int pin_led_red, int pin_led_yellow ) {
  int mapOutputValue  = hexToDec(serialBuffer);
  switch(getDirection(mapOutputValue)) {
    case -1:  // Backward
      digitalWrite(pin_led_red, HIGH);
      digitalWrite(pin_led_yellow, HIGH);
      digitalWrite(pin_in1, HIGH);
      digitalWrite(pin_in2, LOW);
      analogWrite(pin_vref, 255 - map(mapOutputValue, 0, 99, 80, 255) + 80);
      break;
    case 0:  // Stop
      digitalWrite(pin_led_yellow, HIGH);
      digitalWrite(pin_in1, LOW);
      digitalWrite(pin_in2, LOW);
      break;
    case 1:  // Forward
      digitalWrite(pin_led_red, HIGH);
      digitalWrite(pin_in1, LOW);
      digitalWrite(pin_in2, HIGH);
      analogWrite(pin_vref, map(mapOutputValue, 150, 255, 80, 255));
      break;
  }
  
  if (Serial.available() < 1) {
    delay(100);
    digitalWrite(pin_led_red, LOW);
    digitalWrite(pin_led_yellow, LOW);
  }  
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

