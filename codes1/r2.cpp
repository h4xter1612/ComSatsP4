#include <iostream>
#include "../RF24/RF24.h"

using namespace std;

#define PIN_CE  17
#define PIN_CSN 0

const uint64_t address = 0xF0F0F0F0D2LL;
uint8_t pipeNumber;
uint8_t payloadSize;

int main() {
  RF24 radio(PIN_CE, PIN_CSN);

  radio.begin();
  radio.setChannel(5);                    // Mismo canal que transmisor
  radio.setDataRate(RF24_1MBPS);         // Mismo Data Rate
  radio.setPALevel(RF24_PA_HIGH);
  radio.setAutoAck(true);     // Transmisor
  radio.enableDynamicPayloads();

  radio.openReadingPipe(0, address);     // Escuchar en pipe 1

  radio.startListening();
  radio.printDetails();

  cout << "Start listening..." << endl;

  while (true) {
    if (radio.available(&pipeNumber)) {
      payloadSize = radio.getDynamicPayloadSize();
      char payload[payloadSize + 1];     // +1 para null-terminator
      radio.read(&payload, payloadSize);
      payload[payloadSize] = '\0';       // Asegura que sea string

      cout << "Pipe : " << (int) pipeNumber
           << " | Size : " << (int) payloadSize
           << " | Data : " << payload << endl;

      delay(100);
    }
  }

  return 0;
}
