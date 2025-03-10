-- Query to create the sensor_readings table in the iot_data database
-- The table will store the sensor readings sent by the ESP32 devices via MQTT

CREATE TABLE sensor_readings (
    id SERIAL PRIMARY KEY,
    reading_time TIMESTAMPTZ NOT NULL,
    customer_ID TEXT NOT NULL,
    iot_device_serial_number TEXT NOT NULL,
    temperature NUMERIC(5,2),
    humidity NUMERIC(5,2),
    wifi_status INTEGER
);


-- Insert a test row to verify permissions:
INSERT INTO sensor_readings 
(reading_time, customer_ID, iot_device_serial_number, temperature, humidity, wifi_status)
VALUES (NOW(), 'test_customer', 'esp32_001', 25.5, 60.0, 1);