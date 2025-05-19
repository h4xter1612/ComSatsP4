#include <iostream>
#include <RF24/RF24.h> // Ya incluye SPI y GPIO (usa numeración BCM)
#include <unistd.h>    // Para sleep/usleep en lugar de delay

using namespace std;

#define PIN_CE 17  // chip enable (GPIO17)
#define PIN_CSN 0  // chip select not (CE0)

int main() {

  RF24 radio(PIN_CE, PIN_CSN); // Crear objeto radio

  if (!radio.begin()) {
    cout << "❌ radio.begin() FALLÓ. Verifica las conexiones." << endl;
    return 1;
  }

  // Configuración básica
  radio.setChannel(5);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_1MBPS);
  radio.setAutoAck(true);
  radio.enableDynamicPayloads();
  radio.setRetries(15, 15);

  // Abrir tubo de escritura con dirección única
  radio.openWritingPipe(0x7878787878LL);

  // ⚠️ Importante: Asegurar modo transmisión
  radio.stopListening();

  // Datos a enviar
  char text[] = "Hello world!";
  char ackData[24];

  while (true) {
    // Enviar mensaje
    if (radio.write(&text, sizeof(text))) {
      cout << "✅ Enviado: " << (int) sizeof(text) << " bytes" << endl;
    } else {
      cout << "❌ Error: Datos no entregados" << endl;
    }

    // Revisar si llegó una respuesta
    if (radio.isAckPayloadAvailable()) {
      radio.read(&ackData, sizeof(ackData));
      string FIFO;
      for (uint8_t i = 0; i < sizeof(ackData); i++) {
        FIFO += ackData[i];
      }
      cout << "📩 Recibido ACK payload: " << FIFO << endl;

      if (radio.rxFifoFull()) {
        cout << "⚠️ Buffer lleno, limpiando..." << endl;
        radio.flush_rx();
      }
    }

    sleep(1); // Esperar 1 segundo
  }

}

