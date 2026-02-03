#include <VarSpeedServo.h>
#include <EEPROM.h> // Biblioteca para salvar dados permanentemente

// ================= DEFINIÇÃO DE PINOS =================
#define pinservog 2
#define pinservob 4
#define pinservod 7
#define pinservoe 8
#define sensor1 12     
#define B1 3           
#define B2 5           
#define pot1 A0
#define LED_GRAVANDO 10
#define LED_REPRODUZINDO 9

// ================= CONFIGURAÇÕES DE MEMÓRIA E CONSTANTES =================
#define MAX_PASSOS_MOTORES 50   
#define MAX_ORDEM 200           
const int limite_histere = 20;    
const int NUM_leituras = 10; 


// ================= ENDEREÇOS EEPROM =================
#define EEPROM_ADDR_CONTADOR   0    // int (2 bytes) - contador será adicionado na posição 0 e 1 da memoria permanente
#define EEPROM_ADDR_ORDEM      2    // int[200] (400 bytes) - ordem será adicionado na posição 2 à 401 da memoria permanente
#define EEPROM_ADDR_MOVIMENTO  402  // byte[4][50] (200 bytes) - movimento será adicionado na posição 402 à 602 da memoria permanente


// Matriz de Ângulos e Vetor de Ordem
byte movimento[4][MAX_PASSOS_MOTORES]; 
int ordem[MAX_ORDEM]; 

// ================= VARIÁVEIS DE ESTADO =================
bool gravando = false;
bool reproduzindo = false;
bool mudou = false;

int proximo[4] = {0,0,0,0}; 
int contador = 0;           

int motorSelecionado = 0; // Inicia na Garra (0)
int leituraantiga;

unsigned long tempoAtual;
unsigned long tempoAnterior = 0;

// ================= OBJETOS SERVO E LIMITES =================
VarSpeedServo servog, servob, servoe, servod;

VarSpeedServo* servos[] = { &servog, &servob, &servoe, &servod }; 

const int limites[4][2] = {
  {0, 60},   // 0: GARRA
  {0, 100}, // 1: BASE
  {30, 150},  // 2: BRAÇO ESQUERDA
  {10, 80}   // 3: BRAÇO DIREITA
};

int posInicial[4] = {0, 30, 135, 10};
int angulos[4] = {0, 30, 135, 10 }; 

// ========================= FUNÇÕES EEPROM =========================

void salvarMovimentoEEPROM() {
  
  //salva o contator na posição 0 e 1 
  EEPROM.put(EEPROM_ADDR_CONTADOR, contador);

  //salva a ordem na posição 2 até contador
  for (int i = 0; i < contador; i++) {
    EEPROM.put(EEPROM_ADDR_ORDEM + (i * sizeof(int)), ordem[i]);
  }

  
  int addr = EEPROM_ADDR_MOVIMENTO;
  //salva a ordem na posição 402 em diante
  for (int m = 0; m < 4; m++) {
    for (int p = 0; p < MAX_PASSOS_MOTORES; p++) {
      EEPROM.put(addr, movimento[m][p]);
      addr += sizeof(byte);
    }
  }

  Serial.println(F("Movimentos salvos na EEPROM!"));
}


void carregarMovimentoEEPROM() {
  
  EEPROM.get(EEPROM_ADDR_CONTADOR, contador);
  
  if (contador > 0 && contador <= MAX_ORDEM) {
    
    for (int i = 0; i < contador; i++) {
      EEPROM.get(EEPROM_ADDR_ORDEM + (i * sizeof(int)), ordem[i]);
    }
    
    
    int addr = EEPROM_ADDR_MOVIMENTO;
    for (int m = 0; m < 4; m++) {
      for (int p = 0; p < MAX_PASSOS_MOTORES; p++) {
        EEPROM.get(addr, movimento[m][p]);
        addr += sizeof(byte);
      }
    }
    Serial.print(F("Movimentos carregados da EEPROM. Total de passos: "));
    Serial.println(contador);
  } else {
    
    contador = 0;
    Serial.println(F("EEPROM vazia ou inválida. Iniciando com memoria limpa."));
  }
}


// ========================= SETUP E LOOP =========================

void setup() {
  Serial.begin(9600);
  pinMode(sensor1, INPUT_PULLUP);
  pinMode(B1, INPUT_PULLUP);
  pinMode(B2, INPUT_PULLUP);

  servog.attach(pinservog);
  servob.attach(pinservob);
  servoe.attach(pinservoe); 
  servod.attach(pinservod);

  // Carrega os movimentos salvos ANTES de mover os servos para o HOME
  carregarMovimentoEEPROM();

  // Move os servos para a posição inicial
  servog.write(0);
  servob.write(30);
  servoe.write(135);
  servod.write(10);
  delay(500);

  Serial.println(F("Sistema iniciado. Servos prontos!"));
  Serial.println(F("Aguardando comandos..."));

  pinMode(LED_GRAVANDO, OUTPUT);
  pinMode(LED_REPRODUZINDO, OUTPUT);

  // Garante que começam desligados
  digitalWrite(LED_GRAVANDO, LOW);
  digitalWrite(LED_REPRODUZINDO, LOW);
}

void loop() {

  // B1: Entrar no modo de Gravação (Usa debounce não-bloqueante apenas para o loop principal)
  if (digitalRead(B1) == HIGH && !gravando) {
    delay(200); 
    Gravarmovimento();
    tempoAnterior = millis();

  }

  // Sensor (Bandeira) ou B1 de novo: Reproduzir
  if (digitalRead(sensor1) == LOW && !gravando && !reproduzindo) {
    delay(200); 
    reproduzir_movimento();
    tempoAnterior = millis();
  }

  tempoAtual = millis();
  // Inatividade: Volta para Posição Inicial
  if (tempoAtual - tempoAnterior >= 2000 && !gravando && !reproduzindo) {
    tempoAnterior = tempoAtual;
    posicao_inicial();
  }

}


// ========================= POSIÇÃO INICIAL =========================
void posicao_inicial() {
  int velo = 30;
  for (int i = 3; i > 0 ; i--) {
    servos[i]->write(posInicial[i], velo, true);
    angulos[i] = posInicial[i]; 
  }
}

// ========================= MODO GRAVAÇÃO =========================
void Gravarmovimento() {
  gravando = true;
  digitalWrite(LED_GRAVANDO, HIGH); // LED de gravação ligado
  motorSelecionado = 0;
  contador = 0; // Zera o contador de passos
  leituraantiga = lerPotenciometroSuave();

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

  leituraantiga = lerPotenciometroSuave();

  Serial.println(F("=== MODO GRAVACAO INICIADO ==="));
  Serial.println(F("1. Use o POT para mover o motor atual."));
  Serial.println(F("2. Clique B2 para TROCAR o motor."));
  Serial.println(F("3. Clique B1 para SALVAR a posicao."));
  Serial.println(F("4. Segure B2 para SAIR e finalizar."));
  Serial.print(F("Controlando Motor: "));
  Serial.println(F("GARRA"));

  while(gravando) {
    
    // -------------------------------------------------
    // 1. Controle do Motor via POT
    // -------------------------------------------------
    
    int leituraAtual = lerPotenciometroSuave();

    int diferenca = abs(leituraAtual - leituraantiga);

    if ( diferenca > limite_histere) {
      mudou = true;
    } else {
      mudou = false;
    }

    if(mudou) {
            int angulo = MapearPotenciometro(leituraAtual);
            angulos[motorSelecionado] = angulo; 
            servos[motorSelecionado]->write(angulo, 60 , false);
    }
     delay(5); 

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
      delay(200);
      unsigned long tempoPress = millis();
      while(digitalRead(B2) == HIGH); // Espera soltar o botão

      if (millis() - tempoPress > 1000) {

        // SEGUROU POR 1 SEGUNDO -> SAIR
        gravando = false;
        Serial.print(F("Gravacao Finalizada. Total de passos salvos: "));
        Serial.println(contador);
        salvarMovimentoEEPROM();
        digitalWrite(LED_GRAVANDO, LOW); // LED de gravação desligado
        return; // Sai da função Gravarmovimento
        
      } else {

        // CLIQUE RÁPIDO -> TROCAR MOTOR
        leituraantiga = lerPotenciometroSuave();;
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
  digitalWrite(LED_REPRODUZINDO, HIGH); // LED de reprodução ligado
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

  
  Serial.println(F("Fim da reproducao."));
  reproduzindo = false;
  digitalWrite(LED_REPRODUZINDO, LOW); // LED de reprodução desligado
}

// ========================= FUNÇÕES DE UTILIDADE =========================

int lerPotenciometroSuave() {
  long soma = 0;
  for (int i = 0; i <NUM_leituras; i++) {
    soma += analogRead(pot1);
    delayMicroseconds(50); 
  }
  return (int)(soma /NUM_leituras); 
}

int MapearPotenciometro(int leitura) {
  int minLimit = limites[motorSelecionado][0];
  int maxLimit = limites[motorSelecionado][1];
  return map(leitura, 0, 1023, minLimit, maxLimit);
}
