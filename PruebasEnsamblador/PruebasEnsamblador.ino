/*Proyecto final: LEONARDO AGUILAR MARTÍNEZ*/
#include <Wire.h>
#define HW171_ADDRESS 0x20 //Es la dirección del puerto I2C (A4, A5)
#include <avr/io.h>
#include <avr/interrupt.h>

/* Definimos los pines de entrada */
#define Sensor1 A0
#define Sensor2 A1
#define Potenciometro A2

/*Salidas para activar segmentos*/
#define display1 8
#define display2 9

/*Variables globales para temperatura*/
int lecturaA asm("r0");
int lecturaB asm("r1");
int temperatura1 asm("r2");
int temperatura2 asm("r3");
int promedio asm("r4");

/*Variables auxiliares para darle valor a OCR1A*/
float valorVariableOCR1A asm("r5");

/*Variable auxiliar para Wire.write*/
int aux asm ("r6");

void setup(){
  Serial.begin(9600);
  /*Configuración módulo i2c*/
  Wire.begin(); // Inicializar la comunicación I2C
  Wire.beginTransmission(HW171_ADDRESS); // Iniciar transmisión hacia el dispositivo
  Wire.write(0xFF); // Configurar todos los bits como salida (todos en 1)
  Wire.endTransmission(); // Finalizar transmisión

  /*Configurar salidas para transistores en ensamblador*/
  asm volatile (
    "ldi r16, (1 << %0) | (1 << %1) \n"  // Cargamos 1´s en los primeros bits de DDB0 y DDB1
    "out %2, r16 \n"                     // Escribir el valor en el registro DDRB
    :
    : "I" (DDB0), "I" (DDB1), "I" (_SFR_IO_ADDR(DDRB))
    : "r16"
  );
  
  /*Configuración para el temporizador 1*/
  TCCR1A = 0;                /*Limpiamos registros en 0*/
  TCCR1B = 0;                /*A funciona para overflow y B para CTC*/
  
  TCNT1  = 0;                /*Registro para iniciar el temporizador en 0*/
  OCR1A = 62500;            /* Cargamos valor a comparar (cada segundo según formula)*/
  
  TCCR1B |= (1 << WGM12)|(1 << CS12)|(0<<CS10);   /*Configuración modo CTC, prescaler de 256: CS12 = 1 e CS10 = 0 */ 
  TIMSK1 |= (1 << OCIE1A);  /*Usamos OCIE1A para establecer OCI1A como registro comparador*/
}

void loop(){
  
  /*Leer el potenciometro y asignar su valor a OCR1A para variar el tiempo de interrupción*/
  int lecturaPot = analogRead(Potenciometro); 
  valorVariableOCR1A = map(lecturaPot, 0, 1023, 0.05, 5); /*Se mapea para obtener valores de 500ms a 5s*/
  OCR1A = (((16000000) * (valorVariableOCR1A))/ 256)-1; /*Calculamos los ciclos para OCR1A con la formula*/
  
  int unidad = promedio % 10; /* Obtenemos las unidades y decenas usando promedio que es el valor calculado en ISR*/
  int decena = promedio / 10;
  
  encenderDisplay(display1, unidad); /*Prendemos display 1 y cargamos la unidad para mostrar*/
  /*Delays en ensamblador*/
  asm volatile (
    "LDI R18, 50 \n"       
    "Tiempo_3: \n" 
      "LDI R17, 100 \n"    
    "Tiempo_2: \n"  
      "LDI R16, 50 \n"     
    "Tiempo_1: \n" 
        "DEC R16 \n"        
        "BRNE Tiempo_1 \n"  
        "DEC R17 \n"        
        "BRNE Tiempo_2 \n"  
      "DEC R18 \n"         
    "BRNE Tiempo_3 \n"      
  );
  
  encenderDisplay(display2, decena); /*Repetimos para decenas*/
  asm volatile (
    "LDI R18, 50 \n"       
    "Tiempo_A: \n" 
      "LDI R17, 100 \n"     
    "Tiempo_B: \n"  
      "LDI R16, 50 \n"     
    "Tiempo_C: \n" 
        "DEC R16 \n"        
        "BRNE Tiempo_C \n"  
        "DEC R17 \n"        
        "BRNE Tiempo_B \n"  
      "DEC R18 \n"          
    "BRNE Tiempo_A \n"      
  );
  
  digitalWrite(display1, LOW);
  digitalWrite(display2, LOW);
}


/*Activación de displays con alternancia*/
void encenderDisplay(int numDisplay, int numero){
  digitalWrite(numDisplay, HIGH); /*Encendemos display e inicia transmisión */
  Wire.beginTransmission(HW171_ADDRESS); 

  /*Switch case en ensamblador:
    Comparamos la variable numero con un inmediato, si cumple la condición cargamos dicho
    inmediato en la variable AUX (r6) y salta al final,de lo contrario salta a la
    siguiente instrucción
    */
  asm volatile(
    "cpi %[numero], 0 \n"   // Comparar numero con 0
    "brne L1 \n"             // Si no son iguales, saltar a L1
    "ldi %[aux], 0xd7 \n"   // Si son iguales, cargar 0xd7 en aux
    "rjmp L_END \n"          // Saltar a L_END

    "L1: cpi %[numero], 1 \n" 
    "brne L2 \n"
    "ldi %[aux], 0x14 \n"
    "rjmp L_END \n"

    "L2: cpi %[numero],2 \n" // Comparar numero con 2
    "brne L3 \n"
    "ldi %[aux], 0x9b \n"
    "rjmp L_END \n"

    "L3: cpi %[numero],3 \n" // Comparar numero con 3
    "brne L4 \n"
    "ldi %[aux], 0x9e \n"
    "rjmp L_END \n"

    "L4: cpi %[numero],4 \n" // Comparar numero con 4
    "brne L5 \n"
    "ldi %[aux], 0x7c \n"
    "rjmp L_END \n"

    "L5: cpi %[numero], 5 \n" // Comparar numero con 5
    "brne L6 \n"
    "ldi %[aux], 0xee \n"
    "rjmp L_END \n"

    "L6: cpi %[numero], 6 \n" // Comparar numero con 6
    "brne L7 \n"
    "ldi %[aux], 0xef \n"
    "rjmp L_END \n"

    "L7: cpi %[numero], 7 \n" // Comparar numero con 7
    "brne L8 \n"
    "ldi %[aux], 0x9c \n"
    "rjmp L_END \n"

    "L8: cpi %[numero], 8 \n" // Comparar numero con 8
    "brne L9 \n"
    "ldi %[aux], 0xff \n"
    "rjmp L_END \n"

    "L9: cpi %[numero], 9 \n" // Comparar numero con 9
    "brne L10 \n"
    "ldi %[aux], 0xfc \n"
    "rjmp L_END \n"

    "L10: ldi %[aux], 0xfc \n"  // Si ninguna de las comparaciones fue verdadera, cargar 0xfc en aux

    "L_END: \n"              // Fin del bloque de ensamblador
    :
    : [numero] "r" (numero), [aux] "r" (aux) /*Usamos la variable numero y aux para almacenar en un registro de proposito
                                               general para que se ejecuten dentro de las instrucciones*/
  );

  /*Dado que el retorno de aux es un número entero, necesitamos convertirlo a hexadecimal*/
  String hexadecimal = String(aux, HEX); // Convertir el valor de aux a hexadecimal
  Serial.println("El valor en hexadecimal es: " + hexadecimal);
  Wire.write(strtol(hexadecimal.c_str(), NULL, 16)); /* Convertir el valor hexadecimal a entero y escribirlo en el bus I2C}
             strtol(hexadecimalComoCadena, puntero a puntero de char, base a convertir) e imprimir*/
  Wire.endTransmission(); // Finalizar transmisión
}
/*Interrupción generada por el temporizador 1*/
ISR(TIMER1_COMPA_vect){
  /*Leemos dos valores analogos del sensor 1 y 2*/
  lecturaA = analogRead(Sensor1);
  lecturaB = analogRead(Sensor2);
  /*Factor de conversión para la temperatura*/
  temperatura1 = ((1024.0 - lecturaB) * 62) / 1024.0; 
  temperatura2 = ((1024.0 + lecturaA) * 12) / 1024.0;
  /*Obtenemos el promedio de los dos sensores*/
  promedio = (temperatura1 + temperatura2)/2;
  TCNT1 = 0; /*Reiniciamos el temporizador*/
  Serial.println(promedio);
}
