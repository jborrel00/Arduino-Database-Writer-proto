#include <Wire.h>
#include <Adafruit_NFCShield_I2C.h>
#include <stdlib.h>

#define IRQ                     (2)
#define RESET                   (3)     // Not connected by default on the NFC Shield

#define NR_SHORTSECTOR          (32)    // Number of short sectors on Mifare 1K/4K
#define NR_LONGSECTOR           (8)     // Number of long sectors on Mifare 4K
#define NR_BLOCK_OF_SHORTSECTOR (4)     // Number of blocks in a short sector
#define NR_BLOCK_OF_LONGSECTOR  (16)    // Number of blocks in a long sector

// Determine the sector trailer block based on sector number
#define BLOCK_NUMBER_OF_SECTOR_TRAILER(sector) (((sector)<NR_SHORTSECTOR)? \
  ((sector)*NR_BLOCK_OF_SHORTSECTOR + NR_BLOCK_OF_SHORTSECTOR-1):\
  (NR_SHORTSECTOR*NR_BLOCK_OF_SHORTSECTOR + (sector-NR_SHORTSECTOR)*NR_BLOCK_OF_LONGSECTOR + NR_BLOCK_OF_LONGSECTOR-1))

// Determine the sector's first block based on the sector number
#define BLOCK_NUMBER_OF_SECTOR_1ST_BLOCK(sector) (((sector)<NR_SHORTSECTOR)? \
  ((sector)*NR_BLOCK_OF_SHORTSECTOR):\
  (NR_SHORTSECTOR*NR_BLOCK_OF_SHORTSECTOR + (sector-NR_SHORTSECTOR)*NR_BLOCK_OF_LONGSECTOR))

// The default Mifare Classic key
static const uint8_t KEY_DEFAULT_KEYAB[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Create an instance of the NFCShield_I2C class
Adafruit_NFCShield_I2C nfc(IRQ, RESET);

int led = 13;

void setup(void) {
  pinMode(led,OUTPUT);
  Serial.begin(115200);
  //Serial.println("Looking for PN532...");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  // Got ok data, print it out!
  //Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  //Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  //Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  digitalWrite(led,HIGH);
  delay(100);
  digitalWrite(led,LOW);
  delay(100);

  // configure board to read RFID tags
  nfc.SAMConfig();
}

void loop(void) {
  uint8_t success;                          // Flag to check if there was an error with the PN532
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  bool authenticated = false;               // Flag to indicate if the sector is authenticated
  uint8_t blockBuffer[16];                  // Buffer to store block contents
  uint8_t blankAccessBits[3] = { 0xff, 0x07, 0x80 };
  uint8_t idx = 0;
  uint8_t numOfSector = 16;                 // Assume Mifare Classic 1K for now (16 4-block sectors)

  char char1;
  char char2;
  char test[4];
  test[0] = '0';
  test[1] = 'x';
  uint8_t data[16];
  char * pend;
  int value;
  
  //Serial.println("Place your NDEF formatted Mifare Classic 1K card on the reader");
  //Serial.println("and press any key to continue ...");
  
  // Wait for user input before proceeding
  //while (!Serial.available());
  //while (Serial.available()) Serial.read();
    
  // Wait for an ISO14443A type card (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)

success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
Serial.println(success);
if (!success){
  Serial.println("The card couldn't be read");
}
else{
  digitalWrite(led,HIGH);
  delay(1000);
  digitalWrite(led,LOW);
  delay(1000);
  //Serial.println("The card could be read");
  //Serial.println("Found an ISO14443A card/tag");
  //Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
  //Serial.print("  UID Value: ");
  //nfc.PrintHex(uid, uidLength);
  //Serial.println("");
      //one of these keys will authenticate the card
      //keya works if the card has not yet been formatted
      //keyb & keyc work if the card has been ndef formatted
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
      uint8_t keyb[6] = { 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5 };
      uint8_t keyc[6] = { 0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7 };
    
    // Start with block 4 (the first block of sector 1) since sector 0
    // contains the manufacturer data and it's probably better just
    // to leave it alone unless you know what you're doing
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 6, 0, keya);
      Serial.println(success);
      if (!success){
        Serial.println("Block could not be authenticated");
      }
      else{
        //Serial.println("Block could be authenticated");
        digitalWrite(led,HIGH);
        delay(500);
        digitalWrite(led,LOW);
        dleay(500);
        if (Serial.available() == 32){
          for (int index = 0; index < 16; index++){
            char1 = Serial.read();
            char2 = Serial.read();
            test[2] = char1;
            test[3] = char2;
            value = strtol(test, &pend, 0);
            data[index] = value;
            Serial.print((char)data[index]);
          }
        }
        //Players names would be added into data here, i.e.- {'j','o','e',0,0,0,0,0,0,0,0,0,0,0,0,0}
        success = nfc.mifareclassic_WriteDataBlock(6, data);
        Serial.println(success);
          if (success){
            //Serial.println("data was successfully written to the card");
            nfc.PrintHexChar(data, 16);
          }
          else{
            Serial.println("data was not successfully written to the card");
      };
  };
  // Wait a bit before trying again
  Serial.println("\n\nDone!");
  delay(5000);
  Serial.flush();
  while(Serial.available()) Serial.read();
};
};


