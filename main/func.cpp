#include "func.h"

void enrollUser(user users[], uint8_t userExists[], Adafruit_Fingerprint finger) {
  String input;
  uint8_t numInput;

  uint8_t numUsers = sizeof(userExists)/sizeof(userExists[0]);
  Serial.print("Select user ID > 0 and < "); Serial.print(String(numUsers)); Serial.println("");

  Serial.print("Used IDs: ");
  for (uint8_t i = 1; i < numUsers; i++) {
    if (userExists[i] == 1) Serial.print(i);
    Serial.print(" ");
  }
  Serial.print("\n");

  uint8_t ID;
  while(1){
    waitAndGetInput(1, &input, &numInput);
    if (userExists[numInput] | (numInput <= 0) | (numInput >= numUsers)) Serial.println("Invalid ID, try another ID");
    else {
      Serial.println("ID valid");
      ID = numInput;
      break;
    }
  }
  Serial.println("Enter Alias");
  waitAndGetInput(0, &input, &numInput);
  users[ID].alias = input;

  String temp;
  while(1){
    Serial.println("Enter password");
    waitAndGetInput(0, &input, &numInput);
    temp = input;
    Serial.println("Enter password again");
    waitAndGetInput(0, &input, &numInput);
    if (temp == input){
      Serial.println("Password registered");
      break;
    }
    else Serial.println("Passwords dont match, try again");
  }
  users[ID].password = input;

  int p;
  while(1) {
    p = enrollFingerprint(finger, ID);
    if (p != FINGERPRINT_OK) {
      Serial.println("Error, retry or exit?");
      waitAndGetInput(0, &input, &ID);
      if (input == "exit"){
        users[ID].alias = "Null";
        users[ID].password = "Null";
        break;
      }
    }
    else {
      Serial.println("Fingerprint enrolled!");
      break;
    }
  }  
}

uint8_t validateTask(String input, int numCodes, String codes[]) {
  for (int i = 0; i < numCodes; i++){
    if (input == codes[i]) {
      return 1;
    }
  }
  return 0;
}

void printTasks(String codes[], int numCodes) {
  for (int i = 0; i < numCodes-1; i++) {
    Serial.print(codes[i] + ", ");
  }
  Serial.println(codes[numCodes-1]);
}

void waitAndGetInput(uint8_t number, String *input, uint8_t *numInput) {
  while (1) {
    if (Serial.available()){
      if (!number){
      *input = Serial.readString();
      }
      else{
      *numInput = Serial.parseInt();
      }
      break;
    }
  }
}

void setupFingerprintSensor(Adafruit_Fingerprint finger) {
  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.println("Sensor doesn't contain any fingerprint data. Please run the enroll task");
  }
  else {
    Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }
}

int enrollFingerprint(Adafruit_Fingerprint finger, uint8_t id){ // returns p (status/error code)

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
      break;
    }
  }

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      break;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      break;
    default:
      Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
      break;
  }

  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
      break;
    }
  }

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      break;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      break;
    default:
      Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
      break;
  }

  Serial.print("Creating model for #");  Serial.println(id);

  p = finger.createModel();
  switch(p){
    case FINGERPRINT_OK:
      Serial.println("Prints matched!");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_ENROLLMISMATCH:
      Serial.println("Fingerprints did not match");
      break;
    default:
      Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
      break;
  }

  Serial.print("Storing model for #");  Serial.println(id);
  p = finger.storeModel(id);
  switch(p){
    case FINGERPRINT_OK:
      Serial.println("Stored!");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_BADLOCATION:
      Serial.println("Could not store in that location");
      break;
    case FINGERPRINT_FLASHERR:
      Serial.println("Error writing to flash");
      break;
    default:
      Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
      break;
  }
  return p;
}

int deleteFingerprint(Adafruit_Fingerprint finger){
  uint8_t id;
  String unused;
  Serial.println("Ready to delete a fingerprint.");
  Serial.println("Please type in the ID # (from 1 to 127) you want to delete...");
  waitAndGetInput(1, &unused, &id);
  if ((id < 1) | (id > 127)) {
    Serial.println("ID not allowed, try again.");
    return -1;
  }
  Serial.print("Deleting fingerprint at ID #");
  Serial.println(id);

  int p = -1;

  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
  }
  return p;
}