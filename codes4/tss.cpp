#include <iostream>
#include <RF24/RF24.h>
#include <unistd.h>   // usleep
#include <iomanip>    // setw
using namespace std;

#define PIN_CE 17
#define PIN_CSN 0

RF24 radio(PIN_CE, PIN_CSN);

// Función para mapear reintentos a "intensidad de señal"
int signalStrengthFromRetries(uint8_t retries) {
  if (retries == 0) return 100;
  else if (retries <= 2) return 90;
  else if (retries <= 4) return 75;
  else if (retries <= 6) return 60;
  else if (retries <= 9) return 40;
  else if (retries <= 12) return 20;
  else if (retries <= 15) return 5;
  else return 0;
}

void printSignalBar(int strength) {
  int barLength = strength / 5; // Máximo 20 caracteres
  cout << "\r📶 Intensidad de señal: [";
  for (int i = 0; i < 20; i++) {
    if (i < barLength)
      cout << "#";
    else
      cout << "-";
  }
  cout << "] " << setw(3) << strength << "%" << flush;
}

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
    uint8_t retries = radio.getARC();

    int signalStrength = success ? signalStrengthFromRetries(retries) : 0;
    printSignalBar(signalStrength);

    usleep(100000); // 100 ms
  }

  return 0;
}

