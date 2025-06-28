// Constants
const unsigned long debounceDelay = 200; // Debounce delay in milliseconds

// Variables for debounce handling
unsigned long lastDisplayBtnPressTime = 0;
unsigned long lastWifiBtnPressTime = 0;

void refreshKeyboardReadings() {
  
  // Read button states
  displayBtnStatus = digitalRead(DISPLAY_BTN);
  wifiBtnStatus = digitalRead(WIFI_BTN);

  // Handle display button
  if (!displayBtnStatus && millis() - lastDisplayBtnPressTime > debounceDelay) {
    lastDisplayBtnPressTime = millis(); // Update the last press time

    oledState = !oledState;
    displayBtnCounter++;
    // Turn ton the Blue LED
    leds[0] = CRGB(0, 0, 255);
    FastLED.show();

    if (displayBtnCounter > 2) {
      oledState = 0;
      displayBtnCounter = 0;
      // Turn off the RGB LED
      leds[0] = CRGB(0, 0, 0);
      FastLED.show();
    }
  }

  // Handle WiFi button
  if (!wifiBtnStatus && millis() - lastWifiBtnPressTime > debounceDelay) {
    lastWifiBtnPressTime = millis(); // Update the last press time

    //Turn on the Red and Green LEDs (Aka Yellow color)
    leds[0] = CRGB(255, 255, 0);
    FastLED.show();

    // Enables the device to AP mode to set the WiFi connection
    AP_mode_status = 1;
  }
}
