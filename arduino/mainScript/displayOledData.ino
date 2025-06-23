void displayOledData() {


  if(!AP_mode_status){

    switch (oledState) {

      case 0:
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(10, 0);
        display.print("Temperature");
        display.setTextSize(2);
        display.setCursor(30, 20);
        display.print(temperature,1);
        display.setCursor(90, 20);
        display.cp437(true);
        display.write(167);
        display.print("C");
        display.setTextSize(1);
        display.setCursor(30, 50);
        display.print(convertDateTimeToOledDisplay(reading_time));
        display.display();
        break;

      case 1:
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(10, 0);
        display.print("Humidity");
        display.setTextSize(2);
        display.setCursor(30, 20);
        display.print(humidity,1);
        display.setCursor(90, 20);
        display.print("%");
        display.setTextSize(1);
        display.setCursor(30, 50);
        display.print(convertDateTimeToOledDisplay(reading_time));
        display.display();
        break;

      default:
        break;
    }
  }
  else{
      display.clearDisplay();
      
      display.setTextSize(1);
      display.setCursor(0,0);
      display.print("SET");
      display.setCursor(100,0);
      display.print("WiFi");

      display.setCursor(30,20);
      display.print("CONNECT WITH");
      display.setCursor(30,40);
      display.print("192.168.4.1");  

      display.display();
      configure_WiFi();
      if(WiFi.status() == WL_CONNECTED){
          AP_mode_status = 0;
          // Set the LED color
          leds[0] = CRGB(0, 0, 0);
          FastLED.show();
      }
  }
}
