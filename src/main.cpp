#include <Arduino.h>
#include <Wire.h>
#include <esp_bt.h>
#include <esp_wifi.h>
#include <esp_sleep.h>
#include <EEPROM.h>
#include <ADS1110.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

ADS1110 ads(0x48);
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// Pin Definition
const uint8_t SCLPin = 6;
const uint8_t SDAPin = 5;

// Calibration
const float defaultOxygenCalVoltage = 9.0;   // Oxygen voltage in air
const float defaultPureOxygenVoltage = 0.0;  // Oxygen voltage in pure oxygen
float oxygencalVoltage = defaultOxygenCalVoltage;
float pureoxygenVoltage = defaultPureOxygenVoltage;
bool isTwoPointCalibrated = false;           // 2-point calibration

// EEPROM Addresses 
const int ADDR_OXYGEN_CAL_VOLTAGE = 0; 
const int ADDR_PURE_OXYGEN_VOLTAGE = 4;

// Sampling
int16_t oxygenRaw = 0;
float oxygenVoltage = 0.0;
float oxygenmvDisplay = 0.0;
float oxygenPercentage = 0.0;


void setup() {
  // Power Conservation
  esp_wifi_stop();                            // WiFi off
  esp_bt_controller_disable();                // Bluetooth off
  setCpuFrequencyMhz(20);                     // Reduce CPU frequency
  esp_sleep_enable_timer_wakeup(500000);      // 500ms interval

  // I2C Initialisation
  Wire.begin(SDAPin, SCLPin);                 // I2C start
  Wire.setClock(400000);                      // I2C clock speed 400kHz

  // Display Initialisation
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // OLED start
  display.setTextColor(SSD1306_WHITE);        // Set text colour
  display.clearDisplay();                     // Initialise OLED

  // ADS1110 Initialisation
  Wire.beginTransmission(0x48);               // Connection check
  uint8_t error = Wire.endTransmission();
  if (error != 0) {
    display.clearDisplay();
    display.setTextSize(4);
    display.setCursor(30, 28);
    display.print(F("Err"));
    display.display();
    while (true) delay(100);
  }
  ads.reset();                                // ADS1110 start
  ads.setVref(INT_REF);                       // Internal Vref
  ads.setGain(GAIN_8);                        // Gain 8 (256mV)
  ads.setSampleRate(SPS_15);                  // 15 SPS
  ads.setConMode(CONT);                       // Continuous conversion

  // EEPROM Initialisation
  EEPROM.begin(8);                            // EEPROM start
  EEPROM.get(ADDR_OXYGEN_CAL_VOLTAGE, oxygencalVoltage);
  EEPROM.get(ADDR_PURE_OXYGEN_VOLTAGE, pureoxygenVoltage);
  if (isnan(oxygencalVoltage) || oxygencalVoltage <= 0.0) {
    oxygencalVoltage = defaultOxygenCalVoltage;
  }
  if (isnan(pureoxygenVoltage) || pureoxygenVoltage <= 0.0) {
    pureoxygenVoltage = defaultPureOxygenVoltage;
  }
  delay(100);

  // Calibration
  oxygenRaw = ads.getData();                             // ADC reading
  oxygenVoltage = fabs(oxygenRaw * 0.0078125);           // mV at gain 8, 256 mV range
  if (oxygenVoltage > 5.0 && oxygenVoltage < 15.0) {     // 21% calibration
    if (fabs(oxygenVoltage - oxygencalVoltage) > 0.1) {  // Only calibrate if > 0.1 mV difference
      oxygencalVoltage = oxygenVoltage;
      EEPROM.put(ADDR_OXYGEN_CAL_VOLTAGE, oxygencalVoltage);
      EEPROM.commit();
    }
    display.clearDisplay();
    display.setTextSize(4);
    display.setCursor(30, 28);
    display.print(F("21%"));
    display.display();
    delay(500);
  }
  if (oxygenVoltage > 30.0) {                            // 99% calibration
    pureoxygenVoltage = oxygenVoltage;
    EEPROM.put(ADDR_PURE_OXYGEN_VOLTAGE, pureoxygenVoltage);
    EEPROM.commit();
    display.clearDisplay();
    display.setTextSize(4);
    display.setCursor(30, 28);
    display.print(F("99%"));
    display.display();
    delay(500);
  }
  if (pureoxygenVoltage > oxygencalVoltage) {            // 2-point calibration
    isTwoPointCalibrated = true;
  } else {
    isTwoPointCalibrated = false;
  }
}


void loop() {
  // O2 Calculation
  oxygenRaw = ads.getData();                                  // ADC reading
  oxygenVoltage = fabs(oxygenRaw * 0.0078125);                // mV at gain 8, 256 mV range
  oxygenmvDisplay = constrain(oxygenVoltage, 0.00, 99.99);    // mV display: min 0.00, max 99.99
  if (!isTwoPointCalibrated) {
    oxygenPercentage = (oxygenVoltage / oxygencalVoltage) * 20.9;
  } else {
    oxygenPercentage = 20.9 + ((oxygenVoltage - oxygencalVoltage) / (pureoxygenVoltage - oxygencalVoltage)) * (99.0 - 20.9);
  }
  oxygenPercentage = constrain(oxygenPercentage, 0.0, 99.9);  // % display: min 0.0, max 99.9

  // Format
  int intPart = (int)oxygenPercentage;
  int decimalPart = (int)((oxygenPercentage - intPart) * 10 + 0.5);
  if (decimalPart == 10) {
    intPart++;
    decimalPart = 0;
  }

  // Display
  display.clearDisplay();

  display.setTextSize(4);
  display.setCursor(28, 26);
  if (intPart < 10) {
    display.print(F(" "));
  }
  display.print(intPart);

  display.setTextSize(2);
  display.setCursor(74, 38);
  display.print(F("."));

  display.setTextSize(3);
  display.setCursor(84, 26);
  display.print(decimalPart);

  display.setTextSize(1);
  display.setCursor(70, 56);
  if (oxygenmvDisplay < 10) {
    display.print(F(" "));
  }
  display.print(oxygenmvDisplay, 2);

  display.display();
  
  // Sleep
  esp_light_sleep_start();
}