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

#include "func.h"

// variables
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);

uint8_t nextState = 0;
uint8_t verified = 0;
int numCodes = 5;

String input;
uint8_t numInput;
String password = "espTest";
String codes[5] = {"login", "logout", "chgPass", "enroll", "delete"}; // Task indexes are important, they are important for switch.

#define numUsers 10
user users[numUsers];

// Setup
void setup() {
  Serial.begin(115200);
  Serial.println("Initializing");
  while (!Serial) {
    ;  
  }
  setupFingerprintSensor(finger);
  initializeUsers(users);
}

// Loop
void loop() {
  delay(500);
  switch (nextState) {
    case 0:
      Serial.print("Enter Task ");
      Serial.print("valid Tasks are: ");
      printTasks(codes, numCodes);
      waitAndGetInput(0, &input, &numInput);
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
        waitAndGetInput(0, &input, &numInput);
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
        if (validateTask(input, numCodes, codes)) { // returns 1 if task is found
          nextState = 3; // execute tasks
        }
        else {
          Serial.println("Task was not found");
          nextState = 0;
        }
      }
      else {
        Serial.println("You are not verified, login first");
        nextState = 0;
      }
      break;

    case 3:
      if (input == "enroll"){
        enrollUser(users, finger);
      }
      else if (input == "chgPass"){
        Serial.println("placeholder 3");
        // changePassword();
      }
      else if (input == "delete"){
        deleteFingerprint(finger);
      }
      else {
        Serial.println("Error 3");
      }
      nextState = 0;
      break;

    default: 
      Serial.println("Default State: something went wrong");
      break;     
  }
}






