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

OneButton btn1(FOOT1_PIN, true);
OneButton btn2(FOOT2_PIN, true);
OneButton btn3(FOOT3_PIN, true);
OneButton btn4(FOOT4_PIN, true);

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();

  EEPROM.get(0, acoes);
  
  if(acoes[0].tipo == 255) {
    for(int i=0; i<12; i++) {
      acoes[i] = {0, 0, 127, 0};
      estadoToggle[i] = false;
    }
    EEPROM.put(0, acoes);
  }

  // Mapeamento completo dos 4 botões
  btn1.attachClick([](){ disparar(0); });
  btn1.attachDoubleClick([](){ disparar(1); });
  btn1.attachLongPressStop([](){ disparar(2); });

  btn2.attachClick([](){ disparar(3); });
  btn2.attachDoubleClick([](){ disparar(4); });
  btn2.attachLongPressStop([](){ disparar(5); });

  btn3.attachClick([](){ disparar(6); });
  btn3.attachDoubleClick([](){ disparar(7); });
  btn3.attachLongPressStop([](){ disparar(8); });

  btn4.attachClick([](){ disparar(9); });
  btn4.attachDoubleClick([](){ disparar(10); });
  btn4.attachLongPressStop([](){ disparar(11); });
}

void loop() {
  btn1.tick(); btn2.tick(); btn3.tick(); btn4.tick();

  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    if (data.startsWith("SAVE:")) {
      parseComplexConfig(data.substring(5));
    } else if (data == "READ") {
      enviarConfigParaWeb();
    }
  }
}

void disparar(int index) {
  Comando c = acoes[index];
  if (c.tipo == 0) { 
    estadoToggle[index] = !estadoToggle[index];
    byte valor = estadoToggle[index] ? c.val1 : c.val2;
    MIDI.sendControlChange(c.num, valor, 1); 
  } else { 
    MIDI.sendProgramChange(c.num, 1);
  }
}

void enviarConfigParaWeb() {
  Serial.print("DATA:");
  for(int i=0; i<12; i++) {
    Serial.print(acoes[i].tipo); Serial.print(",");
    Serial.print(acoes[i].num); Serial.print(",");
    Serial.print(acoes[i].val1); Serial.print(",");
    Serial.print(acoes[i].val2); if(i < 11) Serial.print(",");
  }
  Serial.println();
}

void parseComplexConfig(String csv) {
  int i = 0;
  char buf[250];
  csv.toCharArray(buf, 250);
  char* ptr = strtok(buf, ",");
  while(ptr != NULL && i < 12) {
    acoes[i].tipo = (byte)constrain(atoi(ptr), 0, 1);
    acoes[i].num  = (byte)constrain(atoi(strtok(NULL, ",")), 0, 127);
    acoes[i].val1 = (byte)constrain(atoi(strtok(NULL, ",")), 0, 127);
    acoes[i].val2 = (byte)constrain(atoi(strtok(NULL, ",")), 0, 127);
    i++;
    ptr = strtok(NULL, ",");
  }
  EEPROM.put(0, acoes);
}