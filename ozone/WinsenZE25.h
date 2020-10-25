/*
  WinsenZE25.h - This library allows you to set and read the ZE25 Winsen Sensor module.
*/

#ifndef WinsenZE25_h
#define WinsenZE25_h

#include "Arduino.h"
#define CO 1
#define SO2 2
#define NO2 2
#define O2 2
#define NH3 1
#define H2S 1
#define HF 1
#define CL2 2
#define O3 2

#define QA false
#define ACTIVE true

#define DEBUG

class WinsenZE25
{
public:
  WinsenZE25()
  {
    _s = NULL;
  }

  void begin(Stream *ser, int type)
  {
    _s = ser;
    _type = type;
  }

  void setAs(bool active)
  {
    // byte setConfig[] = {0xFF, 0x01, 0x78, 0x04, 0x00, 0x00, 0x00, 0x00, 0x83};//QA config
    //   byte response[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    //
    //   if (active){
    //     setConfig[3] =0x03;
    //     setConfig[8] =0x84;
    //   }
    byte setConfig[] = {0xFF, 0x01, 0x78, 0x41, 0x00, 0x00, 0x00, 0x00, 0x46}; //QA config
    byte response[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    if (active)
    {
      setConfig[3] = 0x40;
      setConfig[8] = 0x47;
    }
    _s->write(setConfig, sizeof(setConfig));
    // Wait for the response
    delay(2000);
    //Flush the incomming buffer
    if (_s->available() > 0)
    {
      _s->readBytes(response, 9);
    }
    while (_s->available() > 0)
    {
      byte c = _s->read();
    }
  }

  float readManual()
  {
    float ppm;
    byte petition[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79}; // Petition to get a single result
    byte measure[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};       // Space for the response
    _s->write(petition, sizeof(petition));
    delay(1500);
    // read
    if (_s->available() > 0)
    {
      _s->readBytes(measure, 9);
    }
    // calculate
    if (measure[0] == 0xff && measure[1] == 0x86)
    {
      ppm = measure[2] * 256 + measure[3]; // this formula depends of the sensor is in the dataSheet
      if (_type == 2)
      {
        ppm = ppm * 0.001;
      }
    }
    else
    {
      ppm = -1;
    }
    return ppm;
  }

  float readContinuous()
  {
    if (_s->available() > 0)
    {
      byte measure[8];
      _s->readBytes(measure, 9);
      // incomingByte = _s.read();
      float ppm = measure[4] * 256 + measure[5];
      //float ppm = measure[2];
      return ppm;
    }
  }

  // --- CUSTOM functions to remove delay ---
  void CUS_sendSetAs(bool active)
  {
    byte setConfig[] = {0xFF, 0x01, 0x78, 0x41, 0x00, 0x00, 0x00, 0x00, 0x46}; //QA config
    // byte response[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    if (active)
    {
      setConfig[3] = 0x40;
      setConfig[8] = 0x47;
    }
    _s->write(setConfig, sizeof(setConfig));
  }

  void CUS_clearBuffer(void)
  {
    byte response[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    //Flush the incomming buffer
    if (_s->available() > 0)
    {
      _s->readBytes(response, 9);
    }
    while (_s->available() > 0)
    {
      byte c = _s->read();
    }
    debugPrint(response);
  }

  void CUS_sendReadManual(void)
  {
    byte petition[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79}; // Petition to get a single result
    // byte measure[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};       // Space for the response
    _s->write(petition, sizeof(petition));
  }

  float CUS_readReadManual(void)
  {
    float ppm;
    byte measure[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // Space for the response
    // read
    if (_s->available() > 0)
    {
      _s->readBytes(measure, 9);
    }
    // calculate
    if (measure[0] == 0xff && measure[1] == 0x86)
    {
      ppm = measure[2] * 256 + measure[3]; // this formula depends of the sensor is in the dataSheet
      if (_type == 2)
      {
        ppm = ppm * 0.001;
      }
    }
    else
    {
      ppm = -1;
    }
    debugPrint(measure);
    return ppm;
  }

private:
  void debugPrint(byte arr[])
  {
#ifdef DEBUG
    Serial.print(arr[0], HEX);
    Serial.print(" ");
    Serial.print(arr[1], HEX);
    Serial.print(" ");
    Serial.print(arr[2], HEX);
    Serial.print(" ");
    Serial.print(arr[3], HEX);
    Serial.print(" ");
    Serial.print(arr[4], HEX);
    Serial.print(" ");
    Serial.print(arr[5], HEX);
    Serial.print(" ");
    Serial.print(arr[6], HEX);
    Serial.print(" ");
    Serial.print(arr[7], HEX);
    Serial.print(" ");
    Serial.println(arr[8], HEX);
#endif
  }

  Stream *_s; //Serial1 - Serial3 are USARTClass objects.
  int _type;
};

#endif