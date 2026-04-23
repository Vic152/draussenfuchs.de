/*
  GPS-Synchronized 5-Fox ARDF Beacon
  Platform: ESP32-C3 Super Mini
  Hardware: NEO-M8N GPS (at 115200 baud, powered continuously)
*/

#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

// ==============================================================================
// 1. FOX CONFIGURATION (CHANGE THIS FOR EACH BOARD)
// ==============================================================================
// Set this to 1, 2, 3, 4, or 5 before uploading to the respective ESP32!
#define FOX_ID 1

// ==============================================================================
// 2. TIMING & HARDWARE CONSTANTS
// ==============================================================================
#define BEACON_PERIOD_MS 300000ULL      // 5 minutes (300,000 ms) total rotation cycle
#define BEACON_DURATION_US 60000000ULL  // 60 seconds perfectly padded transmission
#define WPM 10                          // Morse speed (words per minute)
#define DIT_DURATION_MS (1200 / WPM)    // Standard timing math
#define FREQ_HZ 450                    // Pitch of the tone

#define DATA_PIN 3               // RF Transmitter Data
#define TX_ARTIFICIAL_VCC_PIN 4  // RF Transmitter Power (DO NOT USE FOR GPS)
#define GPS_RX_PIN 20            // Connect to GPS TX
#define GPS_BAUD 9600            // Super-fast baud rate for low latency

TinyGPSPlus gps;
HardwareSerial GPS_Serial(1);

// ==============================================================================
// 3. MORSE CODE DICTIONARY
// ==============================================================================
const char* morseFor(char c) {
  if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
  switch (c) {
    case 'A': return ".-";
    case 'B': return "-...";
    case 'C': return "-.-.";
    case 'D': return "-..";
    case 'E': return ".";
    case 'F': return "..-.";
    case 'G': return "--.";
    case 'H': return "....";
    case 'I': return "..";
    case 'J': return ".---";
    case 'K': return "-.-";
    case 'L': return ".-..";
    case 'M': return "--";
    case 'N': return "-.";
    case 'O': return "---";
    case 'P': return ".--.";
    case 'Q': return "--.-";
    case 'R': return ".-.";
    case 'S': return "...";
    case 'T': return "-";
    case 'U': return "..-";
    case 'V': return "...-";
    case 'W': return ".--";
    case 'X': return "-..-";
    case 'Y': return "-.--";
    case 'Z': return "--..";
    case '0': return "-----";
    case '1': return ".----";
    case '2': return "..---";
    case '3': return "...--";
    case '4': return "....-";
    case '5': return ".....";
    case '6': return "-....";
    case '7': return "--...";
    case '8': return "---..";
    case '9': return "----.";
    default: return "";
  }
}

// Sends a single string as properly timed Morse Code
void sendMorse(const char* msg) {
  for (int i = 0; msg[i] != '\0'; i++) {
    char c = msg[i];
    if (c == ' ') {
      delay(4 * DIT_DURATION_MS);  // Word gap
      continue;
    }
    const char* code = morseFor(c);
    if (code[0] == '\0') continue;

    for (int j = 0; code[j] != '\0'; j++) {
      tone(DATA_PIN, FREQ_HZ);
      digitalWrite(LED_BUILTIN, LOW);  // Turn LED ON (Active-Low)

      int duration = (code[j] == '-') ? (3 * DIT_DURATION_MS) : DIT_DURATION_MS;
      delay(duration);

      noTone(DATA_PIN);
      digitalWrite(LED_BUILTIN, HIGH);  // Turn LED OFF (Active-Low)

      if (code[j + 1] != '\0') delay(DIT_DURATION_MS);  // Intra-character gap
    }
    delay(3 * DIT_DURATION_MS);  // Inter-character gap
  }
}


// ==============================================================================
// 4. THE 60-SECOND PADDED TRANSMISSION FUNCTION
// ==============================================================================
void transmit_beacon(const char* beacon_code) {
  unsigned long startTime = micros();
  unsigned long singleTxDuration = 0;
  bool firstRun = true;

  // Turn on the RF transmitter
  digitalWrite(TX_ARTIFICIAL_VCC_PIN, HIGH);

  // --- PHASE 1: LOOPING MORSE ID ---
  while (true) {
    unsigned long elapsed = micros() - startTime;

    // Look-ahead: Do we have time for another Morse sequence before 60s is up?
    if (!firstRun) {
      if ((BEACON_DURATION_US - elapsed) < singleTxDuration) {
        break;  // Exit loop, start continuous tone
      }
    }

    unsigned long txStart = micros();
    sendMorse(beacon_code);
    delay(7 * DIT_DURATION_MS);  // Gap before repeating

    if (firstRun) {
      singleTxDuration = micros() - txStart;
      firstRun = false;
    }
  }

  // --- PHASE 2: CONTINUOUS TONE PADDING ---
  // --- PHASE 2: CONTINUOUS TONE PADDING ---
  unsigned long finalElapsed = micros() - startTime;
  if (BEACON_DURATION_US > finalElapsed) {
    unsigned long padding_us = BEACON_DURATION_US - finalElapsed;

    tone(DATA_PIN, FREQ_HZ);
    digitalWrite(LED_BUILTIN, LOW);  // <--- ADDED: LED ON solid for the long tone!

    if (padding_us > 2000) {
      delay(padding_us / 1000);
    }

    // Busy-wait the final microseconds to hit 60.000 seconds perfectly
    while (micros() - startTime < BEACON_DURATION_US) {}

    noTone(DATA_PIN);
    digitalWrite(LED_BUILTIN, HIGH);  // <--- ADDED: LED OFF when time is up
  }

  // Turn off the RF transmitter
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(TX_ARTIFICIAL_VCC_PIN, LOW);
}

// ==============================================================================
// 5. MAIN LOGIC (WAKE, SYNC, TRANSMIT, SLEEP)
// ==============================================================================
void setup() {
  pinMode(TX_ARTIFICIAL_VCC_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(TX_ARTIFICIAL_VCC_PIN, LOW);  // Ensure TX is off initially
  digitalWrite(LED_BUILTIN, HIGH);           // LED off (active low)

  GPS_Serial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, -1);

  // 1. Calculate the target offset for this specific FOX_ID
  // Fox 1 = 0s, Fox 2 = 60s, Fox 3 = 120s, Fox 4 = 180s, Fox 5 = 240s
  uint32_t period_sec = BEACON_PERIOD_MS / 1000;
  uint32_t offset_sec = (FOX_ID - 1) * 60;

  // 2. Select the official Morse Callsign based on FOX_ID
  const char* my_callsign = "UUU";  // Default fallback
  if (FOX_ID == 1) my_callsign = "MOE";
  if (FOX_ID == 2) my_callsign = "MOI";
  if (FOX_ID == 3) my_callsign = "MOS";
  if (FOX_ID == 4) my_callsign = "MOH";
  if (FOX_ID == 5) my_callsign = "MO5";

 // 3. THE "SERIAL AMBUSH" (Wait for the exact GPS slot)
  while (true) {
    while (GPS_Serial.available() > 0) {
      gps.encode(GPS_Serial.read());
    }

    // We force the ESP32 to wait until it has a valid date and the year is > 2023.
    // This prevents the fox from instantly firing on a 00:00:00 cold boot!
    if (gps.time.isValid() && gps.time.isUpdated() && gps.date.isValid() && gps.date.year() > 2023) {
      // Calculate where we are in the 5-minute (300 second) cycle
      uint32_t current_sec_of_hour = (gps.time.minute() * 60) + gps.time.second();
      uint32_t current_phase_sec = current_sec_of_hour % period_sec;

      // The exact moment our phase matches our offset, break the loop!
      if (current_phase_sec == offset_sec) {
        break;
      }
    }
  }

  // --- WE ARE IN THE TIMESLOT! ---

  // 4. Fire the perfectly timed transmission
  transmit_beacon(my_callsign);

  // 5. Go to Deep Sleep
  // We transmitted for 60s. Total cycle is 300s. Time remaining is 240s.
  // We sleep for 230 seconds so we wake up 10 seconds early for the next GPS ambush.
  esp_sleep_enable_timer_wakeup(230ULL * 1000000ULL);
  esp_deep_sleep_start();
}

void loop() {
  // Never reached
}