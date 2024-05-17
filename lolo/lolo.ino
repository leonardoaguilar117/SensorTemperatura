int pulsePin = 8;
int pulseHigh, pulseLow, pulseTotal, frequency;
void setup(){
  Serial.begin(9600);
  pinMode(pulsePin,INPUT);
}
void loop(){


pulseHigh = pulseIn(pulsePin,HIGH);
pulseLow = pulseIn(pulsePin,LOW);

pulseTotal = pulseHigh + pulseLow; // Time period of the pulse in microseconds
frequency=1000000/pulseTotal; // Frequency in Hertz (Hz)

Serial.write(frequency);
delay(500);
}
