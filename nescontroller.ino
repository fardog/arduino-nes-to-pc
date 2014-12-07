/*
NES to Keyboard Controller
By Nathan Wittstock <nate@milkandtang.com>
Adapted from a sketch by Sebastian Tomczak
2014-12-06
MIT Licensed
*/

int CLOCK = 22; // set the CLOCK pin
int LATCH = 24; // set the LATCH pin
int DATA = 26;// set the data in pin
byte last_controller_data = 0;
byte controller_data = 0;

/* Controller Keys */
byte KEYS[] = {
  /* Controller 1 */
  0xD7, // Right, KEY_RIGHT_ARROW
  0xD8, // Left, KEY_LEFT_ARROW
  0xD9, // Down, KEY_DOWN_ARROW
  0xDA, // Up, KEY_UP_ARROW
  0x5D, // Start, ']'
  0x5B, // Select, '['
  0x7A, // B, 'z'
  0x78 // A, 'x'
};

/* SETUP */
void setup() {
  Serial.begin(57600);
  Keyboard.begin();
  
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, INPUT);

  digitalWrite(LATCH, HIGH);
  digitalWrite(CLOCK, HIGH);
}

void controllerRead() {
  controller_data = 0;
  digitalWrite(LATCH, LOW);
  digitalWrite(CLOCK, LOW);

  digitalWrite(LATCH, HIGH);
  delayMicroseconds(2);
  digitalWrite(LATCH, LOW);

  controller_data = digitalRead(DATA);

  for (int i = 1; i < 8; i++) {
    digitalWrite(CLOCK, HIGH);
    delayMicroseconds(2);
    controller_data = controller_data << 1;
    controller_data = controller_data + digitalRead(DATA) ;
    delayMicroseconds(4);
    digitalWrite(CLOCK, LOW);
  }
}

void loop() {
  controllerRead();
  if (controller_data != last_controller_data) {
    // update our keys
    for (int i = 0; i < 8; i++) {
      byte controller_value = controller_data & (1 << i);
      byte last_value = last_controller_data & (1 << i);
      
      if (controller_value != last_value) {
        /* Bits are inverted, so a falsey value means we should press */
        if (!controller_value) {
          Keyboard.press(KEYS[i]);
        }
        else {
          Keyboard.release(KEYS[i]);
        }
      }
    }
  }
  last_controller_data = controller_data;
  delay(10);
}
