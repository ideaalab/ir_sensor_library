/******************************************************************************
 *						LIBRERIA PARA LEER SENSORES IR
 * 
 * DEFINES QUE HAY QUE DECLARAR:
 * - IR_NUM_SENSORS: dice cuantos sensores estamos usando
 * - IR_CUT_VALUE: (opcional) valor de corte para detecta presencia de objeto
 * - IR_MAX_VALUE: (opcional) valor maximo de lectura por sensor
 * - IR_LED_EMITTER_PIN: (opcional) pin para emisor IR
 * 
 * VARIABLES:
 * - IR_pins[]: asignar a esta variable el pin de cada sensor
 *				(IR_pins[0] = PIN_A3)
 * - IR_sensor_value[]: valor leido por cada sensor. Mientras mas alto mas
 *				distancia
 * - IR_sensor_state[]: un bit por cada sensor, es 1 si el valor del sensor
 *				esta por debajo de IR_CUT_VALUE
 * 
 * FUNCIONES:
 * - IR_init(): inicializa la libreria configurando el Timer1
 * - IR_sensors_read(): lee los valores de los sensores
 * - IR_emitters_on(): enciende los emisores IR
 * - IR_emitters_off(): apaga los emisores IR
 ******************************************************************************/
#include "ir_sensors.h"

/*
 * Leemos los sensores IR
 * Mediremos lo que tarda en pasar el condensador de la entrada de HIGH a LOW
 * El consensador se descarga mas rapido cuanto mas luz recibe el sensor
 * Lo que hacemos es:
 *		- Cargar el condensador poniendo el pin como salida HIGH
 *		- Esperamos 10uS para que cargue
 *		- Ponemos el pin como entrada
 *		- Medimos cuanto tarda el pin en ponerse en LOW
 * 
 * readMode sirve para decirle si queremos que se enciendan los emisores IR:
 * - IR_LED_EMITTERS_OFF: no enciende emisores, lee los valores de luz ambiente
 * - IR_LED_EMITTERS_ON: enciende emisores y no los apaga
 * - IR_LED_EMITTERS_ON_AND_OFF: enciende los emisores solo durante la lectura
 * 
 * Mientras mas tiempo, mas lejos esta el objeto o no hay objeto
 * 
 * El valor devuelto en IR_sensor_value[i] es en uS y varia de 0 a IR_MAX_VALUE
 * 
 * La variable IR_sensor_state indica si hay objeto o no. Cada bit de la variable
 * corresponde a un sensor. El tiempo que se usa para determinar si hay objeto o no
 * es IR_CUT_VALUE. Por debajo de este valor se considera que si hay objeto, sino no
 */

void IR_sensors_read(int readMode = IR_LED_EMITTERS_ON_AND_OFF){
int i;
long time = 0;
	
	//enciendo emisor IR
	if((readMode == IR_LED_EMITTERS_ON) || (readMode == IR_LED_EMITTERS_ON_AND_OFF)){
		IR_emitters_on();
	}
	else{
		IR_emitters_off();
	}
	
	//almaceno el valor antiguo y limpio el nuevo
	IR_prev_sensor_state.Completo = IR_sensor_state.Completo;
	IR_sensor_state.Completo = 0;
	
	//configuro los pines como salida
	for(i = 0; i < IR_NUM_SENSORS; i++){
		IR_sensor_value[i] = IR_MAX_VALUE;
		output_high(IR_pins[i]);
	}

	delay_us(10);	//cargo los condensadores por 10uS

	//pongo los pines como entradas
	for(i = 0; i < IR_NUM_SENSORS; i++){
		input(IR_pins[i]);
	}

	set_timer1(0);	//pongo a 0 el timer1
	
	while(time < IR_MAX_VALUE){
		time = get_timer1();
		
		for(i = 0; i < IR_NUM_SENSORS; i++){
			if((input(IR_pins[i]) == FALSE) && (time < IR_sensor_value[i])){	//
				IR_sensor_value[i] = time;
				
				if(time < IR_CUT_VALUE){
					IR_sensor_state.bit[i] = TRUE;
				}
				else{
					IR_sensor_state.bit[i] = FALSE;
				}
			}
		}
	}
	
	//apago emisor IR
	if(readMode == IR_LED_EMITTERS_ON_AND_OFF){
		IR_emitters_off();
	}
}

/*
 * Enciende los LEDs emisores
 */
void IR_emitters_on(void){
#if IR_LED_EMITTER_PIN != IR_LED_NO_EMITTER_PIN
	#if IR_LED_EMITTER_PIN_ACTIVE == TRUE
		output_high(IR_LED_EMITTER_PIN);
	#else
		output_low(IR_LED_EMITTER_PIN);
	#endif	

	delay_us(200);
#else
	return;
#endif
}

/*
 * Apaga los LEDs emisores
 */
void IR_emitters_off(void){
#if IR_LED_EMITTER_PIN != IR_LED_NO_EMITTER_PIN
	#if IR_LED_EMITTER_PIN_ACTIVE == TRUE
		output_low(IR_LED_EMITTER_PIN);
	#else
		output_high(IR_LED_EMITTER_PIN);
	#endif
	
	delay_us(200);
#else
	return;
#endif
}

/*
 * Inicializa la libreria
 */
void IR_init(void){
//timer1 config para que cada tick sea de 1uS
#if getenv("CLOCK") == 4000000
	setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);
#elif getenv("CLOCK") == 8000000
	setup_timer_1(T1_INTERNAL|T1_DIV_BY_2);
#elif getenv("CLOCK") == 16000000
	setup_timer_1(T1_INTERNAL|T1_DIV_BY_4);
#elif getenv("CLOCK") == 32000000
	setup_timer_1(T1_INTERNAL|T1_DIV_BY_8);
#else
	#error La velocidad del PIC debe ser de 4, 8, 16 o 32Mhz
#endif
}