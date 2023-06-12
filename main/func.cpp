#include "func.h"

int getFingerID(Adafruit_Fingerprint finger) {
  int p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return -1;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return -1;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return -1;
    default:
      Serial.println("Unknown error");
      return -1;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return -1;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return -1;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return -1;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return -1;
    default:
      Serial.println("Unknown error");
      return -1;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return -1;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return -1;
  } else {
    Serial.println("Unknown error");
    return -1;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  return finger.fingerID;
}

void validateFingerAndPass(user users[], uint8_t ID, uint8_t *access, Adafruit_Fingerprint finger) {
  String input, pass;
  uint8_t numInput, exit;
  int result;

  while (1) {
    Serial.println("Enter password");
    waitAndGetInput(0, &input, &numInput, &exit);
    if (exit) return;
    pass = input;
    if (pass == users[ID].password){
      result = getFingerID(finger);
      if ((uint8_t)result == ID) {
        *access = 1;
        break;
      } 
      else *access = 0;
    }
    else Serial.println("Passwords dont match, try again");
  }
}

void discardUser(user users[], uint8_t ID) {
  users[ID].alias = "Null";
  users[ID].password = "Null";
  users[ID].userExists = 0;
}

void deleteUser(user users[], Adafruit_Fingerprint finger) {
  String input;
  uint8_t numInput, exit;
  uint8_t access = 0;

  uint8_t numUsers = sizeof(users)/sizeof(users[0]);
  Serial.print("Select user ID > 0 and < "); Serial.print(String(numUsers)); Serial.println("");

  Serial.print("Used IDs: ");
  for (uint8_t i = 1; i < numUsers; i++) {
    if (users[i].userExists) Serial.print(i); Serial.print(" ");
  }
  Serial.print("\n");

  uint8_t ID;
  while(1){
    waitAndGetInput(1, &input, &numInput, &exit);
    if (exit) return;
    if (!(users[numInput].userExists)) Serial.println("Invalid ID, try another ID");
    else {
      Serial.println("ID valid");
      ID = numInput;
      break;
    }
  }

  validateFingerAndPass(users, ID, &access, finger);
  if (access) {
    deleteFingerprint(finger, ID);
    discardUser(users, ID);
    Serial.println("User Deleted");
  }
  else {
    Serial.println("Process failed");
    return;
  }
}

void initializeUsers(user users[]) {
  uint8_t numUsers = sizeof(users)/sizeof(users[0]);
  for (uint8_t i = 1; i < numUsers; i++) {
    discardUser(users, i);
  }
}

void enrollUser(user users[], Adafruit_Fingerprint finger) {
  String input;
  uint8_t numInput, exit;

  uint8_t numUsers = sizeof(users)/sizeof(users[0]);
  Serial.print("Select user ID > 0 and < "); Serial.print(String(numUsers)); Serial.println("");

  Serial.print("Used IDs: ");
  for (uint8_t i = 1; i < numUsers; i++) {
    if (users[i].userExists) Serial.print(i);
    Serial.print(" ");
  }
  Serial.print("\n");

  uint8_t ID;
  while(1){
    waitAndGetInput(1, &input, &numInput, &exit);
    if (exit) return;
    if (users[numInput].userExists | (numInput <= 0) | (numInput >= numUsers)) Serial.println("Invalid ID, try another ID");
    else {
      Serial.println("ID valid");
      ID = numInput;
      break;
    }
  }

  Serial.println("Enter Alias");
  waitAndGetInput(0, &input, &numInput, &exit);
  if (exit) return;
  users[ID].alias = input;

  String temp;
  while(1){
    Serial.println("Enter password");
    waitAndGetInput(0, &input, &numInput, &exit);
    if (exit) {
      discardUser(users, ID);
      return;
    }
    temp = input;
    Serial.println("Enter password again");
    waitAndGetInput(0, &input, &numInput, &exit);
    if (exit) {
      discardUser(users, ID);
      return;
    }
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
      waitAndGetInput(0, &input, &numInput, &exit);
      if (exit) {
        discardUser(users, ID);
        return;
      }
    }
    else {
      Serial.println("Fingerprint enrolled");
      break;
    }
  }

  Serial.println("User enrolled");
  users[ID].userExists = 1;  
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
  for (int i = 0; i < numCodes; i++) {
    Serial.print(codes[i] + " ");
  }
  Serial.println("");
}

void waitAndGetInput(uint8_t number, String *input, uint8_t *numInput, uint8_t *exit) {
  if (!number) Serial.println("To Exit, type \"exit\"");
  else Serial.println("To Exit, type 255");
  while (1) {
    if (Serial.available()){
      if (!number){
      *input = Serial.readString();
      if (*input == "exit") *exit = 1;
      else *exit = 0;
      }
      else{
      *numInput = Serial.parseInt();
      if (*numInput == 255) *exit = 1;
      else *exit = 0;
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
      Serial.print(".");
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
      Serial.print(".");
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

int deleteFingerprint(Adafruit_Fingerprint finger, uint8_t id){

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