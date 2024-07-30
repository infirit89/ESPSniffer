#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// should be configured
const char* ssid = "";
const char* pass = "";

const int timeout = 60000;
const int wait = 500;
const int errorDelay = 1000;
bool connected = false;

const char* server_address = "192.168.100.7";

// the 1883 port is default for mqtt server
const uint16_t server_port = 1883;
const char* topic = "esp32/humidity";

WiFiClient client;
PubSubClient mqtt_client(client);

#define CRITICAL_ERROR(Error)\
    do\
    {\
        Serial.println(Error);\
        delay(errorDelay);\
    } while(1)

void mqttCallback(char* topic, byte* payload, uint32_t length) 
{
    Serial.printf("Message receive on topic: %s\n", topic);
    Serial.println("Message:");
    for(uint32_t i = 0; i < length; i++)
        Serial.print((char)payload[i]);
    
    Serial.print("\r\n\r\n");
}

bool publishData() 
{
    return mqtt_client.publish(topic, "He_Lvl=1.00; H20_Flow=1; H2O_Temp=0; Shield=0; ReconRuO=0; ReconSi410=0; ColdheadRuO=0; HePress=0; HT  HDC=0; RF=0; FM=0");
}

void connectToMqttBroker() 
{
    while(!mqtt_client.connected()) 
    {
        String client_id = "esp8266-client" + String(WiFi.macAddress());
        Serial.printf("Connecting to MQTT Broker as %s....\n", client_id.c_str());
        if(mqtt_client.connect(client_id.c_str()))
        {
            Serial.println("Connected to MQTT broker");
            mqtt_client.subscribe(topic);
            publishData();
        }
        else 
        {
            Serial.printf("Failed to connect to MQTT broker, rc=%i\n", mqtt_client.state());
            Serial.println("try again in 5 seconds");
            delay(5000);
        }
    }
}

void setup() 
{
    Serial.begin(9600);
    connectToWifi();
    IPAddress address;
    if(!address.fromString(server_address))
    {
        CRITICAL_ERROR("Couldn't parse ip address");
    }

    mqtt_client.setServer(address, server_port);
    mqtt_client.setCallback(mqttCallback);
}

void loop() 
{
    connectToMqttBroker();

    mqtt_client.loop();
}


void connectToWifi() 
{
    WiFi.begin(ssid, pass);
    Serial.println("Connecting to wifi");
    int current = 0;

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(wait);
        Serial.print('.');
        current += wait;
        if(current >= timeout)
            goto failed_to_connect;
    }
    
    connected = true;
    Serial.println("\nConnected to wifi\n");
    return;

    failed_to_connect:
    CRITICAL_ERROR("\nFailed to connect\n");
}