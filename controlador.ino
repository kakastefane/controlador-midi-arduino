#include <MIDI.h>
#include <OneButton.h>
#include <EEPROM.h>

MIDI_CREATE_DEFAULT_INSTANCE();

#define FOOT1_PIN 2
#define FOOT2_PIN 3
#define FOOT3_PIN 4
#define FOOT4_PIN 5

struct Comando {
  byte tipo; 
  byte num;  
  byte val1; 
  byte val2; 
};

Comando acoes[12];
bool estadoToggle[12];
bool modoConfig = false;
byte modos[4]; // bits: 0=doubleClick, 1=hold (3=ambos habilitados)

OneButton btn1(FOOT1_PIN, true);
OneButton btn2(FOOT2_PIN, true);
OneButton btn3(FOOT3_PIN, true);
OneButton btn4(FOOT4_PIN, true);

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();

  // Alimentação do conector MIDI DIN
  pinMode(11, OUTPUT); digitalWrite(11, HIGH); // 5V MIDI
  pinMode(10, OUTPUT); digitalWrite(10, LOW);  // GND MIDI

  EEPROM.get(0, acoes);
  EEPROM.get(sizeof(acoes), modos);
  
  if(acoes[0].tipo == 255) {
    for(int i=0; i<12; i++) {
      acoes[i] = {0, 0, 127, 0};
      estadoToggle[i] = false;
    }
    EEPROM.put(0, acoes);
  }

  if(modos[0] == 255) {
    for(int i=0; i<4; i++) modos[i] = 3; // Default: ambos habilitados
    EEPROM.put(sizeof(acoes), modos);
  }

  // Clique simples sempre ativo
  btn1.attachClick([](){ disparar(0); });
  btn2.attachClick([](){ disparar(3); });
  btn3.attachClick([](){ disparar(6); });
  btn4.attachClick([](){ disparar(9); });

  aplicarModos();
}

void loop() {
  btn1.tick(); btn2.tick(); btn3.tick(); btn4.tick();

  if (Serial.available() > 0) {
    // Sem delay! O buffer do Nano tem apenas 64 bytes.
    // readStringUntil já espera até '\n' (timeout padrão 1s).
    String data = Serial.readStringUntil('\n');
    data.trim();

    if (data == "CONFIG_ON") {
      modoConfig = true;
      Serial.println("OK");
    } else if (data == "CONFIG_OFF") {
      modoConfig = false;
      Serial.println("OK");
    } else if (data == "READ") {
      modoConfig = true; // Ativa modo config automaticamente
      enviarConfigParaWeb();
    } else if (data.startsWith("SAVE:")) {
      parseComplexConfig(data.substring(5));
      Serial.println("CONFIRMED");
    }
  }
}

void disparar(int index) {
  if (modoConfig) return; // Não envia MIDI durante configuração via web
  
  Comando c = acoes[index];
  if (c.tipo == 0) { 
    estadoToggle[index] = !estadoToggle[index];
    byte valor = estadoToggle[index] ? c.val1 : c.val2;
    MIDI.sendControlChange(c.num, valor, 1); 
  } else { 
    MIDI.sendProgramChange(c.num, 1);
  }
}

void aplicarModos() {
  // Foot 1
  if(modos[0] & 1) btn1.attachDoubleClick([](){ disparar(1); });
  else btn1.attachDoubleClick(NULL);
  if(modos[0] & 2) btn1.attachLongPressStop([](){ disparar(2); });
  else btn1.attachLongPressStop(NULL);

  // Foot 2
  if(modos[1] & 1) btn2.attachDoubleClick([](){ disparar(4); });
  else btn2.attachDoubleClick(NULL);
  if(modos[1] & 2) btn2.attachLongPressStop([](){ disparar(5); });
  else btn2.attachLongPressStop(NULL);

  // Foot 3
  if(modos[2] & 1) btn3.attachDoubleClick([](){ disparar(7); });
  else btn3.attachDoubleClick(NULL);
  if(modos[2] & 2) btn3.attachLongPressStop([](){ disparar(8); });
  else btn3.attachLongPressStop(NULL);

  // Foot 4
  if(modos[3] & 1) btn4.attachDoubleClick([](){ disparar(10); });
  else btn4.attachDoubleClick(NULL);
  if(modos[3] & 2) btn4.attachLongPressStop([](){ disparar(11); });
  else btn4.attachLongPressStop(NULL);
}

void enviarConfigParaWeb() {
  Serial.print("DATA:");
  for(int i=0; i<12; i++) {
    Serial.print(acoes[i].tipo); Serial.print(",");
    Serial.print(acoes[i].num); Serial.print(",");
    Serial.print(acoes[i].val1); Serial.print(",");
    Serial.print(acoes[i].val2); Serial.print(",");
  }
  // Modos dos 4 footswitches
  for(int i=0; i<4; i++) {
    Serial.print(modos[i]);
    if(i < 3) Serial.print(",");
  }
  Serial.println();
}

void parseComplexConfig(String csv) {
  int i = 0;
  char buf[300]; // Aumentado para segurança
  csv.toCharArray(buf, 300);
  
  char* ptr = strtok(buf, ",");
  while(ptr != NULL && i < 12) {
    acoes[i].tipo = (byte)constrain(atoi(ptr), 0, 1);
    
    ptr = strtok(NULL, ",");
    if(ptr) acoes[i].num = (byte)constrain(atoi(ptr), 0, 127);
    
    ptr = strtok(NULL, ",");
    if(ptr) acoes[i].val1 = (byte)constrain(atoi(ptr), 0, 127);
    
    ptr = strtok(NULL, ",");
    if(ptr) acoes[i].val2 = (byte)constrain(atoi(ptr), 0, 127);
    
    i++;
    ptr = strtok(NULL, ",");
  }
  
  // Parse modos (4 bytes após as 12 ações)
  for(int j=0; j<4; j++) {
    if(ptr) {
      modos[j] = (byte)constrain(atoi(ptr), 0, 3);
      ptr = strtok(NULL, ",");
    }
  }
  
  // GRAVAÇÃO FÍSICA NA MEMÓRIA
  EEPROM.put(0, acoes);
  EEPROM.put(sizeof(acoes), modos);
  aplicarModos();
  
  for(int j=0; j<12; j++) estadoToggle[j] = false;
}
