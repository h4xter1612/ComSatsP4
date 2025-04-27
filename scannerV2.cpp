// ISM band frequency scanner (from 2400 MHz to 2527 MHz)
#include <iostream>
#include <chrono>  // For timing
// #include <RF24/nRF24L01.h>
#include <RF24/RF24.h>

using namespace std;
using namespace chrono;  // For easier access to chrono types

#define PIN_CE 17
#define PIN_CSN 0
#define NUM_CHANNELS 126

uint8_t values[NUM_CHANNELS];
const int num_reps = 100;

int main() {

    RF24 radio(PIN_CE, PIN_CSN);
    radio.begin();
    radio.setAutoAck(false); // disable automatic transmission of acknowledgment packets for all pipes

    radio.startListening(); // enter receiver mode
    radio.stopListening();

    radio.printDetails(); // print configuration details

    // Variables to measure time between packets
    auto last_detection_time = high_resolution_clock::now();

    // Print upper header
    for (int i = 0; i < NUM_CHANNELS; ++i) {
        cout << hex << (i >> 4);
    }
    cout << endl;

    // Print lower header
    for (int i = 0; i < NUM_CHANNELS; ++i) {
        cout << hex << (i & 0xf);
    }
    cout << endl;

    while (true) {

        memset(values, 0, sizeof(values)); // reset all channel values to zero

        // Scan all channels num_reps times
        for (int k = 0; k < num_reps; ++k) {
            for (int i = 0; i < NUM_CHANNELS; ++i) {

                radio.setChannel(i);

                radio.startListening();
                delayMicroseconds(130); // switching time between module channels is 130 μs
                radio.stopListening();

                // Check for the presence of a carrier frequency on the selected channel (frequency)
                if (radio.testCarrier()) {
                    ++values[i];

                    // Measure time since last detection
                    auto current_time = high_resolution_clock::now();
                    duration<double> time_span = duration_cast<duration<double>>(current_time - last_detection_time);
                    cout << "Time since last packet: " << time_span.count() << " seconds" << endl;

                    last_detection_time = current_time; // Update last detection time
                }
            }
        }

        // Print channel measurements as a single hexadecimal digit
        for (int i = 0; i < NUM_CHANNELS; ++i) {
            cout << hex << min(0xf, (values[i] & 0xf));
        }
        cout << endl;
    }
    return 0;
}
