#include <iostream>
#include <fstream>
#include <vector>
#include <RF24/RF24.h>
#include <cstring>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace std::chrono;

RF24 radio(17, 0);
const uint8_t address[6] = "00001";
const int CHUNK_SIZE = 32;

int main() {
    radio.begin();
    radio.setPALevel(RF24_PA_LOW);
    radio.setRetries(15, 15);
    radio.setDataRate(RF24_2MBPS);
    radio.openReadingPipe(0, address);
    radio.startListening();

    // Recibir dimensiones
    uint8_t dims[4];
    while (!radio.available()) {}
    radio.read(&dims, 4);
    int width = (dims[0] << 8) | dims[1];
    int height = (dims[2] << 8) | dims[3];
    cout << "🟢 Recibiendo imagen de " << width << "x" << height << "\n";

    // Inicializar matriz y log
    vector<vector<uint8_t>> image(height, vector<uint8_t>(width));
    char buffer[CHUNK_SIZE];

    ofstream log("reception_log.txt");
    log << "Fila,Bytes,Tiempo(s),Tasa(kbps)\n";

    auto total_start = high_resolution_clock::now();

    for (int i = 0; i < height; ++i) {
        int bytes_received = 0;
        auto start = high_resolution_clock::now();

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

        auto end = high_resolution_clock::now();
        double duration_s = duration_cast<microseconds>(end - start).count() / 1e6;
        double bits_received = width * 8.0;
        double kbps = (bits_received / duration_s) / 1000.0;

        // Tiempo total transcurrido
        auto now = high_resolution_clock::now();
        double elapsed_s = duration_cast<seconds>(now - total_start).count();

        // Barra de progreso
        int progress = static_cast<int>((i + 1) * 100.0 / height);
        cout << "\r📥 Recibiendo imagen: [" 
             << string(progress / 2, '=') << string(50 - progress / 2, ' ')
             << "] " << setw(3) << progress << "% | " 
             << fixed << setprecision(1) << kbps << " kbps | "
             << setprecision(0) << elapsed_s << "s" << flush;

        log << i << "," << width << "," << duration_s << "," << kbps << "\n";
    }

    auto total_end = high_resolution_clock::now();
    double total_duration = duration_cast<duration<double>>(total_end - total_start).count();

    cout << endl << "✅ Imagen recibida en " << fixed << setprecision(2) << total_duration << " segundos"
         << "\n📄 Log guardado en 'reception_log.txt'\n";

    log.close();

    // Guardar como .pgm
    ofstream out("recibida.pgm");
    out << "P2\n" << width << " " << height << "\n255\n";
    for (const auto& row : image) {
        for (uint8_t val : row) {
            out << static_cast<int>(val) << " ";
        }
        out << "\n";
    }
    out.close();
    cout << "🖼  Guardado como recibida.pgm\n";

    // Convertir a PNG
    int res = system("convert recibida.pgm recibida.png");
    if (res == 0) {
        cout << "✅ Convertido a recibida.png\n";
    }

    return 0;
}
