#include <Wire.h>
#define HW171_ADDRESS 0x20 

/* Definimos los pines de entrada */
#define Sensor1 A0
#define Sensor2 A1
#define Potenciometro A2

#define display1 8
#define display2 9
int variable = 15;

void setup() {
  Serial.begin(9600);
  Wire.begin(); // Inicializar la comunicación I2C
  Wire.beginTransmission(HW171_ADDRESS); // Iniciar transmisión hacia el dispositivo
  Wire.write(0xFF); // Configurar todos los bits como salida (todos en 1)
  Wire.endTransmission(); // Finalizar transmisión
  pinMode(display1, OUTPUT); // Configurar el pin display1 como salida
  pinMode(display2, OUTPUT); // Configurar el pin display2 como salida
}

void loop() {
  int lecturaPot = analogRead(Potenciometro);
  float valorPote = map(lecturaPot, 0, 1023, 0, 3400);
  Serial.print("Temperatura: ");
  Serial.print(promedioTemperatura(Sensor1, Sensor2));
  Serial.println(" °C");

  int unidad = promedioTemperatura(Sensor1, Sensor2) % 10 ;
  int decena = promedioTemperatura(Sensor1, Sensor2) / 10;
  
  encenderDisplay(display1, unidad);
  delay(30); 
  encenderDisplay(display2, decena);
  delay(30); 
  
  digitalWrite(display1, LOW);
  digitalWrite(display2, LOW);
  delay(valorPote); 
}

/*Obtención del promedio*/
int promedioTemperatura(int Sensor1, int Sensor2){
  int lecturaA = analogRead(Sensor1);
  int lecturaB = analogRead(Sensor2);
  int temperatura = (lecturaA + lecturaB) / 2.0; // Obtenemos promedio
  temperatura = ((1024.0 - temperatura) * 5.0 * 10.0) / 1024.0; // Convertir el valor a temperatura en grados Celsius
  return temperatura; 
}

/*Activación de displays con alternancia*/
void encenderDisplay(int numDisplay, int numero){
  digitalWrite(numDisplay, HIGH);
  Wire.beginTransmission(HW171_ADDRESS); // Iniciar transmisión hacia el dispositivo
  switch(numero){
    case 0 : Wire.write(0xd7); // Encender todos los pines (binario: 11111111)
    break;
    case 1 : Wire.write(0x14); // Encender todos los pines (binario: 11111111)
    break;
    case 2 : Wire.write(0x9b); // Encender todos los pines (binario: 11111111)
    break;
    case 3 : Wire.write(0x9e); // Encender todos los pines (binario: 11111111)
    break;
    case 4 : Wire.write(0x7c); // Encender todos los pines (binario: 11111111)
    break;
    case 5 : Wire.write(0xee); // Encender todos los pines (binario: 11111111)
    break;
    case 6 : Wire.write(0xef); // Encender todos los pines (binario: 11111111)
    break;
    case 7 : Wire.write(0x9c); // Encender todos los pines (binario: 11111111)
    break;
    case 8 : Wire.write(0xff); // Encender todos los pines (binario: 11111111)
    break;
    case 9 : Wire.write(0xfc); // Encender todos los pines (binario: 11111111)
    break;
  }
  Wire.endTransmission(); // Finalizar transmisión
}
