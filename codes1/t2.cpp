#include <iostream>
#include "../RF24/RF24.h"

using namespace std;

#define PIN_CE  17
#define PIN_CSN 0

const uint64_t address = 0xF0F0F0F0D2LL;

int main() {
  RF24 radio(PIN_CE, PIN_CSN);

  radio.begin();
  radio.setChannel(5);                    // Mismo canal que receptor
  radio.setDataRate(RF24_1MBPS);         // Mismo Data Rate
  radio.setPALevel(RF24_PA_HIGH);        // Nivel de potencia
  radio.enableDynamicPayloads();         // Activa payloads dinámicos

  radio.openWritingPipe(address);        // Dirección a la que se envía

  char text[] = "Hello world";

  while (true) {
    if (radio.write(&text, sizeof(text))) {
      cout << "Delivered " << sizeof(text) << " bytes" << endl;
    } else {
      cout << "Data not delivered" << endl;
    }
    delay(1000);
  }

  return 0;
}
