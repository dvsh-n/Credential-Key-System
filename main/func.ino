uint8_t nextState = 0; 
uint8_t taskIdx = 0;
uint8_t verified = 0;

#define numCodes  5
String input;
const String password = "espTest";
const String codes[numCodes] = {"login", "logout", "chgPass", "enroll", "delete"}; // Task indexes are important, they are important for switch.

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
