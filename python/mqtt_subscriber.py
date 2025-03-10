import json
import psycopg2
import paho.mqtt.client as mqtt
from dotenv import load_dotenv
import os

load_dotenv()  # Load environment variables from .env

# PostgreSQL Configuration
DB_HOST = os.getenv("DB_HOST")
DB_NAME = os.getenv("DB_NAME")
DB_USER = os.getenv("DB_USER")
DB_PASS = os.getenv("DB_PASS")

# MQTT Configuration
MQTT_BROKER = os.getenv("MQTT_BROKER")
MQTT_PORT = int(os.getenv("MQTT_PORT"))
MQTT_TOPIC = os.getenv("MQTT_TOPIC")
MQTT_USER = os.getenv("MQTT_USER")
MQTT_PASSWORD = os.getenv("MQTT_PASSWORD")

def on_connect(client, userdata, flags, rc, properties):  # Add `properties` for VERSION2
    print(f"Connected to MQTT with code {rc}")
    client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
    try:
        data = json.loads(msg.payload.decode())
        insert_into_db(data)
    except Exception as e:
        print(f"Error processing message: {e}")

def insert_into_db(data):
    conn = None
    try:
        conn = psycopg2.connect(
            host=DB_HOST,
            database=DB_NAME,
            user=DB_USER,
            password=DB_PASS
        )
        cur = conn.cursor()
        cur.execute("""
            INSERT INTO sensor_readings 
            (reading_time, customer_ID, iot_device_serial_number, temperature, humidity, wifi_status)
            VALUES (%s, %s, %s, %s, %s, %s)
        """, (
            data["reading_time"],
            data["customer_ID"],
            data["serial"],
            data["temperature"],
            data["humidity"],
            data["wifi_status"]
        ))
        conn.commit()
        print("Data inserted successfully!")
    except Exception as e:
        print(f"Database error: {e}")
    finally:
        if conn: conn.close()

# Initialize MQTT client with VERSION2
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
client.on_connect = on_connect
client.on_message = on_message

client.connect(MQTT_BROKER, MQTT_PORT, 60)
client.loop_forever()