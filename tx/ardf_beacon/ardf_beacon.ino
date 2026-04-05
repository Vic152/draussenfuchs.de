/*

Very basic 70cm-ISM fox-transmitter based on ESP32 and generic transmitter module.
See draussenfuchs.de for details.

--

The MIT License (MIT)

Copyright (c) 2025 Harm, DK4HAA, draussenfuchs.de

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

--

INSTALL

Install ESP32 via boards manager (if not done before)
  1. Open Arduino IDE
  2. Open Tools > Board > Boards Manager
  3. Search for "esp32"
  4. Install "esp32" by Espressif Systems

Select Board
  1. Select Tools > Board > esp32 > Nologo ESP32C3 Super Mini
  2. Use default settings for this board

Compile and install as usual. :)


IMPORTANT NOTE

This code uses deep sleep. Once flashed, your ESP32 will go
to deep sleep most of the time and will not be available as a serial
device during deep sleep.

You have to hold down the "boot" button and press the "RST" button 
to go back into bootloader mode to re-flash/update the board.

*/
// Settings
#define BEACON_TYPE 2               // Defines which beacon is setup on the transmitter
                                    // 0 = Finish beacon - MO - continues
                                    // 1 = First minute - MOE
                                    // 2 = Second minute - MOI
                                    // 3 = Third minute - MOS
                                    // 4 = Fourth minute - MOH
                                    // 5 = Fith minute - MO5
                                    // Any other number will result in error code UUU being emmited


#define FREQ_HZ 494  // Tone frequency of the fox. For reference: c-major-scale: 440 494 523 587 659 698 784 880 988 1047
#define WPM_SPEED 10                  // Words per minute speed
#define TIME_TO_NEXT_TRANSMIT 240000  // Time between beacon activations - 240000 = 4 minutes
#define BEACON_DURATION 60000         // How long will the beacon be active - 60000 = 1 minute
// End of settings


#define BASE_DOT_TIME 1200          // Dot time for 1 word per minute

#define DATA_PIN 3               // Pin of the ESP32 connected to the data pin of the transmitter module
#define TX_ARTIFICIAL_VCC_PIN 4  // Pin of the ESP32 providing power to the transmitter module due to unused enable pin

#define uS_TO_MS_FACTOR 1000  // Conversion factor for micro seconds to ms

// Calculate dot time based on speed
int dot_time() {
  return BASE_DOT_TIME / WPM_SPEED;
}

int space_time() {
  int dot = dot_time();
  return dot * 5;
}

void dot() {

  // start TX
  digitalWrite(TX_ARTIFICIAL_VCC_PIN, HIGH);  // enable transmitter module
  digitalWrite(LED_BUILTIN, LOW);             // enable built-in LED
  tone(DATA_PIN, FREQ_HZ);                    // output tone to transmitter
  int dot = dot_time();

  delay(dot);  // wait...

  // stop TX
  noTone(DATA_PIN);                          // stop tone
  digitalWrite(LED_BUILTIN, HIGH);           // disable LED
  digitalWrite(TX_ARTIFICIAL_VCC_PIN, LOW);  // disable transmitter module
  delay(dot);
}

void dash() {

  // start TX
  digitalWrite(TX_ARTIFICIAL_VCC_PIN, HIGH);  // enable transmitter module
  digitalWrite(LED_BUILTIN, LOW);             // enable built-in LED
  tone(DATA_PIN, FREQ_HZ);                    // output tone to transmitter
  int dot = dot_time();

  delay(dot * 3);  // wait...

  // stop TX
  noTone(DATA_PIN);                          // stop tone
  digitalWrite(LED_BUILTIN, HIGH);           // disable LED
  digitalWrite(TX_ARTIFICIAL_VCC_PIN, LOW);  // disable transmitter module
  delay(dot);
}

void send_letter(char letter) {

  // Convert lowercase to uppercase automatically
  // so we only need to write 36 cases instead of 62!
  letter = toupper(letter);
  int space = space_time();

  switch (letter) {
    // --- LETTERS ---
    case 'A':
      dot();
      dash();
      break;
    case 'B':
      dash();
      dot();
      dot();
      dot();
      break;
    case 'C':
      dash();
      dot();
      dash();
      dot();
      break;
    case 'D':
      dash();
      dot();
      dot();
      break;
    case 'E':
      dot();
      break;
    case 'F':
      dot();
      dot();
      dash();
      dot();
      break;
    case 'G':
      dash();
      dash();
      dot();
      break;
    case 'H':
      dot();
      dot();
      dot();
      dot();
      break;
    case 'I':
      dot();
      dot();
      break;
    case 'J':
      dot();
      dash();
      dash();
      dash();
      break;
    case 'K':
      dash();
      dot();
      dash();
      break;
    case 'L':
      dot();
      dash();
      dot();
      dot();
      break;
    case 'M':
      dash();
      dash();
      break;
    case 'N':
      dash();
      dot();
      break;
    case 'O':
      dash();
      dash();
      dash();
      break;
    case 'P':
      dot();
      dash();
      dash();
      dot();
      break;
    case 'Q':
      dash();
      dash();
      dot();
      dash();
      break;
    case 'R':
      dot();
      dash();
      dot();
      break;
    case 'S':
      dot();
      dot();
      dot();
      break;
    case 'T':
      dash();
      break;
    case 'U':
      dot();
      dot();
      dash();
      break;
    case 'V':
      dot();
      dot();
      dot();
      dash();
      break;
    case 'W':
      dot();
      dash();
      dash();
      break;
    case 'X':
      dash();
      dot();
      dot();
      dash();
      break;
    case 'Y':
      dash();
      dot();
      dash();
      dash();
      break;
    case 'Z':
      dash();
      dash();
      dot();
      dot();
      break;

    // --- NUMBERS ---
    case '0':
      dash();
      dash();
      dash();
      dash();
      dash();
      break;
    case '1':
      dot();
      dash();
      dash();
      dash();
      dash();
      break;
    case '2':
      dot();
      dot();
      dash();
      dash();
      dash();
      break;
    case '3':
      dot();
      dot();
      dot();
      dash();
      dash();
      break;
    case '4':
      dot();
      dot();
      dot();
      dot();
      dash();
      break;
    case '5':
      dot();
      dot();
      dot();
      dot();
      dot();
      break;
    case '6':
      dash();
      dot();
      dot();
      dot();
      dot();
      break;
    case '7':
      dash();
      dash();
      dot();
      dot();
      dot();
      break;
    case '8':
      dash();
      dash();
      dash();
      dot();
      dot();
      break;
    case '9':
      dash();
      dash();
      dash();
      dash();
      dot();
      break;
    case ' ':
      delay(space);
      break;


    // --- UNKNOWN CHARACTERS ---
    default:
      dot();
      dot();
      dash();
      dot();
      dot();
      dash();
      dot();
      dot();
      dash();
      break;
  }
}

void send_letters(const char* letters) {

  int dot_t = dot_time();

  for (int i = 0; letters[i] != '\0'; i++) {
    send_letter(letters[i]);
    delay(dot_t * 2);
  }
}

// But during Deep Sleep, it will remember its last state.
RTC_DATA_ATTR bool was_delayed = false;

void transmit_beacon() {

  int beacon_delay = 0;
  char beacon_code[500];

  switch (BEACON_TYPE) {
    case 0:
      beacon_delay = 0;
      strcpy(beacon_code, "MO");
    case 1:
      beacon_delay = 0;
      strcpy(beacon_code, "MOE");
      break;
    case 2:
      beacon_delay = 60000;
      strcpy(beacon_code, "MOI");
      break;
    case 3:
      beacon_delay = 120000;
      strcpy(beacon_code, "MOS");
      break;
    case 4:
      beacon_delay = 180000;
      strcpy(beacon_code, "MOH");
      break;
    case 5:
      beacon_delay = 240000;
      strcpy(beacon_code, "MO5");
      break;

    default:
      beacon_delay = 0;
      strcpy(beacon_code, "UUU");
      break;
  }

  bool hasRun = false;  // Flag to ensure it only runs once
  int space = space_time();

  if (BEACON_TYPE == 0) {
    while (true) {
      if (!was_delayed) {
        delay(beacon_delay);
        was_delayed = true;
      }
      send_letters(beacon_code);
      delay(space);
    }
  }

  // Only execute this block if it hasn't run yet
  if (!hasRun) {

    unsigned long startTime = millis();
    const unsigned long totalDuration = BEACON_DURATION;

    // The Arduino is trapped inside this while loop for exactly 60 seconds
    while (millis() - startTime <= totalDuration) {

      // Because there is no interval check, this function fires
      // continuously and aggressively as fast as the chip can process it.
      if (!was_delayed) {
        delay(beacon_delay);
        was_delayed = true;
      }
      send_letters(beacon_code);
      delay(space);
    }

    // This code only runs after the 60 seconds have completely finished
    hasRun = true;  // Set the flag to true so it doesn't run again
  }
}


void setup() {
  // set pin modes
  pinMode(TX_ARTIFICIAL_VCC_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // set wakeup timer
  esp_sleep_enable_timer_wakeup(TIME_TO_NEXT_TRANSMIT * uS_TO_MS_FACTOR);

  transmit_beacon();

  esp_deep_sleep_start();
}

void loop() {
  // never executed because of deep sleep
}
