#include <VarSpeedServo.h>

// ================= DEFINIÇÃO DE PINOS =================
#define pinservog 2
#define pinservob 4
#define pinservod 7
#define pinservoe 8
#define sensor1 12
#define B1 3
#define B2 5
#define pot1 A0

// ================= CONFIGURAÇÕES DE MEMÓRIA =================
#define MAX_PASSOS_MOTORES 50 
#define MAX_ORDEM 200         
// Matriz de Ângulos: [Motor (0-3)][Índice do Movimento (0-49)]
byte movimento[4][MAX_PASSOS_MOTORES]; 
// Vetor de Ordem: [Índice do Passo (0-199)] = Motor que se moveu (0-3)
int ordem[MAX_ORDEM]; 

// ================= VARIÁVEIS DE ESTADO =================
bool gravando = false;
bool reproduzindo = false;

// Variáveis para a Fila Indiana
int proximo[4] = {0,0,0,0}; // Próximo índice vazio para cada motor (0 a 3)
int contador = 0;           // Total de passos gravados no vetor ordem[]

// Outras Variáveis
int motorSelecionado = 1;
int leituraantiga;
int mudou = 0;

unsigned long tempoAtual;
unsigned long tempoAnterior = 0;

// ================= OBJETOS SERVO E LIMITES =================
VarSpeedServo servog, servob, servoe, servod;

// Array de Ponteiros: {Garra, Base, Esquerda, Direita}
VarSpeedServo* servos[] = { &servog, &servob, &servoe, &servod }; 

// Limites dos servos (Garra, Base, Esquerda, Direita)
const int limites[4][2] = {
  {0, 30},
  {30, 180},
  {0, 135},
  {10, 80}
};

// Posições Iniciais (Garra, Base, Esquerda, Direita)
int posInicial[4] = {0, 30, 135, 10};
int angulos[4] = {0, 30, 135, 10 }; // Armazena o ângulo atual de cada motor


void setup() {
  Serial.begin(9600);

  // Definição dos botões e sensor.
  pinMode(sensor1, INPUT);
  pinMode(B1, INPUT);
  pinMode(B2, INPUT);

  // Inicialização e Anexo dos servos.
  servog.attach(pinservog);
  servob.attach(pinservob);
  servoe.attach(pinservoe); 
  servod.attach(pinservod);

  // Inicializa os servos na posição inicial 
  for (int i = 0; i < 4; i++) {
    servos[i]->write(posInicial[i]);
    delay(100);
  }

  Serial.println(F("Sistema iniciado. Servos prontos!"));
  Serial.println(F("Aguardando comandos..."));
}

void loop() {
  tempoAtual = millis();

  // Verifica se o Botão B1 foi pressionado e não está gravando.
  if (digitalRead(B1) == HIGH && !gravando) {
    delay(200);
    Gravarmovimento();
    tempoAnterior = millis();
  }

  // Verifica se o Sensor (sensor1) foi ativado e não está gravando ou reproduzindo.
  if (digitalRead(sensor1) == LOW && !gravando && !reproduzindo) {
    delay(200);
    reproduzir_movimento();
    tempoAnterior = millis();
  }

  // Se o braço ficar inativo, volta para a posição inicial após 2 segundos.
  if (tempoAtual - tempoAnterior >= 2000 && !gravando && !reproduzindo) {
    tempoAnterior = tempoAtual;
    posicao_inicial();
  }
}

// ========================= POSIÇÃO INICIAL =========================
void posicao_inicial() {
  int velo = 30;
  //usa o array de ponteiros para mover todos
  for (int i = 0; i < 4; i++) {
    servos[i]->write(posInicial[i], velo, true);
  }
}

// ========================= MODO GRAVAÇÃO =========================
void Gravarmovimento() {
  gravando = true;
  motorSelecionado = 1;
  contador = 0; // Zera o contador de passos
  
  // Zera o array de índices e o buffer de movimentos
  for(int i = 0; i < MAX_PASSOS_MOTORES; i++) {
    for(int m = 0; m < 4; m++) {
      movimento[m][i] = 255;
    }
  }
  
  for(int i = 0; i < MAX_ORDEM; i++) {
    ordem[i] = -1;
  }
  
  for(int i = 0; i < 4; i++) {
    proximo[i] = 0; // Zera o índice de gravação de cada motor
  }

  leituraantiga = analogRead(pot1);

  Serial.println(F("=== MODO GRAVACAO INICIADO ==="));
  Serial.println(F("1. Use o POT para mover o motor atual."));
  Serial.println(F("2. Clique B2 para TROCAR o motor."));
  Serial.println(F("3. Clique B1 para SALVAR a posicao."));
  Serial.println(F("4. Segure B2 para SAIR e finalizar."));

  while(gravando) {
    
    // -------------------------------------------------
    // 1. Controle do Motor via POT
    // -------------------------------------------------
    
    int leituraAtual = analogRead(pot1);

    if (abs(leituraAtual - leituraantiga) > 15) {
      mudou = 1;
    } else {
      mudou = 0; // Não houve mudança
    }

    if(mudou == 1) {
      // Mapeia e move o servo imediatamente
      int angulo = map(leituraAtual, 0, 1023, limites[motorSelecionado][0], limites[motorSelecionado][1]);
      
      // Atualiza o array angulos[] para ter o último valor conhecido
      angulos[motorSelecionado] = angulo; 
      
      servos[motorSelecionado]->write(angulo);
    }
    leituraantiga = leituraAtual;

    // -------------------------------------------------
    // 2. Botão B1: Salvar Movimento
    // -------------------------------------------------
    
    if(digitalRead(B1) == HIGH) {
      delay(200);

      if (contador < MAX_ORDEM && proximo[motorSelecionado] < MAX_PASSOS_MOTORES) {
        
        movimento[motorSelecionado][proximo[motorSelecionado]] = angulos[motorSelecionado];
        ordem[contador] = motorSelecionado;
        contador++;
        proximo[motorSelecionado]++; 

        Serial.print(F("Movimento ")); Serial.print(contador);
        Serial.print(F(" (Motor ")); Serial.print(motorSelecionado);
        Serial.println(F(") salvo."));
      } else {
        Serial.println(F("Memoria de passos cheia."));
      }
      while(digitalRead(B1) == HIGH); // Espera soltar o botão B1
    }

    // -------------------------------------------------
    // 3. Botão B2: Trocar Motor ou Sair
    // -------------------------------------------------
    
    if (digitalRead(B2) == HIGH) {
      unsigned long tempoPress = millis();
      while(digitalRead(B2) == HIGH); // Espera soltar o botão

      if (millis() - tempoPress > 1000) {

        // SEGUROU POR 1 SEGUNDO -> SAIR
        gravando = false;
        Serial.print(F("Gravacao Finalizada. Total de passos salvos: "));
        Serial.println(contador);
        return; // Sai da função Gravarmovimento
        
      } else {

        // CLIQUE RÁPIDO -> TROCAR MOTOR
        motorSelecionado++;
        if (motorSelecionado > 3) motorSelecionado = 0;

        Serial.print(F("Controlando Motor: "));
        switch(motorSelecionado) {
          case 0: Serial.println(F("GARRA")); break;
          case 1: Serial.println(F("BASE")); break;
          case 2: Serial.println(F("ESQUERDA")); break;
          case 3: Serial.println(F("DIREITA")); break;
        }
      }
    }
  } 
}


// ========================= MODO REPRODUÇÃO =========================
void reproduzir_movimento() {
  if (contador == 0) {
    Serial.println(F("Nenhum movimento gravado."));
    return;
  }
  
  reproduzindo = true;
  Serial.println(F("Reproduzindo sequencia..."));
  
  // ZERA OS ÍNDICES DE CADA MOTOR 
  for(int i = 0; i < 4; i++) {
    proximo[i] = 0;
  }
  
  // Loop pela ordem gravada
  for(int i = 0; i < contador; i++){ 
    
    int motor = ordem[i];
    int indiceMovimento = proximo[motor];
    
    // Reproduz o ângulo da coluna dedicada do motor, usando a velocidade 30
    servos[motor]->write(movimento[motor][indiceMovimento], 30, true);
    
    // Incrementa o índice daquele motor
    proximo[motor]++;
  }

  // Volta para o Home e finaliza
  Serial.println(F("Fim da reproducao."));
  posicao_inicial();
  reproduzindo = false;
}
