#include <iostream>
#include <vector>
#include <fstream>
#include <RF24/RF24.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <unistd.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

RF24 radio(17, 0);
const uint8_t address[6] = "00001";
const int CHUNK_SIZE = 32;

int main() {
    // Inicializar radio
    radio.begin();
    radio.setPALevel(RF24_PA_LOW);
    radio.setRetries(15, 15);
    radio.setDataRate(RF24_2MBPS);
    radio.openWritingPipe(address);
    radio.stopListening();

    // Leer imagen
    int width, height, channels;
    uint8_t* img = stbi_load("test.png", &width, &height, &channels, 1);
    if (!img) {
        cerr << "❌ Error al cargar la imagen\n";
        return 1;
    }
    cout << "Imagen cargada: " << width << "x" << height << endl;

    // Abrir archivo de log
    ofstream log("transmission_log.txt");
    log << "Fila,Bytes,Tiempo(s),Tasa(kbps)\n";

    // Enviar dimensiones
    uint8_t dims[4] = {
        static_cast<uint8_t>(width >> 8), static_cast<uint8_t>(width & 0xFF),
        static_cast<uint8_t>(height >> 8), static_cast<uint8_t>(height & 0xFF)
    };
    radio.write(&dims, 4);
    usleep(5000);

    // Enviar filas
    for (int i = 0; i < height; ++i) {
        const uint8_t* fila = img + i * width;
        int bytes_sent = 0;

        auto start = high_resolution_clock::now();

        while (bytes_sent < width) {
            int len = min(CHUNK_SIZE, width - bytes_sent);
            bool ok = false;
            while (!ok) {
                ok = radio.write(fila + bytes_sent, len);
            }
            bytes_sent += len;
        }

        auto end = high_resolution_clock::now();
        auto duration_us = duration_cast<microseconds>(end - start).count();
        double duration_s = duration_us / 1e6;
        double bits_sent = width * 8.0;
        double kbps = (bits_sent / duration_s) / 1000.0;

        // Mostrar y guardar log
        cout << "Fila " << i << ": " << width << " bytes en "
             << duration_s << " s, ≈ " << kbps << " kbps\n";

        log << i << "," << width << "," << duration_s << "," << kbps << "\n";
    }

    stbi_image_free(img);
    log.close();

    cout << "✅ Imagen enviada\n📄 Log guardado en 'transmission_log.txt'\n";
    return 0;
}
