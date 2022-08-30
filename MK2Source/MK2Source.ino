#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
DFRobot_BMI160 bmi160;
const int8_t i2c_addr = 0x69;
int led = 8;
bool armed = false;
bool triggered = false;
unsigned long prevReadTime = 0;
const unsigned long minDelay = 3000;

String readNFCTag(MFRC522 nfcreader) {
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial()) {
    return "";
  }

  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  return content;
}

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for connection
  SPI.begin();
  mfrc522.PCD_Init();
  delay(4);
  mfrc522.PCD_DumpVersionToSerial();
  pinMode(led, OUTPUT);
  if (bmi160.softReset() != BMI160_OK) {
    Serial.println("reset false");
    while (1);
  }
  //set and init the bmi160 i2c address
  if (bmi160.I2cInit(i2c_addr) != BMI160_OK) {
    Serial.println("init false");
    while (1);
  }
}

void loop() {
  Serial.println(readNFCTag(mfrc522));
}
