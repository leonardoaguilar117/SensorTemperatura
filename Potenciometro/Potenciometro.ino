#include <Wire.h>

#define HW171_ADDRESS 0x20 // Dirección del dispositivo HW-171
#define display1 8
#define display2 9
int variable = 15;
  
void setup() {
  Wire.begin(); // Inicializar la comunicación I2C
  Wire.beginTransmission(HW171_ADDRESS); // Iniciar transmisión hacia el dispositivo
  Wire.write(0xFF); // Configurar todos los bits como salida (todos en 1)
  Wire.endTransmission(); // Finalizar transmisión
  pinMode(display1, OUTPUT); // Configurar el pin display1 como salida
  pinMode(display2, OUTPUT); // Configurar el pin display2 como salida
}

void loop() {
  int unidad = variable % 10 ;
  int decena = variable / 10;
  encenderDisplay(display1, unidad);
  delay(50); 
  encenderDisplay(display2, decena);
  delay(50); 
  digitalWrite(display1, LOW);
  digitalWrite(display2, LOW);
}

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
