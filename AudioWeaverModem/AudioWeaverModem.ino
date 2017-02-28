//-------------------------------------------------------------------------------------------
// Test sketch for the "AudioWeaverModem" Teensy Audio object for Weaver methhod SSB
// modulation/demodulation.
//
// Uses two instances of AudioWeaverModem objects:
//   1) the first configured as a modulator to take an audio signal and convert to an SSB signal
//   2) the second configured as a demodulator  to convert the SSB back to audio.
// Plays audio through the I2S input and outputs on the Teensy Audio shield
//
// Author:  Derek Rowell
// Date:    2/27/2017
//
//-------------------------------------------------------------------------------------------
#include <Audio.h>
#include "AudioWeaverModem.h"
#include <Wire.h>
#include <SPI.h>
#include <SerialFlash.h>
#define LPF_cutoff 1500.0

//------------------------------------------------------
AudioInputI2S     AudioInput;
AudioMixer4       Gain;
AudioWeaverModem  Demodulator;         
AudioWeaverModem  Modulator;         
AudioOutputI2S    Output;
//-------------------------------------------------------
AudioConnection c1(AudioInput,0,  Gain,0);
AudioConnection c2(Gain,0,        Modulator,0);
AudioConnection c3(Modulator,0,   Demodulator,0);
AudioConnection c4(Demodulator,0, Output,0);
AudioControlSGTL5000 audioShield;

//-------------------------------------------------------
void setup() {
  double TuningFrequency;                         // Weaver tuning frequency 
  double SSBFrequency;
//----------------     Enter parameters here:
  SSBFrequency = 12000.;                          // SSB signal frequency
  boolean USB = true;                             // Select upper or lower sideband
//----------------       
  if (USB){
    TuningFrequency = SSBFrequency + LPF_cutoff;  // Set up for USB
    Modulator.USB();
    Demodulator.USB();
  } else { 
    TuningFrequency = SSBFrequency - LPF_cutoff;  // Set up for LSB
    Modulator.LSB();
    Demodulator.LSB();
  }
  audioShield.enable();
  AudioMemory(20);
  audioShield.volume(0.7);
  Gain.gain(0., 2.0);                             // Input gain (adjust to suit audio source)
  Modulator.Oscillator1_freq(LPF_cutoff);         // Weaver modulator oscillator frequencies
  Modulator.Oscillator2_freq(TuningFrequency);
  Demodulator.Oscillator1_freq(TuningFrequency);  // Weaver demodulator oscillator frequencies
  Demodulator.Oscillator2_freq(LPF_cutoff);
}
//----------------------------------------------------------------------------
void loop() {
}


