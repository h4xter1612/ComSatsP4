// receiver.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <RF24/RF24.h>
#include <cstring>
using namespace std;

RF24 radio(17, 0);
const uint8_t address[6] = "00001";
const int CHUNK_SIZE = 32;

int main() {
    radio.begin();
    radio.setPALevel(RF24_PA_LOW);
    radio.setRetries(15, 15);
    radio.setDataRate(RF24_1MBPS);
    radio.openReadingPipe(0, address);
    radio.startListening();

    // Recibir dimensiones
    uint8_t dims[4];
    while (!radio.available()) {}
    radio.read(&dims, 4);
    int width = (dims[0] << 8) | dims[1];
    int height = (dims[2] << 8) | dims[3];
    cout << "Recibiendo imagen de " << width << "x" << height << "\n";

    vector<vector<uint8_t>> image(height, vector<uint8_t>(width));
    char buffer[CHUNK_SIZE];

    for (int i = 0; i < height; ++i) {
        int bytes_received = 0;
        while (bytes_received < width) {
            while (!radio.available()) {}
            int len = min(CHUNK_SIZE, width - bytes_received);
            memset(buffer, 0, CHUNK_SIZE);
            radio.read(&buffer, len);
            for (int j = 0; j < len; ++j) {
                image[i][bytes_received + j] = static_cast<uint8_t>(buffer[j]);
            }
            bytes_received += len;
        }
        cout << "Fila " << i << " reconstruida\n";
    }

    // Guardar como .pgm
    ofstream out("recibida.pgm");
    out << "P2\n" << width << " " << height << "\n255\n";
    for (const auto& row : image) {
        for (uint8_t val : row) {
            out << to_string(val) << " ";
        }
        out << "\n";
    }
    out.close();
    cout << "✅ Guardado como recibida.pgm\n";

    // Convertir a PNG (opcional)
    int res = system("convert recibida.pgm recibida.png");
    if (res == 0) {
        cout << "✅ Convertido a recibida.png\n";
    }

    return 0;
}
