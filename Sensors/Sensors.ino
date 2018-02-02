//#define MY_DEBUG   
#define MY_TRANSPORT_WAIT_READY_MS 1
//#define MY_RF24_ENABLE_ENCRYPTION

#define MY_RADIO_NRF24

//#define MY_NODE_ID 6

#include <SPI.h>
#include <DHT.h>  
#include <MySensors.h> 


#define DHTPIN 3
#define DHTTYPE DHT22



float lastTemp;
float lastHum;
uint32_t SLEEP_TIME = 120000;

#define CHILD_ID_HUM 0
#define CHILD_ID_TEMP 1

#define DIGITAL_INPUT_SENSOR 2
#define CHILD_ID_MOTION 2

#define ANALOG_INPUT_LIGHT_SENSOR 0
#define CHILD_ID_LIGHT 3

MyMessage msgHum(CHILD_ID_HUM, V_HUM);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);
MyMessage msgMotion(CHILD_ID_MOTION, V_TRIPPED);
MyMessage msgLight(CHILD_ID_LIGHT, V_LIGHT_LEVEL);


DHT dht(DHTPIN, DHTTYPE); 


void presentation()  
{ 
  sendSketchInfo("Sensors", "2.0");
  //sendSketchInfo("Motion Sensor", "1.0");
  //sendSketchInfo("Light Sensor", "1.0");

  present(CHILD_ID_HUM, S_HUM);
  present(CHILD_ID_TEMP, S_TEMP);
  present(CHILD_ID_MOTION, S_MOTION);
  present(CHILD_ID_LIGHT, S_LIGHT_LEVEL);


}

void setup()
{
  Serial.begin(115200);  
  dht.begin();
  pinMode(DIGITAL_INPUT_SENSOR, INPUT);
  //pinMode(ANALOG_INPUT_LIGHT_SENSOR,  INPUT);
}

void loop()
{
    SendDHT();
}

void SendDHT() {
    //delay(10000);

    // Light sensor
    float reading = analogRead(ANALOG_INPUT_LIGHT_SENSOR); //Read light level
     int16_t lightLevel = (1023-reading)/10.23;
    Serial.println("Light sensor: " + String(lightLevel));
    send(msgLight.set(lightLevel));
    
    // Motion sensor
    bool tripped = digitalRead(DIGITAL_INPUT_SENSOR) == HIGH;
    String movement = tripped?"YES":"NO";
    Serial.println("Tripped : " + movement);    
    send(msgMotion.set(tripped?"1":"0"));  // Send tripped value to gw

    // Temperature
    float temperature = dht.readTemperature();
    if (temperature != lastTemp) {
        lastTemp = temperature;
        send(msgTemp.set(temperature, 1));
    }

    // Humidity
    float humidity = dht.readHumidity();
    if (humidity != lastHum) {
        lastHum = humidity;
        send(msgHum.set(humidity, 1));
    }
    Serial.println("Temp: " + String(temperature) + ", Hum: " + String(humidity));

    // Sleep until interrupt comes in on motion sensor. Send update every two minute.
    sleep(digitalPinToInterrupt(DIGITAL_INPUT_SENSOR), CHANGE, SLEEP_TIME);

}

