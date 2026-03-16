#pragma once

#include <cstdint>
#include <esp_log.h>

#include "shared.h"

// #include <Arduino.h>

// definitions for timings
#define VIS_LED_BLIP_TIME 1      // blip the visible LED this long (in msec) before transmitting a POWER-Code
#define TIME_BETWEEN_CODES 205   // time to wait (in msec) after sending a POWER-Code and before sending the next POWER-Code
#define BLIP_TIME 125            // length of delay between blinks when quickly blinking the LED

// definitions needed for the ESP32 LEDC library for Arduino
#define DUTY_RESOLUTION 8        // use 8-bit PWM duty cycle resolution (which allows DUTY_CYCLE to have a value of 0 to 255)
#define DUTY_CYCLE 128           // PWM duty cycle (since DUTY_RESOLUTION=8, 128 is a 50% duty cycle)
#define PWM_CHANNEL 0            // PWM channel to use (for this sketch, which channel we use isn't important)

// variables
int buttonStateNA = 0;         // for reading the NA pushbutton status
int buttonStateEU = 0;         // for reading the EU pushbutton status
int buttonStateNA_again = 0;   // for reading the NA pushbutton status if it is pushed during the transmission of the POWER-Codes
int buttonStateEU_again = 0;   // for reading the EU pushbutton status if it is pushed during the transmission of the POWER-Codes
int region;                    // keep track of either NA or EU
int startOver;                 // for starting the transmission over from the beginning, if the user presses a button while transmitting
int numCodes;                  // number of POWER-Codes to transmit (will be set to either num_NAcodes or num_EUcodes, depending on which region we're transmitting)
int numPairs;                  // number of On-Time/Off-Time pairs that comprise a POWER-Code (which is gotten from the POWER-Code's Code table)
uint32_t carrierFreq;          // for reading the Carrier Frequency from the Codes table for each POWER-Code
uint8_t pairsIndex;            // for reading the next index from the POWER-Code's Sequence table -- this is an index to the POWER-Code's Pairs table (of unique On-Time/Off-Time pairs)
uint32_t OnTime;               // for reading the On-Time from the POWER-Code's Pairs table
uint32_t OffTime;              // for reading the Off-Time from the POWER-Code's Pairs table

// constants and variables from "WORLDcodes.cpp"
extern "C" struct IrCode * NApowerCodes[];
extern "C" struct IrCode * EUpowerCodes[];
extern uint8_t num_NAcodes;
extern uint8_t num_EUcodes;

// pointers for grabbing the appropriate info for each POWER-Code to transmit
struct IrCode * pwrCode_ptr;   // points to either the table of NApowerCodes or EUpowerCodes
uint32_t * pairsTab_ptr;       // points to the Pairs table for a POWER-Code (which contains all of the unique On-Time/Off-Time pairs for a given POWER-Code)
uint8_t * sequenceTab_ptr;     // points to the Sequence table for a POWER-Code (which contains indices into the Pairs table, for transmitting the proper sequence of On-Time/Off-time pairs for the POWER-Code)


void xmitPair(uint32_t carFreq, uint32_t onTime, uint32_t offTime) {
  // ESP_LOGD("IR", "%d, %d", onTime, offTime);

  // if the Carrier Frequency is 0, then there is no Carrier Frequency, so don't pulse the IR LED (it will be solidly On)
  if (carFreq != 0) {
    // start PWM at carrier frequency to pulse the IR LED
    ledcAttachChannel(IRLED, carFreq, DUTY_RESOLUTION, PWM_CHANNEL);
    ledcWrite(IRLED, DUTY_CYCLE);
  } else {
    // IR LED "On"
    digitalWrite(IRLED, HIGH);
  }
  // "On" for the number of usec given in onTime
  ets_delay_us(onTime);

  // stop PWM, so IR LED is off (if carFreq isn't 0)
  if (carFreq != 0) {
    ledcDetach(IRLED);
  }
  // IR LED "Off" for the number of usec given in offTime
  digitalWrite(IRLED, LOW);
  // keep the IR LED off for the number of usec given in offTime
  ets_delay_us(offTime);
}

// TODO: run on task
void transmitPowerCodes() {
  pinMode(VISLED, OUTPUT);            // initialize the visible LED pin as an output (Green LED)
  pinMode(IRLED, OUTPUT);             // initialize the IR LED pin as an output

  int numCodes = num_NAcodes;
  for (int powerCodeCount = 0; powerCodeCount < numCodes; powerCodeCount++) {
    // get the address of the next POWER-Code into a pointer
    // if (region == NA) {
      struct IrCode * pwrCode_ptr = NApowerCodes[powerCodeCount];
    // }
    // else if (region == EU) {
    //   pwrCode_ptr = EUpowerCodes[powerCodeCount];
    // }

    // blip the visible LED (to indicate that a new POWER-Code is about to be transmitted on the IR LED)
    digitalWrite(VISLED, HIGH);
    // delay(VIS_LED_BLIP_TIME);
    vTaskDelay(VIS_LED_BLIP_TIME / portTICK_PERIOD_MS);

    digitalWrite(VISLED, LOW);

    // read the info from the IrCode table for this POWER-Code
    carrierFreq = pwrCode_ptr->carrier_freq;     // Carrier Frequency for this POWER-Code
    numPairs = pwrCode_ptr->num_pairs;           // number of On-Time/Off-Time pairs to transmit for this POWER-Code
    pairsTab_ptr = (pwrCode_ptr->pairs);         // point to the Pairs table of unique On-Time/Off-Time pairs for this POWER-Code
    sequenceTab_ptr = (pwrCode_ptr->sequence);   // point to the Sequence table of indices to the unique On-Time/Off-Time pairs for this POWER-Code

    // now that we have read all of the needed info from the current POWER-Code,
    // increment the pointer to the next POWER-Code in the table
    pwrCode_ptr++;

    #if DEBUG > 0
    DEBUGP( Serial.print("The Carrier Frequency for this POWER-Code is: ") );
    DEBUGP( Serial.println(carrierFreq) );
    DEBUGP( Serial.print("The # On-Time/Off-Time pairs for this POWER-Code is: ") );
    DEBUGP( Serial.println(numPairs) );
    DEBUGP( Serial.print("First 4 unique On-Time/Off-Time pairs: ") );
            for (int i=0; i<8; i=i+2) {
    DEBUGP(   Serial.print(pairsTab_ptr[i]) );
    DEBUGP(   Serial.print(",  ") );
    DEBUGP(   Serial.print(pairsTab_ptr[i+1]) );
    DEBUGP(   Serial.print("     ") );
            }
    DEBUGP( Serial.println("") );
    DEBUGP( Serial.print("Sequence: ") );
            for (int i=0; i<numPairs; i++) {
    DEBUGP(   Serial.print(sequenceTab_ptr[i]) );
    DEBUGP(   Serial.print(",  ") );
            }
    DEBUGP( Serial.println("") );
    #endif

    ESP_LOGD("IR", "Transmitting POWER-Code %d...", powerCodeCount);

    // transmit all On-Time/Off-Time pairs for this POWER-Code
    for (int i=0; i<numPairs; i++) {
      pairsIndex = sequenceTab_ptr[i];        // read the next index from the POWER-Code's Sequence table -- this is an index to the POWER-Code's Pairs table (of unique On-Time/Off-Time pairs)
      // ESP_LOGD("IR", "Pair index: %d", pairsIndex);
      pairsIndex = pairsIndex * 2;            // multiply the index by 2 since we want to index pairs in the Pairs table
      OnTime = pairsTab_ptr[pairsIndex];      // read the On-Time from the POWER-Code's Pairs table
      OffTime = pairsTab_ptr[pairsIndex+1];   // read the Off-Time from the POWER-Code's Pairs table
      xmitPair( carrierFreq, OnTime, OffTime );
    }

    ESP_LOGD("IR", "Done\n");

    // delay for a while between sending POWER-Codes (to give IR receiver on TVs long enough to reset themselves after receiving other TV-B-Gone POWER-Codes)
    // delay(TIME_BETWEEN_CODES - VIS_LED_BLIP_TIME);   // Since we already waited for VIS_LED_BLIP_TIME msec, we can subtract this amount from TIME_BETWEEN_CODES
    vTaskDelay((TIME_BETWEEN_CODES - VIS_LED_BLIP_TIME) / portTICK_PERIOD_MS);
    break;
  }
}
