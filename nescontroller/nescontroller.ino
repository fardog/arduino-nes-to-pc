/*
NES to Joystick Controller
By Nathan Wittstock <nate@milkandtang.com>
Adapted from a sketch by Sebastian Tomczak
2014-12-06
MIT Licensed
*/

#include "DigiJoystick.h"

int NUM_BUTTONS = 16;
int last_controller_data = 0;
int controller_data = 0;
int controller_index = 0;

int CLOCK = 0;
int LATCH = 1;
int DATA = 2;

int PINS[] = {
  /* NES Controller */
  0, // CLOCK
  1, // LATCH
  2, // DATA1
  /* SNES Controller */
  3, // DATA2
};

/* SETUP */
void setup() {
  pinMode(PINS[LATCH], OUTPUT);
  pinMode(PINS[CLOCK], OUTPUT);
  
  for (int i = 0; i <= controller_index; i++) {
    pinMode(PINS[DATA + i], INPUT);
  }

  digitalWrite(PINS[LATCH], HIGH);
  digitalWrite(PINS[CLOCK], HIGH);
  
  DigiJoystick.setX((byte) 0x7F);
  DigiJoystick.setY((byte) 0x7F);
}

void controllerRead() {
  digitalWrite(PINS[LATCH], LOW);
  digitalWrite(PINS[CLOCK], LOW);

  /* Latch current controller state */
  digitalWrite(PINS[LATCH], HIGH);
  delayMicroseconds(2);
  digitalWrite(PINS[LATCH], LOW);

  for (int i = 0; i < NUM_BUTTONS; i++) {
    controller_data = controller_data << 1;
    /* We invert reads, since NES sends low for a button press. */
    controller_data = controller_data + !digitalRead(PINS[DATA + controller_index]);
    
    /* Instruct controller to shift off the next bit */
    delayMicroseconds(2);
    digitalWrite(PINS[CLOCK], HIGH);
    delayMicroseconds(4);
    digitalWrite(PINS[CLOCK], LOW);
  }
}

void loop() {
  /* Blank our controller data, and read it */
  controller_data = 0;
  controllerRead();
  
  /* If we see different data than our last go-round, we need to update
     some buttons */
  if (controller_data != last_controller_data) {
    bool dirs[4] = {false, false, false, false};
    byte joystick = controller_data >> 8;
    
    /* Get the directions into a boolean array so we can address them 
       more easily. */
    for (int i = 0; i < 4; i++) {
      int controller_value = joystick & (1 << i);
      if (controller_value) dirs[i] = true;
    }
    
    /* Either set the joystick fully left, right, or center if nothing
       is pressed. */
    if (dirs[0]) DigiJoystick.setX((byte) 0xFF);
    else if (dirs[1]) DigiJoystick.setX((byte) 0x00);
    else if (!dirs[0] && !dirs[1]) DigiJoystick.setX((byte) 0x7F);
    
    if (dirs[2]) DigiJoystick.setY((byte) 0xFF);
    else if (dirs[3]) DigiJoystick.setY((byte) 0x00);
    else if (!dirs[2] && !dirs[3]) DigiJoystick.setY((byte) 0x7F);
    
    /* Take the bytes we need, and get them into the 4 high-order bits */
    byte button_value_high = controller_data >> 8;
    /* Then the remaining buttons, and put them into the 4 low-order bits */
    byte button_value_low = controller_data >> 4;
    for (int i = 0; i < 4; i++) {
      /* Zero out the bits we don't care about (directional controls) */
      button_value_high &= ~(1 << i);
      button_value_low &= ~(1 << (i + 4));
    }
    /* OR the low and high order bytes together to create a single set of buttons */
    DigiJoystick.setButtons(button_value_high | button_value_low, (byte) 0x00);
  }
  
  /* Remember this controller state, and delay the next go-round */
  last_controller_data = controller_data;
  DigiJoystick.delay(10);
}
