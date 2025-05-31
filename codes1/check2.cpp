#include <iostream>
#include <RF24.h>

using namespace std;

#define PIN_CE 17
#define PIN_CSN 0

int main() {
    RF24 radio(PIN_CE, PIN_CSN);

    if (!radio.begin()) {
        cout << "ERROR: No se pudo inicializar el módulo NRF24." << endl;
        return 1;
    }

    cout << "Inicialización exitosa." << endl;

    if (radio.isPVariant()) {
        cout << "El módulo es nRF24L01+ (P variant)." << endl;
    } else {
        cout << "Módulo desconocido o no es P variant." << endl;
    }

    radio.printDetails();

    return 0;
}

