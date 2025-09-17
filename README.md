<h1 align="center">Estacionamento Inteligente</h1>
<h3 align="center">Projeto de IoT</h3>

---

## Introdução
O projeto **Estacionamento Inteligente** tem como objetivo otimizar a experiência de estacionar veículos por meio de:
- Maior facilidade de locomoção;  
- Otimização do tempo;  
- Monitoramento em tempo real das vagas, tanto via **Website** quanto fisicamente;  
- Aumento da segurança.  

O sistema também conta com **motor com controle de posição** para o gerenciamento da entrada e saída.

---

## Componentes Utilizados
- **ESP32** → Microcontrolador com Wi-Fi e Bluetooth integrados.  
- **Sensores a Laser VL53L0X** → Sensores de distância a laser.  
- **Servos Motores SG90** → Controle de barreiras físicas.  
- **Multiplexador TCA9548A** → Gerencia múltiplos dispositivos I2C no mesmo barramento.  
- **LEDs RGB** → Indicação visual das vagas.  
- **Botões** → Dispositivos de entrada digital.  

---

## Funcionamento
- O sistema identifica a ocupação das vagas utilizando **sensores a laser**.  
- LEDs RGB informam a disponibilidade (livre/ocupada).  
- O ESP32 processa as informações e atualiza o **website de monitoramento**.  
- Servos controlam barreiras de entrada e saída de veículos.  
