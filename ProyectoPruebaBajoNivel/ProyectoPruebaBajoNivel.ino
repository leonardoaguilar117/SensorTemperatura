/*Proyecto final: LEONARDO AGUILAR MARTÍNEZ*/
#include <Wire.h>
#define HW171_ADDRESS 0x20 
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
int lecturaA = 0;
int lecturaB = 0;
int temperatura1 = 0;
int temperatura2 = 0;
int promedio = 0;

/*Variables auxiliares para darle valor a OCR1A*/
float valorVariableOCR1A = 0;

void setup(){
  Serial.begin(9600);
  /*Configuración módulo i2c*/
  Wire.begin(); // Inicializar la comunicación I2C
  Wire.beginTransmission(HW171_ADDRESS); // Iniciar transmisión hacia el dispositivo
  Wire.write(0xFF); // Configurar todos los bits como salida (todos en 1)
  Wire.endTransmission(); // Finalizar transmisión

  /*Configurar salidas para transistores*/
  pinMode(display1, OUTPUT); // Configurar el pin display1 como salida
  pinMode(display2, OUTPUT); // Configurar el pin display2 como salida

  /*Configuración para el temporizador 1*/
  TCCR1A = 0;                /*Limpiamos registros iniciando en 0*/
  TCCR1B = 0;                
  
  TCNT1  = 0;                /*Inicializamos el temporizador en 0*/
  OCR1A = 62500;            /* Cargamos valor a comparar (cada segundo)*/
  
  TCCR1B |= (1 << WGM12)|(1 << CS12)|(0<<CS10);   /*modo CTC, prescaler de 256: CS12 = 1 e CS10 = 0 */ 
  TIMSK1 |= (1 << OCIE1A);  /*Usamos OCIE1A para establecer OCI1A como registro comparador*/
}

void loop(){
  /*Leer el potenciometro y asignar su valor a OCR1A para variar el tiempo de interrupción*/
  int lecturaPot = analogRead(Potenciometro);
  valorVariableOCR1A = map(lecturaPot, 0, 1023, 0.05, 5); /*De 500ms a 5 s el pote*/
  OCR1A = (((16000000) * (valorVariableOCR1A))/ 256)-1;
  
  int unidad = promedio % 10 ;
  int decena = promedio / 10;
  
  encenderDisplay(display1, unidad);
  delay(10); 
  encenderDisplay(display2, decena);
  delay(10); 
  
  digitalWrite(display1, LOW);
  digitalWrite(display2, LOW);
  
}


/*Activación de displays con alternancia*/
void encenderDisplay(int numDisplay, int numero){
  digitalWrite(numDisplay, HIGH);
  Wire.beginTransmission(HW171_ADDRESS); // Iniciar transmisión hacia el dispositivo
  switch(numero){
    case 0 : Wire.write(0xd7); 
    break;
    case 1 : Wire.write(0x14); 
    break;
    case 2 : Wire.write(0x9b); 
    break;
    case 3 : Wire.write(0x9e);
    break;
    case 4 : Wire.write(0x7c); 
    break;
    case 5 : Wire.write(0xee); 
    break;
    case 6 : Wire.write(0xef); 
    break;
    case 7 : Wire.write(0x9c); 
    break;
    case 8 : Wire.write(0xff); 
    break;
    case 9 : Wire.write(0xfc); 
    break;
  }
  Wire.endTransmission(); // Finalizar transmisión
}

ISR(TIMER1_COMPA_vect){
  lecturaA = analogRead(Sensor1);
  lecturaB = analogRead(Sensor2);
  temperatura1 = ((1024.0 - lecturaB) * 62) / 1024.0; // Convertir el valor a temperatura en grados Celsius 
  temperatura2 = ((1024.0 + lecturaA) * 12) / 1024.0;
  promedio = (temperatura1 + temperatura2)/2;
  TCNT1=0;
  Serial.println(promedio);
}
