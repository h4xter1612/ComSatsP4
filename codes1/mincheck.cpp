#include <RF24/RF24.h>
#include <iostream>

using namespace std;

RF24 radio(17, 0); // CE = GPIO17, CSN = CE0

int main() {
    if (!radio.begin()) {
        cout << "❌ radio.begin() FALLÓ. Módulo no responde." << endl;
        return 1;
    }
    cout << "✅ radio.begin() OK." << endl;
    radio.printDetails();
    return 0;
}

