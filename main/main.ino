/* State:
0 -> Master State, used for directing flow of tasks
    logging in and loggin out should be involved
1 -> check password, if verified, match the input to codes and execute, else print retry and state = 0
2 -> task identification
3 -> task execution
4 -> reset and exit
*/

/* Task: 
0 -> Change Password
1 -> Enroll Fingerprint
2 -> Delete Fingerprint
3 -> Reset current process
*/
#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>

// variables
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);

uint8_t nextState = 0;
uint8_t verified = 0;

#define numCodes  5
String input;
uint8_t numInput;
String password = "espTest";
const String codes[numCodes] = {"login", "logout", "chgPass", "enroll", "delete"}; // Task indexes are important, they are important for switch.

// Functions
uint8_t validateTask() {
  for (int i = 0; i < numCodes; i++){
    if (input == codes[i]) {
      return 1;
    }
  }
  return 0;
}

void printTasks() {
  for (int i = 0; i < numCodes-1; i++) {
    Serial.print(codes[i] + ", ");
  }
  Serial.println(codes[numCodes]);
}

void waitAndGetInput(uint8_t number) {
  while (1) {
    if (Serial.available()){
      if (!number){
      input = Serial.readString();
      }
      else{
      numInput = Serial.parseInt();
      }
      break;
    }
  }
}

void setupFingerprintSensor() {
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
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the enroll task");
  }
  else {
    Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }
}

int enrollFingerprint(){ // returns p (status/error code)
  Serial.println("Ready to enroll a fingerprint.");
  Serial.print("Please type in the ID # (from ");Serial.print(finger.templateCount+1);Serial.println(" to 127) you want to save this finger as...");
  waitAndGetInput(1);
  uint8_t id = numInput;
  if ((id < finger.templateCount+1) | (id > 127)) {
    Serial.println("ID not allowed, try again.");
    return -1;
  }
  Serial.print("Enrolling ID #");
  Serial.println(id);

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
      Serial.println("Unknown error");
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
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
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
      Serial.println("Unknown error");
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
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.print("Creating model for #");  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  return true;
}



// Setup
void setup() {
  Serial.begin(115200);
  Serial.println("Initializing");
  while (!Serial) {
    ;  
  }
  setupFingerprintSensor();
}

// Loop
void loop() {
  delay(500);
  switch (nextState) {
    case 0:
      Serial.print("Enter Task ");
      Serial.print("valid Tasks are: ");
      printTasks();
      waitAndGetInput(0);
      if (input == "login") {
        nextState = 1; // verification
      }
      else if (input == "logout") {
        nextState = 0;
        verified = 0; 
        Serial.println("You have been logged out");
      }
      else {
         nextState = 2; //check for verification and allocate taskIdx
      }
  
      break;
    case 1:
      if (!verified){
        Serial.println("Enter Password or type exit:");
        waitAndGetInput(0);
        if (input == password) {
          verified = 1;
          Serial.println("Login Succesfull");
          nextState = 0;
        }
        else if (input == "exit") {
          Serial.println("Exitting..");
          nextState = 0;
        }
        else {
          Serial.println("Wrong password, retry");
          nextState = 1;
        }
      } 
      else{
        Serial.println("Already logged in");
      } 
      break;

    case 2:
      if (verified) {
        if (validateTask()) { // returns 1 if task is found
          nextState = 3; // execute tasks
        }
        else {
          Serial.println("Task was not found");
          nextState = 0;
        }
      }
      else {
        Serial.println("You are not verified, login first");
      }
      break;

    case 3:
      if (input == "enroll"){
        enrollFingerprint();
      }
      else if (input == "chgPass"){
        Serial.println("placeholder 3");
        // changePassword();
      }
      else if (input == "delete"){
        Serial.println("placeholder 3");
        // deleteFingerprint();
      }
      else {
        Serial.println("Error 3");
      }
      nextState = 0;
      break;

    // case 4:
    //   Serial.println("new task or exit?");
    //   waitAndGetInput();
    //   if (input == "exit") {
    //     Serial.println("placeholder 4");
    //     // resetVariables();
    //   }
    //   nextState = 0;
    //   break;

    default: 
      Serial.println("Default State: something went wrong");
      break;     
  }
}






