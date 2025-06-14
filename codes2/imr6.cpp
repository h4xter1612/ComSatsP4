#include <iostream>
#include <fstream>
#include <vector>
#include <RF24/RF24.h>
#include <cstring>
#include <chrono>
#include <iomanip>

// Guardar imagen PNG
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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
    radio.openReadingPipe(0, address);
    radio.startListening();

    // Esperar cabecera
    uint8_t dims[5];
    cout << "⏳ Esperando cabecera...\n";
    while (!radio.available()) {}
    radio.read(dims, 5);

    int width = (dims[0] << 8) | dims[1];
    int height = (dims[2] << 8) | dims[3];
    int channels = dims[4];

    cout << "🟢 Recibiendo imagen de " << width << "x" << height
         << " con " << channels << " canales\n";

    // Crear buffer plano para imagen
    vector<uint8_t> image(width * height * channels, 0);
    char buffer[CHUNK_SIZE];

    ofstream log("reception_log.txt");
    log << "Fila,Bytes,Tiempo(s),Tasa(kbps)\n";

    auto total_start = high_resolution_clock::now();

    for (int i = 0; i < height; ++i) {
        int row_bytes = width * channels;
        int bytes_received = 0;
        auto start = high_resolution_clock::now();

        while (bytes_received < row_bytes) {
            while (!radio.available()) {}
            int len = min(CHUNK_SIZE, row_bytes - bytes_received);
            memset(buffer, 0, CHUNK_SIZE);
            radio.read(buffer, len); // ✅ CORREGIDO: no usar como bool
            memcpy(&image[i * row_bytes + bytes_received], buffer, len);
            bytes_received += len;
        }

        auto end = high_resolution_clock::now();
        double duration_s = duration_cast<microseconds>(end - start).count() / 1e6;
        double kbps = (row_bytes * 8.0 / duration_s) / 1000.0;

        auto now = high_resolution_clock::now();
        double elapsed_s = duration_cast<seconds>(now - total_start).count();

        int progress = static_cast<int>((i + 1) * 100.0 / height);
        cout << "\r📥 Recibiendo imagen: ["
             << string(progress / 2, '=') << string(50 - progress / 2, ' ')
             << "] " << setw(3) << progress << "% | "
             << fixed << setprecision(1) << kbps << " kbps | "
             << setprecision(0) << elapsed_s << "s" << flush;

        log << i << "," << row_bytes << "," << duration_s << "," << kbps << "\n";
    }

    auto total_end = high_resolution_clock::now();
    double total_duration = duration_cast<duration<double>>(total_end - total_start).count();
    cout << endl << "✅ Imagen recibida en " << fixed << setprecision(2)
         << total_duration << " segundos\n📄 Log guardado en 'reception_log.txt'\n";

    log.close();

    // Guardar imagen PNG
    int stride = width * channels;
    int result = stbi_write_png("recibida.png", width, height, channels, image.data(), stride);
    if (result) {
        cout << "🖼   Imagen guardada como recibida.png\n";
    } else {
        cerr << "❌ Error al guardar recibida.png\n";
    }

    return 0;
}

