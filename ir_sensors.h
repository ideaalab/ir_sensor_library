/*
 * Libreria para sensores infrarojos usando pines I/O del PIC
 * QRE1113: https://www.fairchildsemi.com/products/optoelectronics/infrared/reflective-sensors/QRE1113.html
 */

#ifndef IRSensors_h
#define IRSensors_h

#define IR_MAX_SENSORS 8

#ifndef IR_NUM_SENSORS
#warning No has declarado el numero de sensores
#endif

#if IR_NUM_SENSORS > IR_MAX_SENSORS
#error Numero de sensores no soportado
#endif

/* MODO DE FUNCIONAMIENTO */
#define IR_LED_EMITTERS_OFF 0
#define IR_LED_EMITTERS_ON 1
#define IR_LED_EMITTERS_ON_AND_OFF 2

#ifndef IR_LED_EMITTER_MODE
#define IR_LED_EMITTER_MODE		IR_LED_EMITTERS_ON
#warning No has declarado el modo de funcionamiento de los LEDs IR
#endif

/* PIN DEL EMISOR IR */
#define IR_LED_NO_EMITTER_PIN  255

#ifndef IR_LED_EMITTER_PIN
#define IR_EMITTER_PIN			IR_LED_NO_EMITTER_PIN
#warning No has indicado un pin para el emisor IR, se considera siempre activo
#else
#ifndef IR_LED_EMITTER_PIN_ACTIVE
#warning No has indicado como se activan los emisores
#endif

/* TIEMPOS DE SENSOR */
//tiempo maximo de lectura de cada sensor
#ifndef IR_MAX_VALUE
#define IR_MAX_VALUE		2000
#warning No has declarado el valor maximo de los sensores
#endif

//valor de corte, por encima se considera que no hay presencia de objeto, y por debajo si
#ifndef IR_CUT_VALUE
#define IR_CUT_VALUE		1000
#warning No has declarado valor de corte de los sensores
#endif

/* PROTOTIPOS */
void IR_init(void);
void IR_sensors_read(int readMode = IR_LED_EMITTERS_ON_AND_OFF);
void IR_emitters_on(void);
void IR_emitters_off(void);

/* VARIABLES */
typedef union{
	short bit[8];
	int Completo;
}bits;

bits IR_prev_sensor_state;
bits IR_sensor_state;		//1 bit por cada sensor

//short IR_sensor_state[IR_NUM_SENSORS];		//1 bit por cada sensor
int IR_pins[IR_NUM_SENSORS];
long IR_sensor_value[IR_NUM_SENSORS];

#endif