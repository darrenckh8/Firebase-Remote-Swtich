#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "mmchyy"
#define WIFI_PASSWORD "mm001971"

// Insert Firebase project API Key
#define API_KEY "AIzaSyD8eQWOpZ7mV1ctHhVdS2T70EBnh6kcOjc"

// Insert Authorized Username and Corresponding Password
#define USER_EMAIL "darrenckh8@gmail.com"
#define USER_PASSWORD "Yokenam123"

// Insert RTDB URL & Define the RTDB URL
#define DATABASE_URL "https://fp-ep32-default-rtdb.asia-southeast1.firebasedatabase.app/"

// Define board paths & add more board nodes in database to add more support for more boards
#define B1 "board1/outputs/digital/"
#define B2 "board2/outputs/digital/"
// #define B# "board#/outputs/digital/"

// Define Firebase objects
FirebaseData stream;
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variables to save database paths & board specifier
String listenerPath = B1;

// Initialize WiFi
void initWiFi()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

// Callback function that runs on database changes
void streamCallback(FirebaseStream data)
{
  Serial.printf("stream path, %s\nevent path, %s\ndata type, %s\nevent type, %s\n\n",
                data.streamPath().c_str(),
                data.dataPath().c_str(),
                data.dataType().c_str(),
                data.eventType().c_str());
  printResult(data); // see addons/RTDBHelper.h
  Serial.println();

  // Get the path that triggered the function
  String streamPath = String(data.dataPath());

  // if the data returned is an integer, there was a change on the GPIO state on the following path /{gpio_number}
  if (data.dataTypeEnum() == fb_esp_rtdb_data_type_integer)
  {
    String gpio = streamPath.substring(1);
    int state = data.intData();
    Serial.print("GPIO: ");
    Serial.println(gpio);
    Serial.print("STATE: ");
    Serial.println(state);
    digitalWrite(gpio.toInt(), state);
  }

  /* When it first runs, it is triggered on the root (/) path and returns a JSON with all keys
  and values of that path. So, we can get all values from the database and updated the GPIO states*/
  if (data.dataTypeEnum() == fb_esp_rtdb_data_type_json)
  {
    FirebaseJson json = data.to<FirebaseJson>();

    // To iterate all values in Json object
    size_t count = json.iteratorBegin();
    Serial.println("\n---------");
    for (size_t i = 0; i < count; i++)
    {
      FirebaseJson::IteratorValue value = json.valueAt(i);
      int gpio = value.key.toInt();
      int state = value.value.toInt();
      Serial.print("STATE: ");
      Serial.println(state);
      Serial.print("GPIO:");
      Serial.println(gpio);
      digitalWrite(gpio, state);
      Serial.printf("Name: %s, Value: %s, Type: %s\n", value.key.c_str(), value.value.c_str(), value.type == FirebaseJson::JSON_OBJECT ? "object" : "array");
    }
    Serial.println();
    json.iteratorEnd(); // required for free the used memory in iteration (node data collection)
  }

  // This is the size of stream payload received (current and max value)
  // Max payload size is the payload size under the stream path since the stream connected
  // and read once and will not update until stream reconnection takes place.
  // This max value will be zero as no payload received in case of ESP8266 which
  // BearSSL reserved Rx buffer size is less than the actual stream payload.
  Serial.printf("Received stream payload size: %d (Max. %d)\n\n", data.payloadLength(), data.maxPayloadLength());
}

void streamTimeoutCallback(bool timeout)
{
  if (timeout)
    Serial.println("stream timeout, resuming...\n");
  if (!stream.httpConnected())
    Serial.printf("error code: %d, reason: %s\n\n", stream.httpCode(), stream.errorReason().c_str());
}

void setup()
{
  Serial.begin(115200);
  initWiFi();

  // Initialize Outputs
  int outputPins[] = {2, 4, 5, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33};

  // Iterate through the pins and set them as outputs
  for (int i = 0; i < sizeof(outputPins) / sizeof(outputPins[0]); i++)
  {
    pinMode(outputPins[i], OUTPUT);   // Set the current pin as an output
    digitalWrite(outputPins[i], LOW); // Set the initial state to LOW
  }

  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);

  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Streaming (whenever data changes on a path)
  // Begin stream on a database path --> board1/outputs/digital
  if (!Firebase.RTDB.beginStream(&stream, listenerPath.c_str()))
    Serial.printf("stream begin error, %s\n\n", stream.errorReason().c_str());

  // Assign a calback function to run when it detects changes on the database
  Firebase.RTDB.setStreamCallback(&stream, streamCallback, streamTimeoutCallback);

  delay(2000);
}

void loop()
{
  if (Firebase.isTokenExpired())
  {
    Firebase.refreshToken(&config);
    Serial.println("Refresh token");
  }
  handleSerialInput();
}

void handleSerialInput()
{
  if (Serial.available())
  {
    String command = Serial.readStringUntil('\n'); // Read the command from Serial Monitor

    if (command.startsWith("on ") || command.startsWith("off "))
    {
      int gpioPin = command.substring(3).toInt();

      if ((gpioPin == 2 || gpioPin == 4 || gpioPin == 5) || (gpioPin >= 12 && gpioPin <= 33))
      {
        int newState = command.startsWith("on ") ? 1 : 0; // Set the new state (1 for ON, 0 for OFF)
        Serial.print("Turning ");
        Serial.print(newState == 1 ? "ON " : "OFF ");
        Serial.println("GPIO Pin " + String(gpioPin));

        // Create the path to update in Firebase RTDB (adjust the path to your database)
        String path = "/" + listenerPath + String(gpioPin);

        // Set the new state using Firebase's `setInt` function
        if (Firebase.RTDB.setInt(&fbdo, path.c_str(), newState))
        {
          Serial.println("Firebase RTDB update successful");
        }
        else
        {
          Serial.println("Firebase RTDB setInt failed");
          Serial.println(fbdo.errorReason());
        }
      }
      else
      {
        Serial.println("Invalid GPIO Pin\nUse GPIO pins 2, 4, 5, or 12-33");
      }
    }
    else
    {
      Serial.println("Invalid command. Use 'on <pin>' or 'off <pin>'.");
    }
  }
}