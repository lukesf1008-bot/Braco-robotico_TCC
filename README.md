# M.R.P. – Manipulador Robótico Programável

O M.R.P. (Manipulador Robótico Programável) é um braço robótico educacional de 3 graus de liberdade (3-DOF), controlado por Arduino, desenvolvido para simular operações industriais de pick-and-place em ambientes de ensino técnico.

O projeto é open-source, de baixo custo e voltado ao aprendizado prático de robótica, automação industrial e controle, utilizando uma interface do tipo teach-in, que permite a gravação e reprodução de movimentos sem necessidade de reprogramação do código.

---

## Demonstração em Vídeo

▶ Demonstração completa do funcionamento do M.R.P.  
https://youtu.be/xj1kZEuCuok

---

## Objetivo do Projeto

O M.R.P. foi desenvolvido com os seguintes objetivos:

- Demonstrar conceitos fundamentais de automação e robótica industrial  
- Integrar mecânica, eletrônica e programação em um sistema funcional  
- Oferecer uma alternativa educacional a kits comerciais fechados  
- Servir como plataforma didática para experimentação e evolução por estudantes  

---

## Visão Geral de Funcionamento

O sistema permite que o operador movimente manualmente cada articulação do braço utilizando um potenciômetro. As posições desejadas são registradas pelo usuário e armazenadas na memória EEPROM do microcontrolador.

Após a gravação, o braço é capaz de reproduzir automaticamente toda a sequência salva, executando os movimentos de forma suave e ordenada, simulando ciclos repetitivos comuns em linhas de produção industrial.

O acionamento da rotina automática pode ser condicionado a um sensor infravermelho, representando a detecção de peças em um processo real.

---

## Funcionalidades Principais

- Gravação de movimentos
- Reprodução automática de sequências armazenadas  
- Persistência dos dados na EEPROM  
- Movimentos suavizados com controle de velocidade dos servomotores  
- Acionamento por sensor infravermelho  
- Interface simples baseada em botões, potenciômetro e LEDs de estado  
- Arquitetura modular e didática  

---

## Arquitetura do Sistema

O projeto é organizado em três camadas principais:

### Mecânica
Estrutura híbrida composta por peças em MDF e componentes impressos em 3D, priorizando simplicidade construtiva, rigidez estrutural e fácil manutenção.

### Eletrônica
Baseada em um Arduino Uno, com alimentação adequada para os servomotores, evitando quedas de tensão e reinicializações indesejadas do microcontrolador.

### Firmware
Desenvolvido em C++, estruturado como uma máquina de estados, alternando entre os modos de gravação e reprodução. Utiliza a biblioteca VarSpeedServo para controle suave dos movimentos.

---

## Componentes Utilizados

### Eletrônica

- Arduino Uno R3 (ou compatível)  
- 1x Servomotor MG995 (base – alto torque)  
- 3x Servomotores MG90S (ombro, cotovelo e garra)  
- Potenciômetro linear B500K  
- 2x Botões de pressão (push-buttons)  
- Sensor infravermelho de obstáculo (HW-201 ou similar)  
- 2x LEDs indicadores + resistores de 270 Ω  
- Fonte de alimentação 5 V / 3 A  

---

## Mapeamento de Pinos (Arduino Uno)

| Componente     | Pino |
|---------------|------|
| Servo Garra    | D2   |
| Botão B1       | D3   |
| Servo Base     | D4   |
| Botão B2       | D5   |
| Servo Cotovelo | D7   |
| Servo Ombro    | D8   |
| LED Reprodução | D9   |
| LED Gravação   | D10  |
| Sensor IR      | D12  |
| Potenciômetro | A0   |

---

## Modos de Operação

### Modo de Gravação (LED Azul)

- Ativação: pressionar o botão B1 em stand-by  
- Seleção de eixo: pressionar B2 (clique curto)  
- Ajuste de posição: rotação do potenciômetro  
- Registro da posição: pressionar B1  
- Finalização e gravação na EEPROM: segurar B2 por mais de 1 segundo  

### Modo de Reprodução (LED Vermelho)

- Ativação automática pelo sensor infravermelho  
- Execução da sequência salva na EEPROM  
- Movimentos realizados de forma ordenada e suavizada  

---

## Estrutura do Repositório

- Programação Final - MRP.ino – Código-fonte principal do projeto  
- docs/ – Documentação técnica (manual de uso)  
- media/ – Imagens e registros do protótipo  

---

## Como Utilizar

1. Instale a Arduino IDE  
2. Adicione a biblioteca VarSpeedServo  
3. Conecte o Arduino via USB  
4. Carregue o arquivo Programação Final - MRP.ino  
5. Alimente o sistema com fonte externa adequada  
6. Realize a calibração inicial antes do uso  

---

## Limitações Conhecidas

- Capacidade de carga limitada a aproximadamente 60 g  
- Sistema em malha aberta, sem feedback externo de posição  
- Alcance máximo aproximado de 20 cm a partir da base  
- Precisão dependente das características dos servomotores  

---

## Possíveis Melhorias Futuras

- Implementação de cinemática inversa (controle por coordenadas XYZ)  
- Comunicação sem fio (Bluetooth ou Wi-Fi)  
- Uso de encoders para realimentação de posição  
- Integração com esteira transportadora didática  
- Interface gráfica para programação visual  

---

## Contexto Educacional

Este projeto foi desenvolvido como Trabalho de Conclusão de Curso (TCC) do curso Técnico em Automação Industrial no Instituto Federal de Minas Gerais (IFMG) – Campus Itabirito.

É voltado ao uso didático em disciplinas relacionadas a robótica industrial, microcontroladores, programação e sistemas automatizados.

---

## Status do Projeto

Projeto finalizado no contexto acadêmico, funcional e validado em laboratório. Aberto a melhorias, adaptações e usos educacionais.

---

## Créditos

Autores:
- Igor Gabriel Marçal Nogueira  |  igorm4758@gmail.com
- Lucas Salles Santana Ferreira  |  lukesf1008@gmail.com
- Philipe Eduardo Faria  |  philipefariaestds@gmail.com
- Victor Martins de Souza  |  victormartinssousa23@gmail.com


---

## Licença

Este projeto é distribuído sob a licença MIT. Consulte o arquivo LICENSE para mais detalhes.
