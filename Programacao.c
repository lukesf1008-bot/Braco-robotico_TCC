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

//movimento : Armazena os movimentos dos servos de forma ordenada: Garra, Base, Direita e Esquerda.
int movimento[100];       
bool gravando = false;   
int ultimoCaso = 0;       


Servo servog, servob, servod, servoe;

//Variavel que armazena as posições iniciais dos servos
int imotorg, imotorb, imotord, imotore;


//Limites dos servos 
const int limMin1 = 0, limMax1 = 30; //Garra
const int limMin2 = 30, limMax2 = 180; //Base
const int limMin3 = 10, limMax3 = 80;  //Direita
const int limMin4 = 0, limMax4 = 135;  //Esquerda




void setup() {
  Serial.begin(9600);
  
  //Definição dos botões.
  pinMode(sensor1, INPUT);
  pinMode(B1, INPUT_PULLUP);
  pinMode(B2, INPUT_PULLUP);

  //Inicialização dos servos.
  servog.attach(pinservog);
  servob.attach(pinservob);
  servod.attach(pinservod);
  servoe.attach(pinservoe);

  //Definição do ponto incial dos servos.
  imotorg = 0;
  imotorb = 30;
  imotord = 10;
  imotore = 135;
  
  //Inicializa os servos na posição inicial.
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
 
  //Verifica se o Botão B1 foi precionado e que não está gravando.
  if (digitalRead(B1) == HIGH && !gravando){
    delay(200);
    
    //contabiliza que o B1 foi pressionado para que o primeiro movimento seja contabilzado também.
    contador2++;
    
    gravarMovimento();
  }

  if (digitalRead(sensor1) == HIGH){
    delay(200);
    Reproduzir_movimento();
  }
  
  posicao_inicial();
  
}

// ========================= MOVIMENTO =========================
void Reproduzir_movimento() {
  Serial.println("Reproduzindo movimento gravado...");
 
  //Imprime determinada frase para determinado movimento.
  for (int i = 1; i <= contador2; i++) {
  Serial.print("Reproduzindo movimento[");
  Serial.print(i);
  Serial.print("] = ");
  Serial.println(movimento[i]);

  //reproduz o movimento armazenado no motor especifíco na qual foi gravado. 
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
  //variável de controle para a impressão única das frases l.150
  ultimoCaso = 0;
  
  //Reseta vetor movimento, a variável de controle e o angulo.
  for (int i = 0; i < 100; i++) {
    movimento[i] = 0;
  }
  
  contador2 = 0; 
  
  Serial.println("Vetor de movimentos limpo.");
  int angulo = 0;

  while (gravando) {
     
    if(contador2 > 100){
       Serial.println("Erro: Limite de movimentos gravados atingidos.");
       break;
      }
    
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


   //Movimenta e armazena determinado angulo para determinado motor.
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
     //Incrementa contador para contabilizar a passagem para o proximo motor.
      contador2++;

     
      
      //Registra o angulo definido na posição equivalente no vetor.
      movimento[contador2] = angulo; 
      Serial.print("Registrando movimento[");
      Serial.print(contador2);
      Serial.print("] = ");
      Serial.println(movimento[contador2]);
      contador++;
     
      //Reseta a variável contador para 1 novamento para continuar o ciclo.
      if (contador > 4) contador = 1;
    }
    

    //Caso B2 seja clicado, salva o vetor e sai do modo de gravação.
    if (digitalRead(B2) == HIGH) {
      delay(200);
      gravando = false;
      Serial.println("Saindo do modo de gravação...");
    }
    
  }
}
