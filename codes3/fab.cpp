#include <RF24/RF24.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <unistd.h>
#include <cstring>
#include <chrono>

namespace fs = std::filesystem;

const size_t MAX_PACKET_SIZE = 32;
const size_t CCSDS_HEADER_SIZE = 6;
const size_t MAX_PAYLOAD_SIZE = MAX_PACKET_SIZE - CCSDS_HEADER_SIZE;

const int img_filename_apid         = 34;
const int img_data_apid             = 35;

const int fin_transmision           = 40;

const int diag_filename_apid        = 44;
const int diag_data_apid            = 45;

const int tc_filename_apid          = 54;
const int tc_data_apid              = 55;
const int mandar_tc_apid            = 58;

const int mandar_todo_apid          = 80;
const int mandar_img_apid           = 81;
const int mandar_diag_apid          = 82;

const int terminar_conexion_apid    = 100;

RF24 radio(17, 0);

std::vector<uint8_t> create_ccsds_header(uint16_t seq_count, uint16_t payload_length, uint16_t apid, uint8_t seq_flags) {
    std::vector<uint8_t> header(6);

    uint16_t version_type_apid = (apid & 0x07FF);
    uint16_t seq_flags_seq_count = ((seq_flags & 0x3) << 14) | (seq_count & 0x3FFF);
    uint16_t length_field = payload_length - 1;

    header[0] = version_type_apid >> 8;
    header[1] = version_type_apid & 0xFF;
    header[2] = seq_flags_seq_count >> 8;
    header[3] = seq_flags_seq_count & 0xFF;
    header[4] = length_field >> 8;
    header[5] = length_field & 0xFF;

    return header;
}

void send_packet(const std::vector<uint8_t>& packet) {
    while (!radio.write(packet.data(), packet.size())) {
        std::cerr << "Reintentando...\n";
        usleep(100);
    }
    usleep(100);
}

void send_control(const std::string& filename, uint16_t apid, uint8_t seq_flags) {
    std::vector<uint8_t> name_packet = create_ccsds_header(0, filename.size(), apid, seq_flags);
    name_packet.insert(name_packet.end(), filename.begin(), filename.end());
    send_packet(name_packet);
}

std::vector<std::vector<uint8_t>> split_image(const std::string& path, uint16_t& seq, uint16_t apid, uint8_t seq_flags) {
    std::ifstream file(path, std::ios::binary);
    std::vector<std::vector<uint8_t>> packets;
    if (!file) return packets;

    std::vector<uint8_t> image_data((std::istreambuf_iterator<char>(file)), {});
    size_t total = image_data.size();

    for (size_t i = 0; i < total; i += MAX_PAYLOAD_SIZE) {
        size_t len = std::min(MAX_PAYLOAD_SIZE, total - i);
        auto header = create_ccsds_header(seq++, len, apid, seq_flags);
        std::vector<uint8_t> packet = header;
        packet.insert(packet.end(), image_data.begin() + i, image_data.begin() + i + len);
        packets.push_back(packet);
    }

    return packets;
}

void transmit_to_folder(const std::string& folder, int filename_apid, int data_apid) {
    uint16_t seq = 0;
    bool archivos_enviados = false;

    for (const auto& entry : fs::directory_iterator(folder)) {
        if (!entry.is_regular_file()) continue;

        std::string path = entry.path().string();
        std::string filename = entry.path().filename().string();

        send_control(filename, filename_apid, 3);
        auto packets = split_image(path, seq, static_cast<uint16_t>(data_apid), 3);

        for (auto& packet : packets) {
            send_packet(packet);
        }

        std::cout << "Transmitida: " << filename << std::endl;
        usleep(100);
        archivos_enviados = true;
    }

    send_control(" ", 40, 3);
    if (archivos_enviados)
        std::cout << "Transmisión finalizada. Cambiar modo" << std::endl;
    else
        std::cout << "No hay archivos para enviar. Cambiar modo" << std::endl;
}

int main() {
    using clock = std::chrono::high_resolution_clock;
    std::chrono::time_point<clock> last_time;

    radio.begin();
    radio.setPALevel(RF24_PA_LOW);
    radio.setDataRate(RF24_2MBPS);

    std::string folder_tc = "telecomandos";
    fs::create_directory(folder_tc);

    std::string output_imag_dir = "imagenes_recibidas";
    fs::create_directory(output_imag_dir);

    std::string output_diag_dir = "diagnosticos_recibidos";
    fs::create_directory(output_diag_dir);

    std::ofstream output;
    std::string current_filename;
    uint8_t buffer[MAX_PACKET_SIZE];
    bool control = false;

    while (true) {
        radio.openReadingPipe(1, 0xF0F0F0F0D2LL);
        radio.startListening();

        if (radio.available()) {
            radio.read(buffer, MAX_PACKET_SIZE);

            auto now = clock::now();
            std::chrono::duration<double> elapsed = now - last_time;
            last_time = now;
            double velocidad = MAX_PACKET_SIZE / elapsed.count();

            uint16_t seq_count = ((buffer[2] & 0x3F) << 8) | buffer[3];
            uint16_t length = ((buffer[4] << 8) | buffer[5]) + 1;
            uint16_t apid = ((buffer[0] << 8) | buffer[1]) & 0x07FF;

            if (apid == mandar_tc_apid) {
                control = true;
                std::cout << "Recibiendo telecomandos: ";

                while (control) {
                    if (radio.available()) {
                        radio.read(buffer, MAX_PACKET_SIZE);
                        now = clock::now();
                        elapsed = now - last_time;
                        last_time = now;

                        velocidad = MAX_PACKET_SIZE / elapsed.count();
                        seq_count = ((buffer[2] & 0x3F) << 8) | buffer[3];
                        length = ((buffer[4] << 8) | buffer[5]) + 1;
                        apid = ((buffer[0] << 8) | buffer[1]) & 0x07FF;

                        switch (apid) {
                            case tc_filename_apid:
                                current_filename = std::string((char*)(buffer + 6), length);
                                output.close();
                                output.open(folder_tc + "/" + current_filename, std::ios::binary);
                                std::cout << "Recibiendo archivo: " << current_filename << std::endl;
                                break;

                            case tc_data_apid:
                                if (output.is_open()) {
                                    std::cout << "Paquete #" << seq_count 
                                              << " recibido | Velocidad: " << velocidad << " bytes/s" << std::endl;
                                    output.write((char*)(buffer + 6), length);
                                }
                                break;

                            case fin_transmision:
                                output.close();
                                std::cout << "Transmisión finalizada." << std::endl;
                                control = false;
                                break;

                            default:
                                std::cerr << "APID desconocido: " << apid << std::endl;
                                usleep(100);
                                break;
                        }
                    }
                }
            } else {
                radio.openWritingPipe(0xF0F0F0F0D2LL);
                radio.stopListening();

                switch (apid) {
                    case mandar_todo_apid:
                        transmit_to_folder(output_imag_dir, img_filename_apid, img_data_apid);
                        transmit_to_folder(output_diag_dir, diag_filename_apid, diag_data_apid);
                        break;
                    case mandar_img_apid:
                        transmit_to_folder(output_imag_dir, img_filename_apid, img_data_apid);
                        break;
                    case mandar_diag_apid:
                        transmit_to_folder(output_diag_dir, diag_filename_apid, diag_data_apid);
                        break;
                    case terminar_conexion_apid:
                        std::cout << "Conexión finalizada." << std::endl;
                        return 0;
                    default:
                        std::cerr << "APID desconocido: " << apid << std::endl;
                        usleep(100);
                        break;
                }
            }
        }
    }
}

