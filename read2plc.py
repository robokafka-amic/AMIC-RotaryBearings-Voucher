from flask import Flask, request
import paho.mqtt.client as mqtt
import threading
import json

# Configurations
MQTT_BROKER = "10.90.101.241"  # Replace with your MQTT broker IP
MQTT_PORT = 1883  # Typically 1883 for MQTT
MQTT_USERNAME = "user"
MQTT_PASSWORD = "123qweasd"
BASE_TOPIC = "RotaryBearings/Neuron"  # Main MQTT topic base

# Initialize Flask app for webhook
app = Flask(__name__)

# MQTT client setup
mqtt_client = mqtt.Client()
mqtt_client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT Broker!")
    else:
        print("Failed to connect, return code %d\n", rc)

mqtt_client.on_connect = on_connect

def connect_mqtt():
    try:
        mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)
        mqtt_client.loop_start()
    except Exception as e:
        print(f"MQTT connection error: {e}")

# Function to publish data to separate subtopics
def publish_data(data):
    for key, value in data.items():
        topic = f"{BASE_TOPIC}/{key}"
        
        # Convert lists and dictionaries to JSON strings
        if isinstance(value, (list, dict)):
            message = json.dumps(value)
        else:
            message = str(value)
        
        mqtt_client.publish(topic, message)
        print(f"Data '{message}' sent to topic '{topic}'")

# Flask endpoint to receive data and publish to MQTT
@app.route('/wh', methods=['POST'])
def webhook():
    data = request.json
    print("Received data:", data)
    
    # Publish each field to its own MQTT subtopic
    publish_data(data)
    
    return 'Data received and published', 200

# Run Flask server in a separate thread
def run_webhook():
    app.run(port=5001)

threading.Thread(target=run_webhook).start()
connect_mqtt()
