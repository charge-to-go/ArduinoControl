#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_1_PIN        8         // Configurable, take a unused pin, only HIGH/LOW required, must be diffrent to SS 2
#define SS_2_PIN        10          // Configurable, take a unused pin, only HIGH/LOW required, must be diffrent to SS 1

#define NR_OF_READERS   2

byte ssPins[] = {SS_1_PIN, SS_2_PIN};

int isBattery[] = {0, 0};
bool isNFC;

MFRC522 mfrc522[NR_OF_READERS];   // Create MFRC522 instance.
void dump_byte_array(byte *buffer, byte bufferSize);

/**
 * Initialize.
 */
void setup() {

  Serial.begin(9600); // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

  SPI.begin();        // Init SPI bus

  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN); // Init each MFRC522 card
    mfrc522[reader].PCD_DumpVersionToSerial();
    isBattery[reader] = mfrc522[reader].PICC_IsNewCardPresent();
  }
}

/**
 * Main loop.
 */
void loop() {

  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    if (mfrc522[reader].PICC_IsNewCardPresent()) {
        isNFC = true;
        mfrc522[reader].PICC_IsNewCardPresent();
    } else {
        isNFC = false;
    }
    
    // Look for new cards
    if (isBattery[reader] != isNFC) {
        isBattery[reader] = isNFC; 
               
        if (isNFC) {
                  Serial.println("There is a new battery \n");
                  
                  if (mfrc522[reader].PICC_ReadCardSerial()) {
                     
                      dump_byte_array(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);
                      MFRC522::PICC_Type piccType = mfrc522[reader].PICC_GetType(mfrc522[reader].uid.sak);
                
                      // Halt PICC
                      mfrc522[reader].PICC_HaltA();
                      // Stop encryption on PCD
                      mfrc522[reader].PCD_StopCrypto1();
                  }
                  

        } else {
              Serial.println("Holi");
        }
    }

  } 
  delay(1000);
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
  Serial.println();
}
