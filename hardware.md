# 🔧 Especificações Técnicas e Soldagem

## 🔌 Alimentação 9V (Padrão Pedal)

Utilize um Jack J4 com **Centro Negativo**:

- **Pino Central (-):** GND do Arduino.
- **Pino Externo (+):** VIN do Arduino.

## 🎹 Jack MIDI DIN 5 Pinos (Saída)

Vista traseira (soldagem):

- **Pino 4:** D11 do Arduino (5V via firmware).
- **Pino 2:** D10 do Arduino (GND via firmware).
- **Pino 5:** TX do Arduino (pino 1) — saída de dados MIDI.

> **Nota:** Os pinos D10 e D11 são configurados como OUTPUT pelo firmware
> para fornecer alimentação ao circuito MIDI (5V e GND respectivamente).
> O pino TX (1) envia os dados MIDI a 31250 baud.

## 🔘 Footswitches

Conecte os botões entre o **GND** e os pinos digitais **D2, D3, D4, D5**.

## ⚠️ Modo Configuração

Quando conectado ao computador via USB e usando a interface web (`config.html`),
o Arduino entra automaticamente em **modo config**, suspendendo o envio de MIDI
pelos footswitches. Ao desconectar o USB, o pedal volta a funcionar normalmente.
