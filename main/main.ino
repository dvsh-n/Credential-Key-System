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

// Variables and definitions
uint8_t nextState = 0; 
uint8_t taskIdx = 0;
uint8_t verified = 0;

#define numCodes  5
String input;
const String password = "espTest";
const String codes[numCodes] = {"login", "logout", "chgPass", "enroll", "delete"}; // Task indexes are important, they are important for switch.


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);

// Functions
uint8_t getTaskIdx() {
  uint8_t taskFound = 0;
  for (int i = 0; i < numCodes; i++){
    if (input == codes[i]) {
      taskIdx = i;
      taskFound = 1;
      break;
    }
  }
  return taskFound;
}

void printTasks() {
  for (int i = 0; i < numCodes-1; i++) {
    Serial.print(codes[i] + ", ");
  }
  Serial.println(codes[numCodes]);
}

void waitAndGetInput() {
  while (1) {
    if (Serial.available()){
      input = Serial.readString();
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
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("Waiting for valid finger...");
      Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }
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
      waitAndGetInput();
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
        waitAndGetInput();
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
        if (getTaskIdx()) { // changes taskIdx and tells if task was found
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
        Serial.println("placeholder 3");
        // enrollFingerprint();
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
      nextState = 4;
      break;

    case 4:
      Serial.println("new task or exit?");
      waitAndGetInput();
      if (input == "exit") {
        Serial.println("placeholder 4");
        // resetVariables();
      }
      nextState = 0;
      break;

    default: 
      Serial.println("Default State: something went wrong");
      break;     
  }
}






