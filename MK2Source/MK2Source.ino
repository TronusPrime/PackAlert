#include <DFRobot_BMI160.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above
#define MINDELAY        500
#define LED1            8
#define BMI_I2C_ADDR    0x69

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
DFRobot_BMI160 bmi160;

bool armed = false;
bool triggered = false;
unsigned long prevReadTime = 0;
const String uid = "93 45 F2 94";
String nfcData = "";
int rslt;
int16_t accelGyro[6] = {0};

String readNFCTag(MFRC522 nfcreader) {

  String content = "";
  byte letter;
  
  if ( ! nfcreader.PICC_IsNewCardPresent() || ! nfcreader.PICC_ReadCardSerial()) {
    return content;
  }
  
  for (byte i = 0; i < nfcreader.uid.size; i++) {
    content.concat(String(nfcreader.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(nfcreader.uid.uidByte[i], HEX));
  }
  
  content.toUpperCase();
  
  return content;
}

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for connection
  
  SPI.begin();
  
  mfrc522.PCD_Init();
  delay(4);
  mfrc522.PCD_DumpVersionToSerial();
  
  pinMode(LED1, OUTPUT);
  
  if (bmi160.softReset() != BMI160_OK) {
    Serial.println("reset false");
    while (1);
  }
  
  //set and init the bmi160 i2c address
  if (bmi160.I2cInit(BMI_I2C_ADDR) != BMI160_OK) {
    Serial.println("init false");
    while (1);
  }
}

void loop() {
  rslt = bmi160.getAccelGyroData(accelGyro);
  nfcData = readNFCTag(mfrc522);

  if (millis() - prevReadTime > MINDELAY) {
    if (nfcData.substring(1) == uid) // Make sure you change this with your own UID number
    {
      Serial.println(nfcData);
    }
  }

  // Must be after all usages of 'prevReadTime'
  if (nfcData != "") {
    prevReadTime = millis();
  }
}
