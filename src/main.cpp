#include <Arduino.h>
#include <Wire.h>
#include <BlynkSimpleEsp32.h>
#include <HTTPClient.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFi.h>

#include "BH1750.h"
#include "dht.h"

#include "Adafruit_Sensor.h"
#include "Adafruit_BMP280.h"

#include "My_Defines.h"
#include "My_Debug.h"

BlynkTimer timer;
uint8_t led_status = 0;

Adafruit_BMP280 bmp;
float pressure = 0;
float pressure_total = 0;
float pressure_avg = 0;
float pressure_atm = 0;

float temp2 = 0;
float temp2_total = 0;
float temp2_avg = 0;

volatile byte count = 0;
unsigned long count_time = 0;
float velocidade = 0;
float velocidade_segundos = 0;
float velocidade_km = 0;

BH1750 lightMeter;
uint32_t lux = 0;

dht DHT;
uint16_t hum;
uint16_t hum_total = 0;
uint16_t hum_avg = 0;

float temp = 0;
float temp_avg = 0;
float temp_total = 0;

float sensorhall_one = 0;
float sensorhall_two = 0;
String direcao = "";

// --- Constantes ---
const float pi = 3.14159265; // Número de pi
float period = 1000;		 // Tempo de medida(miliseconds)
float delaytime = 2000;		 // Invervalo entre as amostras (miliseconds)
float radius = 12;			 // Raio do anemometro(mm)

// --- Variáveis Globais ---
unsigned int counter = 0; // Contador para o sensor
float RPM = 0;			  // Rotações por minuto
float speedwind = 0;	  // Velocidade do vento (m/s)
float windspeed = 0;	  // Velocidade do vento (km/h)

TaskHandle_t hTaskSensorDHT;
TaskHandle_t hTaskInitWifi;
TaskHandle_t hTaskEncoder;
TaskHandle_t hTaskLuminosity;
TaskHandle_t hTaskPressure;
TaskHandle_t hTaskLedStatus;
TaskHandle_t hTaskBlynk;
TaskHandle_t hTaskSensorHall;

//?#############################################*/
//!-------------------------------------------------------*/
//?#############################################*/
void InitWifi(void *pvParameters)
{
	disableCore0WDT();

	String ssid = SSID;
	String pass = PASS;

	uint8_t reset = 0;

	int wifi_rssi = 0;

	D_PRINT("\n\nConectando-se a ");
	D_PRINT(ssid);
	WiFi.begin(ssid.c_str(), pass.c_str());
	// Blynk.begin(BLYNK_AUTH_TOKEN, ssid.c_str(), pass.c_str());

	for (;;)
	{
		if (WiFi.status() != WL_CONNECTED)
		{
			led_status = WIFI_DISABLE;
			D_PRINT("\nTrying to connect to the network...");
			reset++;
			if (reset > 20)
			{
				D_PRINT("\nRESETING ESP...");
				ESP.restart();
			}
		}

		else
		{
			led_status = WIFI_CLIENT_INIT;
			reset = 0;
			wifi_rssi = WiFi.RSSI();
			D_PRINT("\nWiFi RSSI:");
			D_PRINT(wifi_rssi);
		}

		vTaskDelay(1000);
	}
}
//?#############################################*/
//!-------------------------------------------------------*/
//?#############################################*/
void LedStatus(void *pvParameters)
{
	uint8_t blink = 0;
	for (;;)
	{
		const TickType_t xDelay = 1000 / portTICK_PERIOD_MS;

		switch (led_status)
		{
		case WIFI_DISABLE:
			if (blink)
			{
				digitalWrite(PIN_LED, HIGH);
			}

			else
			{
				digitalWrite(PIN_LED, LOW);
			}

			blink = !blink;
			break;

		case WIFI_CLIENT_INIT:
			digitalWrite(PIN_LED, HIGH);
			break;
		}
		vTaskDelay(xDelay);
	}
}
//?#############################################*/
//!-------------------------------------------------------*/
//?#############################################*/
void Pressure(void *pvParameters)
{
	for (;;)
	{
		temp2_total = 0;
		temp2_avg = 0;

		pressure_total = 0;
		pressure_avg = 0;

		for (int i = 0; i < NUMBER_READS; i++)
		{
			pressure = bmp.readPressure();
			temp2 = bmp.readTemperature();

			pressure_total += pressure;
			temp2_total += temp2;
		}

		temp2_avg = (float)(temp2_total / NUMBER_READS);
		pressure_avg = (float)(pressure_total / NUMBER_READS);
		pressure_atm = (float)((pressure_avg * ATM) / PASCAL);

		PRESSAO("\nTemperature Avg: ");
		PRESSAO(temp2_avg);
		PRESSAO("*C");

		PRESSAO("\nPressão: ");
		PRESSAO(pressure_avg);
		PRESSAO(" Pa");
		PRESSAO("\t");
		PRESSAO(pressure_atm);
		PRESSAO(" atm\n");

		vTaskDelay(1000);
	}
}
//?#############################################*/
//!-------------------------------------------------------*/
//?#############################################*/
void Luminosity(void *pvParameters)
{
	for (;;)
	{
		lux = lightMeter.readLightLevel();

		LUMINOSITY("\nLuminosity: ");
		LUMINOSITY(lux);
		LUMINOSITY(" lux\n");
		vTaskDelay(1000);
	}
}
//?#############################################*/
//!-------------------------------------------------------*/
//?#############################################*/

void addcount()
{
	count++;
}

void Encoder(void *pvParameters)
{
	for (;;)
	{
		detachInterrupt(PIN_ENCODER);
		RPM = (count / 10.0 * 60.0); // Calculate revolutions per minute (RPM)
		count = 0;
		attachInterrupt(PIN_ENCODER, addcount, RISING);

		velocidade_segundos = (float)((2.0 * pi * radius * RPM) / 60.0) / 1000.0; // Calcula a velocidade do vento em m/s
		velocidade_km = (float)(((2.0 * pi * radius * RPM) / 60.0) / 1000.0) * 3.6; // Calcula velocidade do vento em km/h
		

		ENCODER("\nVelocidade: ");
		ENCODER(RPM);
		ENCODER("RPM\t");
		ENCODER(velocidade_km);
		ENCODER("km/h\t");
		ENCODER(velocidade_segundos);
		ENCODER("m/s\t");

		//! RETA 6,6cm
		//! FUROS 10

		
	
		vTaskDelay(1000);
		
	}
}
//?#############################################*/
//!-------------------------------------------------------*/
//?#############################################*/
void Direction()
{
	if ((sensorhall_one >= 2500 && sensorhall_one <= 2600) && (sensorhall_two >= 3100 && sensorhall_two <= 3200))
		direcao = "NORTE";
	else if ((sensorhall_one >= 3600 && sensorhall_one <= 4000) && (sensorhall_two >= 2990 && sensorhall_two <= 3100))
		direcao = "SUL";

	else if ((sensorhall_one >= 2950 && sensorhall_one <= 3050) && (sensorhall_two >= 3550 && sensorhall_two <= 4000))
		direcao = "OESTE";
	else if ((sensorhall_one >= 3100 && sensorhall_one <= 3200) && (sensorhall_two >= 2600 && sensorhall_two <= 2700))
		direcao = "LESTE";

	
	else if ((sensorhall_one >= 2700 && sensorhall_one <= 2800) && (sensorhall_two >= 3300 && sensorhall_two <= 3400))
		direcao = "NOROESTE";
	else if ((sensorhall_one >= 3400 && sensorhall_one <= 3500) && (sensorhall_two >= 2800 && sensorhall_two <= 2900))
		direcao = "SUDESTE";

	else if ((sensorhall_one >= 3400 && sensorhall_one <= 3500) && (sensorhall_two >= 3400 && sensorhall_two <= 3550))
		direcao = "SUDOESTE";
	else if ((sensorhall_one >= 2700 && sensorhall_one <= 2800) && (sensorhall_two >= 2700 && sensorhall_two <= 2800))
		direcao = "NORDESTE";

	SENSORHALL("\nDirecao: ");
	SENSORHALL(direcao);
}
//?#############################################*/
//!-------------------------------------------------------*/
//?#############################################*/
void SensorHall(void *pvParameters)
{
	float sensor1 = 0;
	float sensor_total1 = 0;

	float sensor2 = 0;
	float sensor_total2 = 0;

	for (;;)
	{
		sensor_total1 = 0;
		sensor_total2 = 0;

		float temp_max = 0;		// variavel para medição max.
		float temp_min = 10000; // variavel para medição min.

		float temp_max2 = 0;	 // variavel para medição max.
		float temp_min2 = 10000; // variavel para medição min.

		for (int i = 0; i < NUMBER_READS; i++)
		{
			sensor1 = analogRead(SENSOR_HALL_ONE);
			sensor2 = analogRead(SENSOR_HALL_TWO);

			sensor_total1 += sensor1;
			sensor_total2 += sensor2;

			if (temp_max < sensor1) // Define qual foi valor max. lido no numero de vezes que foi lido
			{
				temp_max = sensor1;
			}

			if (temp_min > sensor1) // Define qual foi valor min. lido no numero de vezes que foi lido
			{
				temp_min = sensor1;
			}

			if (temp_max2 < sensor2) // Define qual foi valor max. lido no numero de vezes que foi lido
			{
				temp_max2 = sensor2;
			}

			if (temp_min2 > sensor2) // Define qual foi valor min. lido no numero de vezes que foi lido
			{
				temp_min2 = sensor2;
			}
		}

		sensorhall_one = (sensor_total1 / NUMBER_READS);

		sensorhall_two = (sensor_total2 / NUMBER_READS);

		SENSORHALL("\nSENSOR HALL 1: ");
		SENSORHALL(sensorhall_one);
		SENSORHALL("\t");
		SENSORHALL(temp_max);
		SENSORHALL("\t");
		SENSORHALL(temp_min);

		SENSORHALL("\nSENSOR HALL 2: ");
		SENSORHALL(sensorhall_two);
		SENSORHALL("\t");
		SENSORHALL(temp_max2);
		SENSORHALL("\t");
		SENSORHALL(temp_min2);
		Direction();

		vTaskDelay(100);
	}
}
//?#############################################*/
//!-------------------------------------------------------*/
//?#############################################*/
void SensorDHT(void *pvParameters)
{

	for (;;)
	{
		temp_total = 0;
		hum_total = 0;

		for (int i = 0; i < NUMBER_READS; i++)
		{
			DHT.read11(DHTPIN);
			temp = DHT.temperature;
			hum = DHT.humidity;

			temp_total += temp;
			hum_total += hum;
			// vTaskDelay(50);
		}

		temp_avg = (float)(temp_total / NUMBER_READS);
		hum_avg = (hum_total / NUMBER_READS);

		DHT_PRINT("\nTemperature Avg: ");
		DHT_PRINT(temp_avg);
		DHT_PRINT("*C");

		DHT_PRINT("\nHumidity Avg: ");
		DHT_PRINT(hum_avg);
		DHT_PRINT("%\n");
		// teste();

		vTaskDelay(1000);
	}
}
//?#############################################*/
//!-------------------------------------------------------*/
//?#############################################*/
void Send()
{
	//? PRESSÂO
	Blynk.virtualWrite(V0, pressure_avg);
	Blynk.virtualWrite(V1, pressure_atm);
	//? TEMPERATURA
	Blynk.virtualWrite(V2, temp2_avg);
	Blynk.virtualWrite(V6, temp_avg);
	//? LUMINOSIDADE
	Blynk.virtualWrite(V3, lux);
	//? HUMIDADE
	Blynk.virtualWrite(V5, hum_avg);
	//? ENCODER
	Blynk.virtualWrite(V7, velocidade_segundos);
	Blynk.virtualWrite(V4, velocidade_km);
	Blynk.virtualWrite(V8, RPM);
	//? SENSOR HALL
	Blynk.virtualWrite(V9, direcao);
}
//?#############################################*/
//!-------------------------------------------------------*/
//?#############################################*/
void BlynkSend(void *pvParameters)
{
	Blynk.config(BLYNK_AUTH_TOKEN);
	timer.setInterval(1000L, Send);
	for (;;)
	{
		Blynk.run();
		timer.run();
	}
}
//?#############################################*/
//!-------------------------------------------------------*/
//?#############################################*/
void StartTasks()
{
	//--- Creating task for SensorHall ---
	xTaskCreatePinnedToCore(
		SensorHall,		  // Task function.
		"SensorHall",	  // name of task.
		5000,			  // Stack size of task
		NULL,			  // parameter of the task
		1,				  // priority of the task
		&hTaskSensorHall, // Task handle to keep track of created task
		PROCESSOR_MAIN);  // pin task to core communication
	Serial.print("\n>>> Task SensorHall...\tDONE");

	//--- Creating task for Encoder ---
	xTaskCreatePinnedToCore(
		Encoder,		 // Task function.
		"Encoder",		 // name of task.
		5000,			 // Stack size of task
		NULL,			 // parameter of the task
		1,				 // priority of the task
		&hTaskEncoder,	 // Task handle to keep track of created task
		PROCESSOR_MAIN); // pin task to core communication
	Serial.print("\n>>> Task Encoder...\tDONE");

	//--- Creating task for Luminosity ---
	xTaskCreatePinnedToCore(
		Luminosity,		  // Task function.
		"Luminosity",	  // name of task.
		5000,			  // Stack size of task
		NULL,			  // parameter of the task
		1,				  // priority of the task
		&hTaskLuminosity, // Task handle to keep track of created task
		PROCESSOR_MAIN);  // pin task to core communication
	Serial.print("\n>>> Task Luminosity...\tDONE");

	//--- Creating task for Luminosity ---
	xTaskCreatePinnedToCore(
		Pressure,		 // Task function.
		"Pressure",		 // name of task.
		5000,			 // Stack size of task
		NULL,			 // parameter of the task
		1,				 // priority of the task
		&hTaskPressure,	 // Task handle to keep track of created task
		PROCESSOR_MAIN); // pin task to core communication
	Serial.print("\n>>> Task Pressure...\tDONE");

	//--- Creating task for WIFI ---
	xTaskCreatePinnedToCore(
		InitWifi,				  // Task function.
		"InitWifi",				  // name of task.
		10000,					  // Stack size of task
		NULL,					  // parameter of the task
		1,						  // priority of the task
		&hTaskInitWifi,			  // Task handle to keep track of created task
		PROCESSOR_COMMUNICATION); // pin task to core communication
	Serial.print("\n>>> Task InitWifi...\tDONE");

	//--- Creating task for Blynk ---
	xTaskCreatePinnedToCore(
		BlynkSend,				  // Task function.
		"BlynkSend",			  // name of task.
		5000,					  // Stack size of task
		NULL,					  // parameter of the task
		1,						  // priority of the task
		&hTaskBlynk,			  // Task handle to keep track of created task
		PROCESSOR_COMMUNICATION); // pin task to core communication
	Serial.print("\n>>> Task BlynkSend...\tDONE");

	//--- Creating task for LedStatus ---
	xTaskCreatePinnedToCore(
		LedStatus,		 // Task function.
		"LedStatus",	 // name of task.
		5000,			 // Stack size of task
		NULL,			 // parameter of the task
		2,				 // priority of the task
		&hTaskLedStatus, // Task handle to keep track of created task
		PROCESSOR_MAIN); // pin task to core communication
	Serial.print("\n>>> Task LedStatus...\tDONE");

	//--- Creating task for DHT ---
	xTaskCreatePinnedToCore(
		SensorDHT,		 // Task function.
		"SensorDHT",	 // name of task.
		5000,			 // Stack size of task
		NULL,			 // parameter of the task
		1,				 // priority of the task
		&hTaskSensorDHT, // Task handle to keep track of created task
		PROCESSOR_MAIN); // pin task to core communication
	Serial.print("\n>>> Task SensorDHT...\tDONE");
}
//?#############################################*/
//!-------------------------------------------------------*/
//?#############################################*/
void InitSystem()
{
	Serial.begin(115200);

	Wire.begin();
	lightMeter.begin();
	bmp.begin(0x76);

	pinMode(PIN_ENCODER, INPUT);
	pinMode(PIN_LED, OUTPUT);
	pinMode(BUTTON_PIN, INPUT_PULLUP);

	pinMode(SENSOR_HALL_ONE, INPUT);
	pinMode(SENSOR_HALL_TWO, INPUT);
}
//?#############################################*/
//!-------------------------------------------------------*/
//?#############################################*/

void setup()
{
	InitSystem();
	StartTasks();

	attachInterrupt(PIN_ENCODER, addcount, RISING);
}

void loop()
{
	// teste();		  // IMPRIME O TEXTO NA SERIAL
	// delay(2000);
	//  vTaskDelay(1);
}