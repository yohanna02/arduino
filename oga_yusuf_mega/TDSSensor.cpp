#include "TDSSensor.h"

#define ADC_MAX 1024.0   // ESP32 ADC resolution

TDSSensor::TDSSensor(uint8_t pin, float vref)
  : _pin(pin), _vref(vref), _index(0), _lastSampleTime(0) {}

void TDSSensor::begin() {
  pinMode(_pin, INPUT);
}

void TDSSensor::update() {
  if (millis() - _lastSampleTime > 40) {
    _lastSampleTime = millis();

    _buffer[_index] = analogRead(_pin);
    _index++;

    if (_index >= SCOUNT)
      _index = 0;
  }
}

float TDSSensor::getTDS(float temperature) {
  memcpy(_bufferTemp, _buffer, sizeof(_buffer));

  int median = getMedian(_bufferTemp, SCOUNT);
  float voltage = median * _vref / ADC_MAX;

  float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
  float compensatedVoltage = voltage / compensationCoefficient;

  float tds =
    (133.42 * compensatedVoltage * compensatedVoltage * compensatedVoltage
    - 255.86 * compensatedVoltage * compensatedVoltage
    + 857.39 * compensatedVoltage) * 0.5;

  return tds;
}

int TDSSensor::getMedian(int arr[], int len) {
  for (int i = 0; i < len; i++) {
    for (int j = i + 1; j < len; j++) {
      if (arr[j] < arr[i]) {
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
      }
    }
  }

  if (len % 2 == 1)
    return arr[len / 2];
  else
    return (arr[len / 2] + arr[len / 2 - 1]) / 2;
}
