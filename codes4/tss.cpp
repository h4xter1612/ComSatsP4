#include <iostream>
#include <RF24/RF24.h>
#include <unistd.h> // usleep para microsegundos

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
  radio.setRetries(15, 15);
  radio.setAutoAck(true);
  radio.enableDynamicPayloads();
  radio.openWritingPipe(0x7878787878LL);
  radio.stopListening();

  char text[] = "Ping";

  while (true) {
    bool success = radio.write(&text, sizeof(text));
    uint8_t retries = radio.getARC(); // Número de reintentos

    if (success) {
      cout << "✅ Enviado con " << (int)retries << " reintentos" << endl;
    } else {
      cout << "❌ Falló envío tras " << (int)retries << " reintentos" << endl;
    }

    usleep(100000); // Espera 100 ms
  }

  return 0;
}

