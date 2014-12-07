/*
NES to Keyboard Controller
By Nathan Wittstock <nate@milkandtang.com>
Adapted from a sketch by Sebastian Tomczak
2014-12-06
MIT Licensed
*/

int NUM_BUTTONS = 8;
int NUM_CONTROLLERS = 2;
int last_controller_data = 0;
int controller_data = 0;

int CLOCK = 0;
int LATCH = 1;
int DATA = 2;

int PINS[2][3] = {
  /* Controller 1 */
  22, // CLOCK
  24, // LATCH
  26, // DATA
  /* Controller 2 */
  23, // CLOCK
  25, // LATCH
  27, // DATA
};

/* Controller Keys */
byte KEYS[] = {
  /* Controller 1, NES only */
  0xD7, // Right, KEY_RIGHT_ARROW
  0xD8, // Left, KEY_LEFT_ARROW
  0xD9, // Down, KEY_DOWN_ARROW
  0xDA, // Up, KEY_UP_ARROW
  0x5D, // Start, ']'
  0x5B, // Select, '['
  0x7A, // B, 'z'
  0x78, // A, 'x'
  /* Controller 2, NES only */
  0x66, // Right, Numpad 6
  0x64, // Left, Numpad 4
  0x62, // Down, Numpad 2
  0x68, // Up, Numpad 8
  0x69, // start, Numpad 9
  0x67, // Select, Numpad 7
  0x6B, // B, Numpad *
  0x65, // A, Numpad 5
};

/* SETUP */
void setup() {
  Serial.begin(57600);
  Keyboard.begin();
  
  for (int i = 0; i < NUM_CONTROLLERS; i++) {
    pinMode(PINS[i][LATCH], OUTPUT);
    pinMode(PINS[i][CLOCK], OUTPUT);
    pinMode(PINS[i][DATA], INPUT);

    digitalWrite(PINS[i][LATCH], HIGH);
    digitalWrite(PINS[i][CLOCK], HIGH);
  }
}

void controllerRead(int c) {
  digitalWrite(PINS[c][LATCH], LOW);
  digitalWrite(PINS[c][CLOCK], LOW);

  /* Latch current controller state */
  digitalWrite(PINS[c][LATCH], HIGH);
  delayMicroseconds(2);
  digitalWrite(PINS[c][LATCH], LOW);

  for (int i = 0; i < NUM_BUTTONS; i++) {
    controller_data = controller_data << 1;
    /* We invert reads, since NES sends low for a button press. */
    controller_data = controller_data + !digitalRead(PINS[c][DATA]);
    
    /* Instruct controller to shift off the next bit */
    delayMicroseconds(2);
    digitalWrite(PINS[c][CLOCK], HIGH);
    delayMicroseconds(4);
    digitalWrite(PINS[c][CLOCK], LOW);
  }
}

void loop() {
  controller_data = 0;
  
  for (int i = NUM_CONTROLLERS - 1; i >= 0; i--) {
    controllerRead(i);
  }
  
  if (controller_data != last_controller_data) {
    /* Compare each individual key */
    for (int i = 0; i < NUM_BUTTONS * NUM_CONTROLLERS; i++) {
      /* Isolate just the current key's bit */
      int controller_value = controller_data & (1 << i);
      int last_value = last_controller_data & (1 << i);
      
      /* if it's changed, we need to update the keyboard emulator */
      if (controller_value != last_value) {
        if (controller_value) {
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
