#include <iostream>
#include <RF24/RF24.h>

using namespace std;

#define PIN_CE 17
#define PIN_CSN 0

uint8_t pipeNumber;
uint8_t payloadSize;

int main() {
  RF24 radio(PIN_CE, PIN_CSN);

  if (!radio.begin()) {
    cerr << "NRF24 no detectado. Verifica conexiones y alimentación." << endl;
    return 1;
  }

  radio.setChannel(5);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(0, 0x7878787878LL);
  radio.enableAckPayload();
  radio.setAutoAck(true);
  radio.enableDynamicPayloads();

  radio.printDetails(); // ✅ imprime velocidad y potencia ya aplicadas
  radio.startListening();

  cout << "Start listening..." << endl;

  while (true) {
    if (radio.available(&pipeNumber)) {
      payloadSize = radio.getDynamicPayloadSize();
      char payload[payloadSize + 1] = {0};

      radio.read(&payload, payloadSize);
      cout << "Pipe: " << (int)pipeNumber
           << " | Size: " << (int)payloadSize
           << " | Data: " << payload << endl;

      const char ackData[] = "Data from buffer";
      radio.writeAckPayload(0, &ackData, sizeof(ackData));
    }
  }

  return 0;
}

