//********* BLYNK **************
#define BLYNK_TEMPLATE_ID "TMPL2yI3kdDCD"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "7Bnx6qLS4MxJx8ukbOCZdjH0fJq3Vevd"
//********* WIFI **************
#define SSID "2.4G_BUSCAPE"
#define PASS "luaetrovao01"
//********* PINS **************
// Sensor Temp. and Humi.
#define DHTPIN 23 // Digital pin connected to the DHT sensor
//#define DHTTYPE    DHT11 

#define TIMER_ENCODER 1000

#define SENSOR_HALL_ONE 36 //?Laranja
#define SENSOR_HALL_TWO 39 //?Amarelo

#define ATM 1
#define PASCAL 101325

#define PIN_ENCODER 4
#define PIN_LED 2

#define BUTTON_PIN 23

#define NUMBER_READS 10

#define PROCESSOR_COMMUNICATION 1
#define PROCESSOR_MAIN 0

// WIFI
#define WIFI_DISABLE 0       // WiFi disabled (Off)
#define WIFI_ST_MODE 1       // Entering in WiFi mode as client
#define WIFI_SERVER_ENABLE 2 // Entering in WiFi mode as server
#define WIFI_CLIENT_INIT 3   // Initialize WiFi mode as client
#define WIFI_SERVER_INIT 4   // Initialize WiFi mode as server
#define WIFI_INIT_ST_MODE 5  // Connecting in the WiFi net
#define WIFI_INIT_AP_MODE 6  // Entering in WiFi AP mode
#define WIFI_AP_RUNNING 7    // Running in WiFi mode (On)
