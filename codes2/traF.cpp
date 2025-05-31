#include <RF24/RF24.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <unistd.h>
#include <cstring>

namespace fs = std::filesystem; // Para trabajar con carpetas y archivos

const size_t MAX_PACKET_SIZE = 32; // Tamaño máximo del payload del nrf
const size_t CCSDS_HEADER_SIZE = 6; // Tamaño del header CCSDS
const size_t MAX_PAYLOAD_SIZE = MAX_PACKET_SIZE - CCSDS_HEADER_SIZE; // Espacio restante para datos útiñes

RF24 radio(17, 0); // Pines CE y CSN (GPIO 22, CE0 para SPI)

// CCSDS headers
std::vector<uint8_t> create_ccsds_header(uint16_t seq_count, uint16_t payload_length, uint16_t apid) {
    std::vector<uint8_t> header(6);

    uint16_t version_type_apid = (1 << 13) | (0 << 12) | (0 << 11) | (apid & 0x07FF); 
    // Indica versión 1 de ccsds
    // 0 es telemetría (satélite - tierra), 1 es telecomando (tierra - satélite)
    // Sin encabezado secundario
    // Establece APID como 34 (para transmisión de imágenes de la cámara)

    uint16_t seq_flags_seq_count = (0x3 << 14) | (seq_count & 0x3FFF); // Fin de paquete / número de paquete
    uint16_t length_field = payload_length - 1;    

    header[0] = version_type_apid >> 8;
    header[1] = version_type_apid & 0xFF;
    header[2] = seq_flags_seq_count >> 8;
    header[3] = seq_flags_seq_count & 0xFF;
    header[4] = length_field >> 8;
    header[5] = length_field & 0xFF;
    return header;
}

// Función para enviar un paquete con reintentos
void send_packet(const std::vector<uint8_t>& packet) {
    while (!radio.write(packet.data(), packet.size())) {
        std::cerr << "Reintentando...\n";
        usleep(1000); // Espera 1 ms antes de reintentar
    }
    usleep(2000); // Espera entre paquetes
}

// Envia el nombre del archivo como primer paquete
void send_filename(const std::string& filename) {
std::vector<uint8_t> name_packet = create_ccsds_header(0, filename.size(), 34); // APID 34 = nombre de archivo
    name_packet.insert(name_packet.end(), filename.begin(), filename.end());
    send_packet(name_packet);
}

// Divide una imagen en paquetes CCSDS
std::vector<std::vector<uint8_t>> split_image(const std::string& path, uint16_t& seq) {
    std::ifstream file(path, std::ios::binary);
    std::vector<std::vector<uint8_t>> packets;
    if (!file) return packets;

    std::vector<uint8_t> image_data((std::istreambuf_iterator<char>(file)), {});
    size_t total = image_data.size();

    for (size_t i = 0; i < total; i += MAX_PAYLOAD_SIZE) {
        size_t len = std::min(MAX_PAYLOAD_SIZE, total - i);
        auto header = create_ccsds_header(seq++, len, 35);
        std::vector<uint8_t> packet = header;
        packet.insert(packet.end(), image_data.begin() + i, image_data.begin() + i + len);
        packets.push_back(packet);
    }

    return packets;
}

int main() {
    // Inicializa el transceptor
    radio.begin();
    radio.setPALevel(RF24_PA_LOW);
    radio.setDataRate(RF24_1MBPS); // GFSK ~ equivalente
    radio.openWritingPipe(0xF0F0F0F0D2LL); // Dirección del receptor
    radio.stopListening(); // Modo transmisión

    std::string folder = "imagenes_para_enviar"; // Carpeta de origen
    uint16_t seq = 0;

    // Recorre cada archivo en la carpeta
    for (const auto& entry : fs::directory_iterator(folder)) {
        if (!entry.is_regular_file()) continue;

        std::string path = entry.path().string();
        std::string filename = entry.path().filename().string();

        send_filename(filename); // Informa el nombre primero
        auto packets = split_image(path, seq);

        for (auto& packet : packets) {
            send_packet(packet);
        }

        std::cout << "Transmitida: " << filename << std::endl;
        usleep(50000); // Pausa entre imágenes
    }

    std::cout << "Transmisión finalizada." << std::endl;
    return 0;
}

