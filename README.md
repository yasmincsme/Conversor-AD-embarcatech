<div align="center">
    <img src="https://moodle.embarcatech.cepedi.org.br/pluginfile.php/1/theme_moove/logo/1733422525/Group%20658.png" alt="Logo Embarcatech" height="100">
</div>

<br>


<div align="center">

# Conversores A/D

</div>

## Descrição
O projeto utiliza um joystick para fornecer valores analógicos correspondentes aos eixos X e Y, que serão utilizados para controlar os leds da placa BitDogLab via PWM e também o Display SSD1306.

## Funcionalidades
- **Controle dos LEDs RGB via PWM:** Os leds azul e vermelho devem ter seu brilho controlado via PWM para permitir variação suave da intensidade luminosa;
- **Representação Gráfica no Display SSD1306:** Um quadrado de 8x8 pixels deve ser desenhado no display e se moverá proporcionalmente aos valores capturados pelo joystick. O protocolo I2C será utilizado para a comunicação com o display;
- **Funcionalidades dos Botões:** O botão do Joystick deve alternar o estado do LED Verde a cada acionamento e modificar a borda do display.

## Requisitos
- **Uso de Interrupções (IRO):** Todas as funcionalidades relacionadas aos botões serão implementadas utilizando interrupções.;
- **Debouncing dos Botões:** Implementação de debouncing via software para evitar leituras incorretas.;
- **Utilização do Display 128x64:** Uso de ferramentas gráficas para demonstrar o entendimento do display e o protocolo I2C;
- **Organização do Código:** Código bem estruturado e comentado para facilitar o entendimento.

## Testes

1. **Controle do LED Azul e do LED Vermelho pelos eixos X e Y do Joystick:** Verificar se o LED Azul e o LED Vermelho respondem corretamente ao movimento nos eixos X e Y.

<div align="center">

![Demostração no Simulador](docs/leds_vermelho_azul.gif)

</div>

2. **Movimentação do Quadrado no Display:** Garantir que o quadrado se move corretamente conforme o joystick.

<div align="center">

![Demostração no Simulador](docs/display.gif)

</div>

3. **Alternância do LED Verde e da borda do display pelo botão do Joystick:** Testar se o botão do joystick liga e desliga o LED Verde corretamente e altera a borda do display.

<div align="center">

![Demostração no Simulador](docs/led_verde.gif)

</div>

4. **Ativação/Desativação dos LEDs RGB pelo botão A:** Garantir que os LEDs RGB possam ser ativados ou desativados.

<div align="center">

![Demostração no Simulador](docs/pwm(1).gif)

</div>

## Instruções de Uso

1. **Clonar o Repositório**:

```bash
git clone https://github.com/yasmincsme/embarcatech-U4C4-Interrupcoes.git
```

2. **Compilar e Carregar o Código**:
   No VS Code, configure o ambiente e compile o projeto com os comandos:

```bash	
cmake -G Ninja ..
ninja
```

3. **Interação com a Placa**:
   - Conecte a placa ao computador.
   - Clique em run usando a extensão do raspberry pi pico.

## Vídeo de Demonstração

[Clique aqui](https://youtu.be/UFglGplZERQ)

---
### Autora:
Yasmin Cordeiro de Souza Meira