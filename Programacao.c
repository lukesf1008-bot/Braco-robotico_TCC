#include <Servo.h>


#define pinservog 2
#define pinservob 4
#define pinservod 7
#define pinservoe 8
#define sensor1 12
#define B1 3
#define B2 5
#define pot1 A0

//contador : Utilizado para indentificar qual motor está sendo reproduzido ou programado.
//contador2 : Utilizado para armazenar a quantidade de movimentos que foram gravados e posteriormente reproduzi-lo.
int contador = 1;  
int contador2 = 0;  

//movimento : Armazena 
int movimento[100];       
bool gravando = false;   
int ultimoCaso = 0;       


Servo servog, servob, servod, servoe;

int imotorg, imotorb, imotord, imotore;


const int limMin1 = 0, limMax1 = 30;
const int limMin2 = 30, limMax2 = 180;
const int limMin3 = 10, limMax3 = 80;
const int limMin4 = 0, limMax4 = 135;


const int pausaMovimento = 500;

unsigned long ultimoTempo = 0;
const unsigned long intervalo = 1000;


void setup() {
  Serial.begin(9600);
  pinMode(sensor1, INPUT);
  pinMode(B1, INPUT_PULLUP);
  pinMode(B2, INPUT_PULLUP);

  servog.attach(pinservog);
  servob.attach(pinservob);
  servod.attach(pinservod);
  servoe.attach(pinservoe);

  imotorg = 0;
  imotorb = 30;
  imotord = 10;
  imotore = 135;

  servog.write(imotorg);
  servob.write(imotorb);
  servod.write(imotord);
  servoe.write(imotore);

  delay(500);

  Serial.println("Sistema iniciado. Servos prontos!\n");
  Serial.println("Aguardando comandos...");
}

// ========================= LOOP =========================
void loop() {
 
  if (digitalRead(B1) == HIGH && !gravando){
    delay(200);
    contador2++;
    if (contador2 > 99) contador2 = 99; 
    gravarMovimento();
  }

 
  if (digitalRead(sensor1) == HIGH){
    delay(200);
    movimentopegaecoloca();
  }
  
  posicao_inicial();
  
}

// ========================= MOVIMENTO =========================
void movimentopegaecoloca() {
  Serial.println("Reproduzindo movimento gravado...");
 
  for (int i = 1; i <= contador2; i++) {
  Serial.print("Reproduzindo movimento[");
  Serial.print(i);
  Serial.print("] = ");
  Serial.println(movimento[i]);

  switch (i%4) {
    case 1:
      servog.write(movimento[i]);
      break;
    case 2:
      servob.write(movimento[i]);
      break;
    case 3:
      servod.write(movimento[i]);
      break;
    case 4:
      servoe.write(movimento[i]);
      break;
  }

  delay(500); 
}
  for (int i = 1; i <= contador2; i++) {
    Serial.print("movimento[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(movimento[i]);
    delay(200);
  }
}

// ========================= POSIÇÃO INICIAL =========================
void posicao_inicial() {
  servod.write(imotord);
  servoe.write(imotore);
  servog.write(imotorg);
  servob.write(imotorb);
}

// ========================= GRAVAR MOVIMENTO =========================
void gravarMovimento() {
  gravando = true;
  Serial.println("Entrando no modo de gravacao...");
  ultimoCaso = 0;
  
  for (int i = 0; i < 100; i++) {
    movimento[i] = 0;
  }
  contador2 = 0; 
  Serial.println("Vetor de movimentos limpo.");
  
  int angulo = 0;

  while (gravando) {
    
    if (contador != ultimoCaso) {
  Serial.print("Gravando ");
  switch (contador) {
    case 1:
      Serial.println("movimento do motor da garra...");
      break;
    case 2:
      Serial.println("movimento do motor da base...");
      break;
    case 3:
      Serial.println("movimento do motor da direita...");
      break;
    case 4:
      Serial.println("movimento do motor da esquerda...");
      break;
  }
  ultimoCaso = contador;
  delay(100);
}


   
    switch (contador) {
      case 1:
        angulo = map(analogRead(pot1), 0, 1023, limMin1, limMax1);
        servog.write(angulo);

        break;
      case 2:
        angulo = map(analogRead(pot1), 0, 1023, limMin2, limMax2);
        servob.write(angulo);

        break;
      case 3:
        angulo = map(analogRead(pot1), 0, 1023, limMin3, limMax3);
        servod.write(angulo);

        break;
      case 4:
        angulo = map(analogRead(pot1), 0, 1023, limMin4, limMax4);
        servoe.write(angulo);
        break;
    }
     
    
    
    if (digitalRead(B1) == HIGH) {
      delay(200);

      contador2++;
      if (contador2 > 99) contador2 = 99;

      movimento[contador2] = angulo; 
      Serial.print("Registrando movimento[");
      Serial.print(contador2);
      Serial.print("] = ");
      Serial.println(movimento[contador2]);
      contador++;
     
      if (contador > 4) contador = 1;
    }

    
    if (digitalRead(B2) == HIGH) {
      delay(200);
      gravando = false;
      Serial.println("Saindo do modo de gravação...");
    }
  }
}
