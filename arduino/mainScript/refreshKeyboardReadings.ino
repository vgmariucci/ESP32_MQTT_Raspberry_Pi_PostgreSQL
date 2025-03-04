// Constants
const unsigned long debounceDelay = 200; // Debounce delay in milliseconds

// Variables for debounce handling
unsigned long lastDisplayBtnPressTime = 0;
unsigned long lastWifiBtnPressTime = 0;

void refreshKeyboardReadings() {
  // Read button states
  displayBtnStatus = digitalRead(displayBtn);
  wifiBtnStatus = digitalRead(wifiBtn);

  // Handle display button
  if (!displayBtnStatus && millis() - lastDisplayBtnPressTime > debounceDelay) {
    lastDisplayBtnPressTime = millis(); // Update the last press time

    oledState = !oledState;
    displayBtnCounter++;
    digitalWrite(led, 1);

    if (displayBtnCounter > 2) {
      oledState = 0;
      displayBtnCounter = 0;
      digitalWrite(led, 0);
    }
  }

  // Handle WiFi button
  if (!wifiBtnStatus && millis() - lastWifiBtnPressTime > debounceDelay) {
    lastWifiBtnPressTime = millis(); // Update the last press time

    digitalWrite(led, 1); // Example LED feedback

    // Enables the device to AP mode to set the WiFi connection
    AP_mode_status = 1;
  }
}
