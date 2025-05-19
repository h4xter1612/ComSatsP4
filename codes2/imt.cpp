// transmitter.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <RF24/RF24.h>
#include <unistd.h>
using namespace std;

RF24 radio(17, 0);
const uint8_t address[6] = "00001";

vector<string> leer_pgm_lineas(const string& filename) {
    ifstream file(filename);
    string line;
    vector<string> output_lines;
    int width, height, maxval;

    getline(file, line); // P2
    while (getline(file, line) && (line[0] == '#')); // skip comments
    stringstream(line) >> width >> height;
    getline(file, line); // maxval

    vector<int> pixels;
    while (file >> line) pixels.push_back(stoi(line));

    for (int i = 0; i < height; i++) {
        string fila = "";
        for (int j = 0; j < width; j++) {
            fila += to_string(pixels[i * width + j]) + ",";
        }
        fila.pop_back();
        output_lines.push_back(fila);
    }

    return output_lines;
}

int main() {
    radio.begin();
    radio.setPALevel(RF24_PA_LOW);
    radio.setRetries(15, 15);
    radio.setDataRate(RF24_1MBPS);
    radio.openWritingPipe(address);
    radio.stopListening();

    auto filas = leer_pgm_lineas("imagen.pgm");

    for (size_t i = 0; i < filas.size(); i++) {
        bool delivered = false;
        while (!delivered) {
            delivered = radio.write(filas[i].c_str(), filas[i].size() + 1);
            if (!delivered) {
                cout << "Reintentando línea " << i << endl;
                usleep(5000);
            }
        }
        cout << "Línea " << i << " enviada\n";
        usleep(10000);
    }

    const char* fin = "FIN";
    radio.write(fin, strlen(fin) + 1);

    cout << "Imagen enviada\n";
    return 0;
}
