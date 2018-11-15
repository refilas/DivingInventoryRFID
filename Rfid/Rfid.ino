/*
   Initial Author: ryand1011 (https://github.com/ryand1011)

   Reads data written by a program such as "rfid_write_personal_data.ino"

   See: https://github.com/miguelbalboa/rfid/tree/master/examples/rfid_write_personal_data

   Uses MIFARE RFID card using RFID-RC522 reader
   Uses MFRC522 - Library
   -----------------------------------------------------------------------------------------
               MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
               Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
   Signal      Pin          Pin           Pin       Pin        Pin              Pin
   -----------------------------------------------------------------------------------------
   RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
   SPI SS      SDA(SS)      10            53        D10        10               10
   SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
   SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
   SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15

   RXD                      2
   TXD                      4

   Modifié le 13/11/2018 par Bouchet Julien pour l' envoi des données par bluetooth
   en vue de créer une identification du materiel de plongée
*/

#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above

#define RXD_PIN         2
#define TXD_PIN         4

#define LED_BLEU        3           // Led de fin de setup
#define LED_ROUGE       6           // Led ecriture
#define LED_VERTE       7           // Led lecture
#define BOUTON          5

#define ANTI_REBOND     250

// Etat du bouton
int etat = 0;


MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
SoftwareSerial bluetooth(TXD_PIN, RXD_PIN); // RX, TX


//*****************************************************************************************//
void setup() {
  Serial.begin(9600);                                           // Initialize serial communications with the PC
  bluetooth.begin(9600);
  SPI.begin();                                                  // Init SPI bus
  mfrc522.PCD_Init();                                              // Init MFRC522 card
  Serial.println(F("Read personal data on a MIFARE PICC:"));    //shows in serial that it is ready to read
  
  pinMode(LED_BLEU, OUTPUT);
  pinMode(LED_ROUGE, OUTPUT);
  pinMode(LED_VERTE, OUTPUT);
  pinMode(BOUTON, INPUT_PULLUP);

  digitalWrite(LED_BLEU, LOW);
  digitalWrite(LED_VERTE, LOW);
  digitalWrite(LED_ROUGE, HIGH);
}

//*****************************************************************************************//
void loop() {

  // Lecture du poussoir
  if (digitalRead(BOUTON) == LOW && etat == 0)
  {
    etat = 1;
    delay(ANTI_REBOND);
  }
  else if (digitalRead(BOUTON) == LOW && etat == 1)
  {
    etat = 0;
    delay(ANTI_REBOND);
  }

  if (etat == 0)
  {
    digitalWrite(LED_VERTE, LOW);
    digitalWrite(LED_ROUGE, HIGH);


    // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
    MFRC522::MIFARE_Key key;
    for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

    //some variables we need
    byte block;
    byte len;
    MFRC522::StatusCode status;

    //-------------------------------------------

    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      return;
    }

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      return;
    }

    Serial.println(F("**Card Detected:**"));

    //-------------------------------------------

    mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

    //-------------------------------------------

    Serial.print(F("Type: "));
    byte buffer1[18];

    block = 4;
    len = 18;

    //------------------------------------------- GET Type
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Authentication failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    status = mfrc522.MIFARE_Read(block, buffer1, &len);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Reading failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
    Serial.print(" ");

    //---------------------------------------- GET ID

    byte buffer2[18];
    block = 1;

    // J' envoi # pour prévenir qu' il s'agit une nouvelle information
    bluetooth.write("#");

    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Authentication failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    status = mfrc522.MIFARE_Read(block, buffer2, &len);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Reading failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    //PRINT ID
    for (uint8_t i = 0; i < 16; i++) {
      Serial.write(buffer2[i] );
      bluetooth.write(buffer2[i]);
    }
    bluetooth.write("#");
    delay (2000);


    //----------------------------------------

    Serial.println(F("\n**End Reading**\n"));

    delay(200); //change value if you want to read cards faster

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
  //*****************************************************************************************//

  else
  {
    digitalWrite(LED_VERTE, HIGH);
    digitalWrite(LED_ROUGE, LOW);

    //Serial.println("Mode écriture");

    // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
    MFRC522::MIFARE_Key key;
    for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      return;
    }

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      return;
    }
    byte buffer[34];
    byte block;
    MFRC522::StatusCode status;
    byte len;

    Serial.setTimeout(20000L) ;     // wait until 20 seconds for input from serial
    // Création de l'id
    Serial.println(F("Tag détecté"));
    len = Serial.readBytesUntil('#', (char *) buffer, 30) ; // lecture de l'id
    for (byte i = len; i < 30; i++) buffer[i] = ' ';     // pad with spaces

    block = 1;
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("PCD_Authenticate() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    // Write block
    status = mfrc522.MIFARE_Write(block, buffer, 16);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Write() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
    else Serial.println(F("{success:true}"));


    block = 2;
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      //Serial.print(F("PCD_Authenticate() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    // Write block
    status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Write() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
    else Serial.println(F("{success:true}"));

    Serial.println(" ");
    mfrc522.PICC_HaltA(); // Halt PICC
    mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
  }
}
