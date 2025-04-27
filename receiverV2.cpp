#include <iostream>
#include <chrono>  // For timing
//#include <RF24/nRF24L01.h>
#include <RF24/RF24.h> // SPI and GPIO work (BCM numbering) already implemented in the library

using namespace std;
using namespace chrono;

#define PIN_CE 17 // (chip enable)
#define PIN_CSN 0 // (chip select not)

uint8_t pipeNumber;
uint8_t payloadSize;

int main() {

    RF24 radio(PIN_CE, PIN_CSN); // create radio object
    radio.begin();
    radio.setChannel(5); // data transmission channel (0 to 125), 5 -> 2.405 GHz
    radio.setPALevel(RF24_PA_HIGH); // RF power level
    radio.setDataRate(RF24_1MBPS); // data rate
    radio.openReadingPipe(0, 0x7878787878LL); // open pipe 0

    radio.enableAckPayload(); // enable custom ack payloads
    radio.setAutoAck(true); // enable auto acknowledgment
    radio.enableDynamicPayloads(); // allow dynamic payload size

    radio.printDetails();
    radio.startListening(); // start listening

    cout << "Start listening..." << endl;

    auto last_receive_time = high_resolution_clock::now(); // initialize last receive time

    while (true) {
        if (radio.available(&pipeNumber)) {

            payloadSize = radio.getDynamicPayloadSize();
            char payload[payloadSize];
            string receivedData;

            radio.read(&payload, payloadSize);

            for (uint8_t i = 0; i < payloadSize; i++) {
                receivedData += payload[i];
            }

            // Measure current time and compute elapsed time
            auto current_time = high_resolution_clock::now();
            duration<double> time_span = duration_cast<duration<double>>(current_time - last_receive_time);
            last_receive_time = current_time; // update last reception time

            // Print everything in the same line
            cout << "Pipe number: " << (int)pipeNumber << " ";
            cout << "Payload size: " << (int)payloadSize << " ";
            cout << "Data: " << receivedData << " ";
            cout << "Time since last packet: " << time_span.count() << " seconds" << endl;

            // Send ACK payload
            char ackData[] = "Data from buffer";
            radio.writeAckPayload(0, &ackData, sizeof(ackData));
        }
    }
    return 0;
}
