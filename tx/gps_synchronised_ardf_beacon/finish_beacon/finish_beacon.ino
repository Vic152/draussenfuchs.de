/*
  ARDF Finish Beacon ("MO")
  Platform: ESP32-C3 Super Mini
  Hardware: Basic RF Transmitter (No GPS required)
  Behavior: Transmits "MO" continuously
*/

// ==============================================================================
// 1. TIMING & HARDWARE CONSTANTS
// ==============================================================================
#define WPM 10                         // Morse speed (words per minute)
#define DIT_DURATION_MS (1200 / WPM)   // Standard timing math
#define FREQ_HZ 2000                   // Pitch of the tone

#define DATA_PIN 3               // RF Transmitter Data
#define TX_ARTIFICIAL_VCC_PIN 4  // RF Transmitter Power

// ==============================================================================
// 2. MORSE CODE DICTIONARY (Simplified for 'M' and 'O' only)
// ==============================================================================
const char* morseFor(char c) {
  if (c == 'M') return "--";
  if (c == 'O') return "---";
  return ""; 
}

// Sends a single string as properly timed Morse Code
void sendMorse(const char* msg) {
  for (int i = 0; msg[i] != '\0'; i++) {
    char c = msg[i];
    
    // Word gap handling
    if (c == ' ') {
      delay(4 * DIT_DURATION_MS); 
      continue;
    }
    
    const char* code = morseFor(c);
    if (code[0] == '\0') continue;

    // Output the dots and dashes
    for (int j = 0; code[j] != '\0'; j++) {
      tone(DATA_PIN, FREQ_HZ);
      int duration = (code[j] == '-') ? (3 * DIT_DURATION_MS) : DIT_DURATION_MS;
      delay(duration);
      noTone(DATA_PIN);
      
      if (code[j + 1] != '\0') delay(DIT_DURATION_MS); // Intra-character gap
    }
    delay(3 * DIT_DURATION_MS); // Inter-character gap
  }
}

// ==============================================================================
// 3. MAIN LOGIC (CONTINUOUS TRANSMISSION)
// ==============================================================================
void setup() {
  pinMode(TX_ARTIFICIAL_VCC_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Power on the RF transmitter permanently
  digitalWrite(TX_ARTIFICIAL_VCC_PIN, HIGH);  
  digitalWrite(LED_BUILTIN, LOW); 
}

void loop() {
  // 1. Send the finish callsign
  sendMorse("MO");

  // 2. Standard 7-unit delay before repeating the message
  delay(7 * DIT_DURATION_MS);
}