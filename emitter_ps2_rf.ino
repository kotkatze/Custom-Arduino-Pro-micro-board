/*
      Emitter NRF24L01 and PS2 controller

      Components:
        - Arduino Uno;
        - NRF24L01;

      Pinout:
      RF      UNO
      CE      9
      CSN     10
      SCK     13
      MISO    12
      MOSI    11




*/


#include <PS2X_lib.h>  //for v1.6
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


// PS2 pins

#define PS2_DAT        4 // DI    
#define PS2_CMD        3 // DO
#define PS2_SEL        2 // CS
#define PS2_CLK        5 // CLK


struct joystick {
  byte potLX;
  byte potLY;
  byte potRX;
  byte potRY;
  byte buttons1 = 0b00000000;
  byte buttons2 = 0b00000000;
};

joystick joystickData;

RF24 radio(9, 10); // 9 CE, CSN 10;
uint64_t address[6] = {0x7878787878LL,
                       0xB3B4B5B6F1LL,
                       0xB3B4B5B6CDLL,
                       0xB3B4B5B6A3LL,
                       0xB3B4B5B60FLL,
                       0xB3B4B5B605LL
                      };

#define pressures   false
#define rumble      false

PS2X ps2x; // Create a variable to receive data from the control

int error = 0;
byte type = 0;
byte vibrate = 0;

void setup() {

  Serial.begin(57600);

  delay(300);  //Added delay to give wireless ps2 module some time to startup, before configuring it

  //Setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);

  // Inicia Controle
  if (error == 0) {
    Serial.print("Found Controller, configured successful ");
    Serial.print("pressures = ");
    if (pressures)
      Serial.println("true ");
    else
      Serial.println("false");
    Serial.print("rumble = ");
    if (rumble)
      Serial.println("true)");
    else
      Serial.println("false");
    Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
    Serial.println("holding L1 or R1 will print out the analog stick values.");
    Serial.println("Note: Go to www.billporter.info for updates and to report bugs.");
  }
  else if (error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");

  else if (error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

  else if (error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");

  // Inform control type
  type = ps2x.readType();
  switch (type) {
    case 0:
      Serial.print("Unknown Controller type found ");
      break;
    case 1:
      Serial.print("DualShock Controller found ");
      break;
    case 2:
      Serial.print("GuitarHero Controller found ");
      break;
    case 3:
      Serial.print("Wireless Sony DualShock Controller found ");
      break;
  }

  // Initialize Radio
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
  radio.openWritingPipe(address[0]);

}

void loop() {

  ps2x.read_gamepad(false, vibrate); // Read data from the control

  // Check each button and save it in the variable to be sent via RF
  if (ps2x.Button(PSB_START))   {
    bitWrite(joystickData.buttons1, 0, 1);
  } else {
    bitWrite(joystickData.buttons1, 0, 0);
  }

  if (ps2x.Button(PSB_SELECT)) {
    bitWrite(joystickData.buttons1, 1, 1);
  } else {
    bitWrite(joystickData.buttons1, 1, 0);
  }

  if (ps2x.Button(PSB_PAD_UP)) {
    bitWrite(joystickData.buttons1, 2, 1);
  } else {
    bitWrite(joystickData.buttons1, 2, 0);
  }

  if (ps2x.Button(PSB_L1)) {
    bitWrite(joystickData.buttons2, 0, 1);
  } else {
    bitWrite(joystickData.buttons2, 0, 0);
  }

  if (ps2x.Button(PSB_R1)) {
    bitWrite(joystickData.buttons2, 1, 1);
  } else {
    bitWrite(joystickData.buttons2, 1, 0);
  }

  if (ps2x.Button(PSB_PAD_RIGHT)) {
    bitWrite(joystickData.buttons1, 3, 1);
  } else {
    bitWrite(joystickData.buttons1, 3, 0);
  }

  if (ps2x.Button(PSB_PAD_LEFT)) {
    bitWrite(joystickData.buttons1, 4, 1);
  } else {
    bitWrite(joystickData.buttons1, 4, 0);
  }

  if (ps2x.Button(PSB_PAD_DOWN)) {
    bitWrite(joystickData.buttons1, 5, 1);
  } else {
    bitWrite(joystickData.buttons1, 5, 0);
  }

  if (ps2x.Button(PSB_L1)) {
    bitWrite(joystickData.buttons1, 6, 1);
  } else {
    bitWrite(joystickData.buttons1, 6, 0);
  }

  if (ps2x.Button(PSB_R1)) {
    bitWrite(joystickData.buttons1, 7, 1);
  } else {
    bitWrite(joystickData.buttons1, 7, 0);
  }

  if (ps2x.Button(PSB_L2)) {
    bitWrite(joystickData.buttons2, 0, 1);
  } else {
    bitWrite(joystickData.buttons2, 0, 0);
  }

  if (ps2x.Button(PSB_R2)) {
    bitWrite(joystickData.buttons2, 1, 1);
  } else {
    bitWrite(joystickData.buttons2, 1, 0);
  }

  if (ps2x.Button(PSB_L3)) {
    bitWrite(joystickData.buttons2, 2, 1);
  } else {
    bitWrite(joystickData.buttons2, 2, 0);
  }

  if (ps2x.Button(PSB_R3)) {
    bitWrite(joystickData.buttons2, 3, 1);
  } else {
    bitWrite(joystickData.buttons2, 3, 0);
  }

  if (ps2x.Button(PSB_TRIANGLE)) {
    bitWrite(joystickData.buttons2, 4, 1);
  } else {
    bitWrite(joystickData.buttons2, 4, 0);
  }

  if (ps2x.Button(PSB_CIRCLE)) {
    bitWrite(joystickData.buttons2, 5, 1);
  } else {
    bitWrite(joystickData.buttons2, 5, 0);
  }

  if (ps2x.Button(PSB_CROSS))  {
    bitWrite(joystickData.buttons2, 6, 1);
  } else {
    bitWrite(joystickData.buttons2, 6, 0);
  }

  if (ps2x.Button(PSB_SQUARE))  {
    bitWrite(joystickData.buttons2, 7, 1);
  } else {
    bitWrite(joystickData.buttons2, 7, 0);
  }

  // Receive data from the analog inputs
  joystickData.potLY = ps2x.Analog(PSS_LY);
  joystickData.potLX = ps2x.Analog(PSS_LX);
  joystickData.potRY = ps2x.Analog(PSS_RY);
  joystickData.potRX = ps2x.Analog(PSS_RX);


  // Send data via RF
  radio.write(&joystickData, sizeof(joystick));
  delay(50);

}
