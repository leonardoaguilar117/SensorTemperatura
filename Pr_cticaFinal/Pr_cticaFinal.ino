#include <Wire.h>
#define HW171_ADDRESS 0x20 

const int pinInterrupcion = 2;  // Pin de interrupción
volatile int contador = 0;  // Contador de flancos de subida
volatile unsigned long int ultimoSegundo = 0;  // Almacena el último tiempo de conteo
volatile int intervalo = 1000;  // Intervalo de tiempo en milisegundos (1 segundo)
volatile int cuentaPorSegundo = 0;

#define display1 8
#define display2 9

void setup() {
  Wire.begin(); // Inicializar la comunicación I2C
  Wire.beginTransmission(HW171_ADDRESS); // Iniciar transmisión hacia el dispositivo
  Wire.write(0xFF); // Configurar todos los bits como salida (todos en 1)
  Wire.endTransmission(); // Finalizar transmisión
  
  pinMode(pinInterrupcion, INPUT);  // Configurar el pin de interrupción como entrada
  Serial.begin(9600);  // Iniciar comunicación serie
  attachInterrupt(digitalPinToInterrupt(pinInterrupcion),contadorFlancos, RISING);  // Configurar interrupción en flanco de subida
}

void loop() {
  if (millis() - ultimoSegundo >= intervalo) {
    //noInterrupts();  
    cuentaPorSegundo = contador;
    contador = 0;  // Reinicio
    //interrupts();  
    
    Serial.print("Flancos de subida por segundo: ");
    Serial.println(cuentaPorSegundo);

     Serial.print("Ultimo segundo ");
    Serial.println(ultimoSegundo);
    
    
    ultimoSegundo = millis();  // Actualizar el tiempo para el siguiente intervalo
  }
  
  int unidad =  cuentaPorSegundo % 10 ;
  int decena =  cuentaPorSegundo / 10;
  
  encenderDisplay(display1, unidad);
  delay(15); 
  encenderDisplay(display2, decena);
  delay(15); 
  
  digitalWrite(display1, LOW);
  digitalWrite(display2, LOW);
}

void contadorFlancos() {
  contador++;  
}

/*Activación de displays con alternancia*/
void encenderDisplay(int numDisplay, int numero){
  digitalWrite(numDisplay, HIGH);
  Wire.beginTransmission(HW171_ADDRESS); 
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
  Wire.endTransmission();
}
