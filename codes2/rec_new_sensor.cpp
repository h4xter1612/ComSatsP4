#include <iostream>
#include <vector>
#include <fstream>
#include <RF24/RF24.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <unistd.h>
#include <chrono>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;
using namespace std::chrono;

RF24 radio(17, 0);
const uint8_t address[6] = "00001";
const int CHUNK_SIZE = 32;

int main() {
    // Leer nombre del usuario
    char name[32];
    printf("Escriba su nombre: \n");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;  // Eliminar salto de línea

    printf("Presiona Enter para tomar la foto\n");
    getchar();

    // Fork para ejecutar libcamera-still
    pid_t pid = fork();
    if (pid == 0) {
	    const char* args[] = {
		    "libcamera-still",
		    "-o", "/home/MisthycPi5/reto_antenna/image_expo.png",
		    "--width", "1280",
		    "--height", "720",
		    "--immediate",  // inmediate
		    NULL
	    };
	    execvp("libcamera-still", (char* const*)args);
	    perror("Error al ejecutar libcamera-still");
	    exit(EXIT_FAILURE);
    } else if (pid > 0) {
	    waitpid(pid, NULL, 0);
	    printf("📸 Foto tomada y guardada como image_expo.png\n");
    } else {
	    perror("Error al hacer fork");
	    return EXIT_FAILURE;
    }
    // Inicializar radio
    radio.begin();
    radio.setPALevel(RF24_PA_LOW);
    radio.setRetries(15, 15);
    radio.setDataRate(RF24_2MBPS);
    radio.openWritingPipe(address);
    radio.stopListening();

    // Leer imagen
    int width, height, channels;
    uint8_t* img = stbi_load("/home/MisthycPi5/reto_antenna/image_expo.png", &width, &height, &channels, 1);
    if (!img) {
        cerr << "❌ Error al cargar la imagen\n";
        return 1;
    }
    cout << "🖼️  Imagen cargada: " << width << "x" << height << endl;

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

    // Inicia cronómetro total
    auto total_start = high_resolution_clock::now();

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

        // Tiempo transcurrido total
        auto now = high_resolution_clock::now();
        double elapsed_s = duration_cast<seconds>(now - total_start).count();

        // Barra de progreso dinámica
        int progress = static_cast<int>((i + 1) * 100.0 / height);
        cout << "\r🔄 Enviando imagen: ["
             << string(progress / 2, '=') << string(50 - progress / 2, ' ')
             << "] " << setw(3) << progress << "% | "
             << fixed << setprecision(1) << kbps << " kbps | "
             << setprecision(0) << elapsed_s << "s" << flush;

        log << i << "," << width << "," << duration_s << "," << kbps << "\n";
    }

    auto total_end = high_resolution_clock::now();
    double total_duration = duration_cast<duration<double>>(total_end - total_start).count();

    stbi_image_free(img);
    log.close();

    cout << endl << "✅ Imagen enviada en " << fixed << setprecision(2) << total_duration << " segundos"
         << "\n📄 Log guardado en 'transmission_log.txt'\n";

    // ➕ Enviar mensaje final con el nombre del usuario
    char mensaje[64];
    snprintf(mensaje, sizeof(mensaje), "Hola, %s. Esperemos que les haya sido de su agrado.", name);
    radio.write(mensaje, strlen(mensaje) + 1);
    usleep(5000);

    cout << "📨 Mensaje enviado: " << mensaje << endl;

    return 0;
}

