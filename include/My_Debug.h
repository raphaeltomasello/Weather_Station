#ifndef My_DEBUG_h	// Main if()
#define My_DEBUG_h

//#define DEBUG
#define PRINT_SENSORHALL
//#define PRINT_DHT
//#define PRINT_ENCODER
//#define PRINT_PRESSAO
//#define PRINT_LUMINOSITY


#ifdef DEBUG
#define D_PRINT(x) Serial.print(x)
#else
#define D_PRINT(x)
#endif

#ifdef PRINT_SENSORHALL
#define SENSORHALL(x) Serial.print(x)
#else
#define SENSORHALL(x)
#endif

#ifdef PRINT_DHT
#define DHT_PRINT(x) Serial.print(x)
#else
#define DHT_PRINT(x)
#endif

#ifdef PRINT_ENCODER
#define ENCODER(x) Serial.print(x)
#else
#define ENCODER(x)
#endif

#ifdef PRINT_PRESSAO
#define PRESSAO(x) Serial.print(x)
#else
#define PRESSAO(x)
#endif

#ifdef PRINT_LUMINOSITY
#define LUMINOSITY(x) Serial.print(x)
#else
#define LUMINOSITY(x)
#endif



#endif	// Main if()