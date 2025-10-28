#include <Adafruit_INA219.h>

extern Adafruit_INA219 ina219;
float busVoltage = 0, current_mA = 0, power_mW = 0, loadVoltage = 0, shuntVoltage = 0;

void initINA219Module() {
  Wire.begin(D2, D1); // SDA, SCL for sensor
  if (!ina219.begin()) {
    Serial.println("INA219 not found!");
    
  } else {
    Serial.println("INA219 initialized.");
    Serial.println("Measuring voltage and current with INA219 ...");
  }
    ina219.setCalibration_32V_2A();
}

void readINA219() {
  shuntVoltage = ina219.getShuntVoltage_mV();
  busVoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadVoltage = busVoltage + (shuntVoltage / 1000.0);
  
  Serial.print("Bus Voltage:   "); Serial.print(busVoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntVoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadVoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
  Serial.println("");

}


