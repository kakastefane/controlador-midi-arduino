# 🔧 Especificações Técnicas e Soldagem

## 🔌 Alimentação 9V (Padrão Pedal)
Utilize um Jack J4 com **Centro Negativo**:
- **Pino Central (-):** GND do Arduino.
- **Pino Externo (+):** VIN do Arduino.

## 🎹 Jack MIDI DIN 5 Pinos (Saída)
Vista traseira (soldagem):
- **Pino 4:** 5V do Arduino (via resistor 220Ω).
- **Pino 2:** GND do Arduino (Blindagem).
- **Pino 5:** TX do Arduino (via resistor 220Ω).

## 🔘 Footswitches
Conecte os botões entre o **GND** e os pinos digitais **D2, D3, D4, D5**.
