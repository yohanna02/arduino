#ifndef TDS_SENSOR_H
#define TDS_SENSOR_H

#include <Arduino.h>

class TDSSensor {
public:
  TDSSensor(uint8_t pin, float vref = 5);

  void begin();
  void update();                     // call frequently in loop()
  float getTDS(float temperature);   // get TDS value (ppm)

private:
  static const int SCOUNT = 30;

  uint8_t _pin;
  float _vref;

  int _buffer[SCOUNT];
  int _bufferTemp[SCOUNT];
  int _index;

  unsigned long _lastSampleTime;

  int getMedian(int arr[], int len);
};

#endif
