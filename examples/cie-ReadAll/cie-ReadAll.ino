/**************************************************************************/
/*! 
  @file     CIE-ReadAll.ino
  @author   Developers italia
  @license  BSD (see license) 
  This example will wait for a CIE card and read all of its unencrypted
  values.
 

This is an example sketch for the Adafruit PN532 NFC/RFID breakout boards
This library works with the Adafruit NFC breakout 
  ----> https://www.adafruit.com/products/364
 
Check out the links above for our tutorials and wiring diagrams 

*/
/**************************************************************************/
#include <Wire.h>
#include <SPI.h>
#include <cie_PN532.h>

// SPI communication is the only supported one at the moment
#define PN532_SCK  (2)
#define PN532_MOSI (3)
#define PN532_SS   (4)
#define PN532_MISO (5)

// I2C is not supported in this release
//#define PN532_IRQ   (2)
//#define PN532_RESET (3)  // Not connected by default on the NFC Shield


cie_PN532 cie(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
typedef bool (cie_PN532::*readValueFunc)(byte*, word*);


void setup(void) {
  #ifndef ESP8266
    while (!Serial); // for Leonardo/Micro/Zero
  #endif
  Serial.begin(115200);
  cie.begin();
}


void loop(void) {
  
  bool cardDetected = cie.detectCard();
  if (!cardDetected) {
    //No card present, we wait for one
    delay(1000);
    return;
  }

  //Good! A card is present, let's dump some info!
  
  readValue(&cie_PN532::read_EF_SN_ICC, "EF_SN_ICC");
  readValue(&cie_PN532::read_EF_DH, "EF_DH");
  readValue(&cie_PN532::read_EF_ATR, "EF_ATR");

  readValue(&cie_PN532::read_EF_ID_Servizi, "EF_ID_Servizi");
  readValue(&cie_PN532::read_EF_Int_Kpub, "EF_Int_Kpub");
  readValue(&cie_PN532::read_EF_Servizi_Int_Kpub, "EF_Servizi_Int_Kpub");

  //SOD is pretty large (1972 bytes), so we'll just print its raw value
  Serial.println(F("EF_SOD"));
  word ef_sod_length = 0;
  cie.print_EF_SOD(&ef_sod_length);
  Serial.print(F("EF_SOD length was "));
  Serial.println(ef_sod_length);

  Serial.println();
  Serial.println(F("Read complete, you can remove the card now"));
  delay(5000);
}
void readValue(readValueFunc func, const char* name) {
  word bufferLength = 600;
  byte* buffer = new byte[bufferLength];
  unsigned long startedAt = millis();
  bool success = (cie.*func)(buffer, &bufferLength);
  if (!success) {
    Serial.print(F("Error reading "));
    Serial.println(name);
    return;
  }
  Serial.print(name);
  Serial.print(" (");
  Serial.print(bufferLength);
  Serial.print(F(" bytes, "));
  Serial.print(millis()-startedAt);
  Serial.print(F(" ms) "));
  cie.printHex(buffer, bufferLength);
  delete [] buffer; 
}
