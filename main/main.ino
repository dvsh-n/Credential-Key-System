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

/* Notes: 
Figure out how to get switch case to work with strings. Maybe use dicts.
*/

#include "Header.h"

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing");
  while (!Serial) {
    ;  
  }
}

void waitAndGetInput() {
  while (1) {
    if (Serial.available()){
      input = Serial.readString();
      break;
    }
  }
}

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






