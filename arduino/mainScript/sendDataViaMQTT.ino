// Function to buffer sensor data
void bufferMqttData() {
  String payload = "{" \
    "\"reading_time\":\"" + reading_time + "\"," \
    "\"customer_ID\":\"" + customer_ID + "\"," \
    "\"iot_device_serial_number\":\"" + iot_device_serial_number + "\"," \
    "\"temperature\":" + String(temperature) + "," \
    "\"humidity\":" + String(humidity) + "," \
    "\"wifi_status\":" + String(wifiStatus) + "" \
  "}";

  mqttDataBuffer.push_back(payload);
  Serial.println("Data buffered for MQTT.");

  // Send if buffer is full
  if (mqttDataBuffer.size() >= BUFFER_SIZE) {
    sendBufferedMqttData();
  }
}

// Function to send buffered MQTT data
void sendBufferedMqttData() {
  Serial.println("Sending buffered MQTT data...");
  for (size_t i = 0; i < mqttDataBuffer.size(); i++) {
    if (mqttClient.publish(MQTT_TOPIC, mqttDataBuffer[i].c_str())) {
      Serial.println("MQTT publish succeeded: " + mqttDataBuffer[i]);
    } else {
      Serial.println("MQTT publish failed!");
      break; // Retry later
    }
  }
  mqttDataBuffer.clear();
}