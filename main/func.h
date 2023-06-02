#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>

typedef struct {
  String userID;
  String password;
  int fingerID;
} user;

uint8_t validateTask(String input, int numCodes, String codes[]);
void printTasks(String codes[], int numCodes);
void waitAndGetInput(uint8_t number, String *input, uint8_t *numInput);
void setupFingerprintSensor(Adafruit_Fingerprint finger);
int enrollFingerprint(Adafruit_Fingerprint finger);
int deleteFingerprint(Adafruit_Fingerprint finger);