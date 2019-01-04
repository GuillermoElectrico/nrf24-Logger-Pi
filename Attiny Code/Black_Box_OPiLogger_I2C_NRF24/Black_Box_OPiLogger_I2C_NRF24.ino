/*  ----------------------------------------------------------------
    Gateway beteen nrf24l01+ and orange pi zero vía ATTiny85 for PiLog System
    --------------------------------------------------------------------
*/
// añadimos librerías necesarias
#include <TinyWire.h>  //https://github.com/lucullusTheOnly/TinyWire
#include <avr/wdt.h>
#include <NRFLite.h>
//#include <EEPROM.h>
// 2-Pin Hookup Guide on https://github.com/dparson55/NRFLite
#define RADIO_ID 0              // Our radio's id node.  The transmitter will send to this id.
#define wchannelDef 100 // por defecto canal 100
#define wbaudratelDef 2 // por defecto 2 mbps
#define PIN_RADIO_MOMI 4
#define PIN_RADIO_SCK 3
// I2C address
#define own_address 5
// inits address in the eeprom to save/load config
#define wchannel_address 30 // 1 byte (30)
#define wbaudratel_address 35 // 1 byte (35)

struct RadioPacket // Any packet up to 32 bytes can be sent.
{
  byte FromRadioId;
  char DataType;
  byte InputNumber;
  long RadioDataLong;
  float RadioDataFloat;
  unsigned long FailedTxCount;
};

NRFLite _radio;
RadioPacket _radioData;

byte wchannel = 100;             // 0-125 (2400 - 2525 MHz)
byte wbaudrate = 2;              // 2 => BITRATE2MBPS, 1 => BITRATE1MBPS, 0 = > BITRATE250KBPS

byte newData = 0;

unsigned long previousMillisEstatus = 0;

boolean receive = false;

void setup()
{
  // config TinyWire library for I2C slave functionality
  TinyWire.begin(own_address);
  // register a handler function in case of a request from a master
  TinyWire.onRequest(onI2CRequest);
  // sets callback for the event of a slave receive
  //  TinyWire.onReceive( onI2CReceive );

  // Cargamos configuración de la eeprom, si hay
  //  EEPROM.get(wchannel_address, wchannel);
  //  EEPROM.get(wbaudratel_address, wbaudrate);

  if (wbaudrate == 2) {
    if (!_radio.initTwoPin(RADIO_ID, PIN_RADIO_MOMI, PIN_RADIO_SCK, NRFLite::BITRATE2MBPS, wchannel))
    {
      wdt_enable(5);
      while (1);
    }
  } else if (wbaudrate == 1) {
    if (!_radio.initTwoPin(RADIO_ID, PIN_RADIO_MOMI, PIN_RADIO_SCK, NRFLite::BITRATE1MBPS, wchannel))
    {
      wdt_enable(5);
      while (1);
    }
  } else {
    if (!_radio.initTwoPin(RADIO_ID, PIN_RADIO_MOMI, PIN_RADIO_SCK, NRFLite::BITRATE250KBPS, wchannel))
    {
      wdt_enable(5);
      while (1);
    }
  }
}

void loop()
{

  // comprobamos si hay algúna comunicacion pendiente vía radio
  if (receive) {
    if (_radio.hasData())
    {
      _radio.readData(&_radioData); // Note how '&' must be placed in front of the variable name.
      receive = false;
      newData++;
    }
  }

}

/*
  void reboot() {
  wdt_enable(5);
  while (1);
  }
*/

// Request Event handler function
void onI2CRequest() {
  TinyWire.send(byte(_radioData.FromRadioId));                    // 1st byte
  TinyWire.send(byte(_radioData.DataType));                       // 2nd byte
  TinyWire.send(byte(_radioData.InputNumber));                    // 3st byte
  TinyWire.send(byte(_radioData.RadioDataLong >> 24));            // 4nd byte
  TinyWire.send(byte(_radioData.RadioDataLong >> 16));            // 5rd byte
  TinyWire.send(byte(_radioData.RadioDataLong >> 8));             // 6st byte
  TinyWire.send(byte(_radioData.RadioDataLong));                  // 7nd byte
  
  volatile byte* FloatPtr = (byte*) &_radioData.RadioDataFloat;
  TinyWire.send(FloatPtr[0]);                                     // 8nd byte
  TinyWire.send(FloatPtr[1]);                                     // 9nd byte
  TinyWire.send(FloatPtr[2]);                                     // 10st byte
  TinyWire.send(FloatPtr[3]);                                     // 11nd byte
  /*
    TinyWire.send(byte(_radioData.RadioDataFloat >> 24));                         // 8nd byte
    TinyWire.send(byte(_radioData.RadioDataFloat >> 16));                         // 9nd byte
    TinyWire.send(byte(_radioData.RadioDataFloat >> 8));                          // 10st byte
    TinyWire.send(byte(_radioData.RadioDataFloat));                               // 11nd byte
  */
  TinyWire.send(byte(_radioData.FailedTxCount >> 24));            // 12st byte
  TinyWire.send(byte(_radioData.FailedTxCount >> 16));            // 13nd byte
  TinyWire.send(byte(_radioData.FailedTxCount >> 8));             // 14nd byte
  TinyWire.send(byte(_radioData.FailedTxCount));                  // 15th byte
  TinyWire.send(newData);                                         // 16th byte
  receive = true;
}

/*
  I2C Slave Receive Callback:
  Note that this function is called from an interrupt routine and shouldn't take long to execute
*/
/*
  void onI2CReceive(int howMany) {
  char bufb;    //buffer donde almacenar valor baudrate 0-2
  char bufc; //buffer donde almacenar valor canal 0-125
  // loops, until all received bytes are read
  while (TinyWire.available() > 0) {
    char data = TinyWire.read();
    // Si 'b' es recibido leer siguiente valor y almacenarlo en buffer
    if (data == 'b') {
      bufb = TinyWire.read();
      wbaudrate = atoi(bufb);
      EEPROM.update(wbaudratel_address, wbaudrate);
    }
    // Si 'c' es recibido leer los 3 siguientes siguiente valores y almacenarlo en buffer
    if (data == 'c') {
      bufc = TinyWire.read();
      wchannel = atoi(bufc);
      EEPROM.update(wchannel_address, wchannel);
    }
  }
  reboot();
  }
*/

