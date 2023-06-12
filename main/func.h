#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>

typedef struct {
  String alias;
  String password;
  uint8_t userExists;
} user; // ID is equal to index

int getFingerID(Adafruit_Fingerprint finger);
void validateFingerAndPass(user users[], uint8_t ID, uint8_t *access, Adafruit_Fingerprint finger);
void discardUser(user users[], uint8_t ID);
void deleteUser(user users[], Adafruit_Fingerprint finger);
void initializeUsers(user users[]);
void enrollUser(user users[], Adafruit_Fingerprint finger);
uint8_t validateTask(String input, int numCodes, String codes[]);
void printTasks(String codes[], int numCodes);
void waitAndGetInput(uint8_t number, String *input, uint8_t *numInput, uint8_t *exit);
void setupFingerprintSensor(Adafruit_Fingerprint finger);
int enrollFingerprint(Adafruit_Fingerprint finger, uint8_t id);
int deleteFingerprint(Adafruit_Fingerprint finger, uint8_t id);