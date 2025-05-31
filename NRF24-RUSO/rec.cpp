#include <iostream>
#include <RF24/RF24.h>

using namespace std;

#define PIN_CE 17
#define PIN_CSN 0

RF24 radio(PIN_CE, PIN_CSN);

int main() {
  radio.begin();
  radio.setChannel(5);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_1MBPS);
  radio.setAutoAck(true);  // solo auto ack simple
  radio.setRetries(15, 15); // no afecta receptor, pero por si se usa en TX

  radio.openReadingPipe(0, 0x7878787878LL); // dirección de recepción
  radio.setPayloadSize(32); // ❗ usamos payload fijo de 32 bytes
  radio.startListening();

  radio.printDetails();
  cout << "Start listening..." << endl;

  while (true) {
    if (radio.available()) {
      char text[32] = {0};
      radio.read(&text, sizeof(text));
      cout << "Received: " << text << endl;
    }
  }
}
