#include <iostream>
#include <RF24/RF24.h>

using namespace std;

#define PIN_CE 17
#define PIN_CSN 0

RF24 radio(PIN_CE, PIN_CSN);

int main() {
  if (!radio.begin()) {
    cerr << "❌ NRF24 no detectado" << endl;
    return 1;
  }

  radio.setChannel(5);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.enableAckPayload();
  radio.setAutoAck(true);
  radio.enableDynamicPayloads();
  radio.openReadingPipe(0, 0x7878787878LL);
  radio.startListening();

  while (true) {
    if (radio.available()) {
      uint8_t len = radio.getDynamicPayloadSize();
      char payload[len + 1] = {0};
      radio.read(&payload, len);

      // Enviar ACK
      const char ackMsg[] = "ACK OK";
      radio.writeAckPayload(0, ackMsg, sizeof(ackMsg));
    }
  }

  return 0;
}

