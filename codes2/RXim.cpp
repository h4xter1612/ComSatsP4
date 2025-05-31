#include <RF24/RF24.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <unistd.h>
#include <cstring>

namespace fs = std::filesystem;

const size_t MAX_PACKET_SIZE = 32;

RF24 radio(17, 0); // Pines CE y CSN

int main() {
    // Inicializa transceptor
    radio.begin();
    radio.setPALevel(RF24_PA_LOW);
    radio.setDataRate(RF24_2MBPS);
    radio.openReadingPipe(1, 0xF0F0F0F0D2LL); // Dirección para recibir
    radio.startListening(); // Modo escucha

    std::string output_dir = "imagenes_recibidas";
    fs::create_directory(output_dir); // Crea la carpeta si no existe

    std::ofstream output;
    std::string current_filename;
    uint8_t buffer[MAX_PACKET_SIZE];
    
    std::cout << "Empezando recepción. " << current_filename << std::endl;

    while (true) {
        if (radio.available()) {
            radio.read(buffer, MAX_PACKET_SIZE);

            // Extrae número de secuencia
            uint16_t seq_count = ((buffer[2] & 0x3F) << 8) | buffer[3];
            uint16_t length = (buffer[4] << 8) | buffer[5];
            length += 1;
            
            uint16_t apid = ((buffer[0] << 8) | buffer[1]) & 0x07FF; // Extrae el apid

            if (apid == 34) {
                // Este paquete es un nombre de archivo
                current_filename = std::string((char*)(buffer + 6), length);
                output.close();
                output.open(output_dir + "/" + current_filename, std::ios::binary);
                std::cout << "Recibiendo imagen: " << current_filename << std::endl;
            } else if (apid == 35) {
                // Paquete de datos de imagen
                output.write((char*)(buffer + 6), length);
                std::cout << "Paquete recibido. " << current_filename << std::endl;
            }
        } else {
            usleep(100); // Espera si no hay datos
            std::cout << "APID desconocido. " << current_filename << std::endl;
        }
    }
    std::cout << "Imagen recibida completamente. " << current_filename << std::endl;
    output.close();
    return 0;
}

