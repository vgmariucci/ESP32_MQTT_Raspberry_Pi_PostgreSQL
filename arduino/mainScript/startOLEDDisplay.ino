void startOLEDDisplay() {

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  delay(2000);

  display.setRotation(2);
  display.clearDisplay();
  display.setTextColor(WHITE);
}
