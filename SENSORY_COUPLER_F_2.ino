//Sesory Coupler by Bleep Labs
// https://bleeplabs.com/sc/
// Production v2 for hardware v11 
// Uses Seeed Xiao



#define TOUCH_PIN 7
#define SENSOR 6
#define POT_TOP 1
#define POT_MIDDLE 3
#define ADJ_BUTTON 9
#define CALIBRATING_button 2
#define JACK_SWITCH_PIN 8
#define LED_PIN 10
#define CV 0
#define T1 4
#define T2 5
#define T2 5
#define flip_jack 1
byte lock_out_period = 6;
byte pot_tick;
int f1, f2, intpot;
int touch1;
float adj_color;

#include "Adafruit_FreeTouch.h"
Adafruit_FreeTouch qt_1 = Adafruit_FreeTouch(TOUCH_PIN, OVERSAMPLE_4, RESISTOR_100K, FREQ_MODE_NONE);
#include <TimerTC3.h>

#include <Adafruit_NeoPixel.h>
byte num_leds = 43;
Adafruit_NeoPixel leds = Adafruit_NeoPixel(num_leds, LED_PIN, NEO_RGB + NEO_KHZ800);

byte tick;

#include <FlashAsEEPROM.h>

#include <Bounce2.h>
Bounce cal_button = Bounce();
Bounce adj_button = Bounce();

#define BOUNCE_LOCK_OUT
uint32_t cm, pm, pm2, pm3, pm4, pm5;
int in[8];
int pbutt[2], butt[2];
int set_low = 0;
int set_high = 1023;
int fout;
int thresh;
int test_out, test_dir;
int raw_in;
int sense_exp;
int sense_ampcal;
int sense_smooth;
int pot_raw[3];
const int ppc_len = 50;
int prev_pot_raw[3][ppc_len];
int prev_pot_counter[3];

int trails[8];
int trail_count;
int ledout, prev_ledout;
int smooth_amount_s1;
int smooth_s1;
int smooth_s1_c;
int32_t du, cu;
#define max_trail_len 65
int trail_len;
int prev_pos[max_trail_len];
float fade_level[max_trail_len];
int smooth_pot;
int smooth_amount;
#define sm_max 2100
#define s1_max 200
int smooth_bank[sm_max];
int smooth_bank_s1[s1_max];
int smooth_count;
int32_t sm_out;
int real_fout;
int set_mode_out;
byte tout;
byte high_is_set = 0;
byte low_is_set = 0;
byte test_mode = 0;
int thresh_led;

int clear_count;
byte clear_hi_low;
int high_read = 0;
int low_read = 2000;
int adj_led;
int cal_led;
int blinko;
int blinko_count;
byte jack_switch, prev_jack_switch;
int16_t cal_count;
int32_t cal_time;
float cal_color = .28;
byte tick_out;
int led_lerp;
int gap_size;
int sm_mode;
int sr, tr;

int sm_set;
int final_sm_out;
float prev_follow_out, follow_out;
float follow_amount;
float g1;
float amp_pot;
int exp_in;
byte setting_out_mode;
int32_t setting_out_mode_timer;
byte out_mode = 3;
byte amp_pot_change;
byte thresh_pot_change;
byte setting_thresh_size;
int thresh_size = 0;
byte smooth_pot_change;
byte setting_bright;
float max_brightness = .085;
int led_fout;
int sm_low = 9;
byte set_mode;
float grad_amount = 200.0;  //gradient distance
float sm_pot_center = 512;
int sm_gap = 75;
int prev_sm_mode;
int prev_smooth_amount;
int jack_a, prev_jack_a;
int smooth_amount_curved;
int smooth_amount_final;
int smooth_amount_skip;
int follow_amount_raw;
byte sas_count;
#define IDLE 10
#define CALIBRATING 20

int sermode = 1;
int compc = 0x2000;
int intc = 0x3F;
byte smode;
byte blink0;
byte lock_out;
byte lock_count;
byte prev_tout;
byte ftout;
byte led_mode = 0;
uint32_t tc, pc, dc;
int mappt;
float pot_color;
int set_low_change, set_high_change;
float pmb;
byte prev_out_mode;
int prev_thresh_size;
byte eepchange;
int thresh_start, thresh_end;
byte sm_change;
byte ttt;
float fade_amount = .96;
byte timer_test = 0;
float trail_off = -0.1;
byte cal_set;
uint32_t cal_rst;
int max_bits = 1023;

byte midi_note_high = 96;
byte midi_note_low = 48;
byte midi_n, midi_fout_change;
byte midi_chromatic_ch = 1;
byte midi_gate_ch = 2;
byte midi_cc_ch = 3;
byte midi_current_n;
byte prev_midi_fout, midi_fout;
int midi_note_send_rate = 32;
byte cc_low;
byte cc_high;
byte note_send_state, prev_note_send_state;
byte cc_send, p_cc_send;
byte tout_ch;
int ar_max = 944;
int ar_min = 12;
float bright2 = .6;

#include <USB-MIDI.h>

USBMIDI_CREATE_DEFAULT_INSTANCE();


void printer() {


  if (1) {
    Serial.print(0);
    Serial.print(" ");
    Serial.print(1023);
    Serial.print(" ");
    Serial.print(pot_raw[0]);
    Serial.print(" ");
    Serial.print(pot_raw[1]);
    Serial.print(" ");
    Serial.println(raw_in);
  }
  if (0) {

    Serial.print(set_low);
    Serial.print(" ");
    Serial.print(set_high);
    Serial.print(" ");
    Serial.print(real_fout);
    Serial.print(" ");
    Serial.println(raw_in);
  }

  if (0) {

    Serial.print(midi_note_high);
    Serial.print(" ");
    Serial.print(midi_note_low);
    Serial.print(" ");
    Serial.println(midi_note_send_rate);
  }
}

void setup() {


  leds.begin();
  set_LED(42, .6, 1, 1);
  leds.show();

  byte et1 = EEPROM.read(20);
  //Serial.println(et1);

  if (et1 == 111) {  //eeep is erased on upload
    delay(250);
    cal_set = 1;
    max_brightness = EEPROM.read(0) / 255.0;
    out_mode = EEPROM.read(1);
    thresh_size = (EEPROM.read(2) << 8) + EEPROM.read(3);
    set_low = (EEPROM.read(4) << 8) + EEPROM.read(5);
    set_high = (EEPROM.read(6) << 8) + EEPROM.read(7);
    midi_note_high = EEPROM.read(8);
    midi_note_low = EEPROM.read(9);
    midi_note_send_rate = (EEPROM.read(10) * 2) + 8;

  } else {

    test_mode = 1;  ////////////////////////////////////////////////////////////////////////
    delay(750);
    cal_set = 0;
    max_brightness = .085;
    out_mode = 3;
    thresh_size = 0;
    set_low = 0;
    set_high = ar_max;
    if (1) {
      EEPROM.write(0, max_brightness * 255);
      EEPROM.write(1, out_mode);
      EEPROM.write(2, thresh_size >> 8);
      EEPROM.write(3, byte(thresh_size));
      EEPROM.write(4, set_low >> 8);
      EEPROM.write(5, byte(set_low));
      EEPROM.write(6, set_high >> 8);
      EEPROM.write(7, byte(set_high));
      EEPROM.write(8, midi_note_high);
      EEPROM.write(9, midi_note_low);
      EEPROM.write(10, (midi_note_send_rate - 8) / 2);
      EEPROM.write(20, 111);

      EEPROM.commit();
    }
  }

  pinMode(13, OUTPUT);
  pinMode(T1, OUTPUT);
  pinMode(T2, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(JACK_SWITCH_PIN, INPUT);  //has pull up resistor
  analogReadResolution(10);

  pinMode(CALIBRATING_button, INPUT_PULLUP);
  cal_button.attach(CALIBRATING_button, INPUT_PULLUP);  //setup the bounce instance for the current cal_button
  cal_button.interval(5);                               // interval in ms

  pinMode(ADJ_BUTTON, INPUT_PULLUP);
  adj_button.attach(ADJ_BUTTON, INPUT_PULLUP);  //setup the bounce instance for the current cal_button
  adj_button.interval(5);                       // interval in ms

  qt_1.begin();
  //qt_1.setCompCap(0x2000);  //0-16383, default  0x2000, 8192
  // qt_1.setIntCap(63);       // 0 - 63, default 0x3F, 63


  cal_button.update();
  adj_button.update();

  if (cal_button.read() == 0 && adj_button.read() == 0) {

    while (adj_button.read() == 0) {
      //while (1) {
      adj_button.update();
      float m;
      for (int j = 0; j < num_leds; j++) {
        set_LED(j, (j / float(num_leds)) + m, .7, 1);
        m += .02;
        if (m > 1.0) {
          m = 0;
        }
        leds.show();

        delay(10);
      }
    }
    test_mode = 1;
    max_brightness = .085;
    out_mode = 3;
    thresh_size = 0;
    set_low = 0;
    set_high = ar_max;
  }
  sm_change = 111;

  MIDI.begin(1);
  MIDI.turnThruOff();



  TimerTc3.initialize(400);
  TimerTc3.attachInterrupt(timerIsr);
}


void loop() {
  if (timer_test) {
    ttt = !ttt;
    digitalWrite(T1, ttt);
  }
  cm = millis();

  if (sm_change > 0) {
    if (sm_change == 10 || sm_change == 20) {
      for (int j = 0; j < sm_max; j++) {
        smooth_bank[j] = raw_in;
      }
      for (int m = 0; m < trail_len; m++) {
        prev_pos[m] = ledout;
        fade_level[m] = 0;
      }
      sm_change = 0;
    }
  }

  if (sm_mode == 2) {
    int st = 512 - (sm_gap / 2);
    smooth_amount_curved = fscale(smooth_amount, 0, st, 4, sm_max, -7);
  }
  if (sm_mode == 1) {
    smooth_amount = 4;
    int st = 512 - (sm_gap / 2);
    float ft = (sm_pot_center - pot_raw[1] - sm_gap) / (sm_pot_center - sm_gap);
    smooth_amount_curved = sm_max;
    float fa = .006;
    follow_amount = (ft * fa) + (1.0 - fa) - .0005;
  }
  if (sm_mode == 0) {
    smooth_amount_curved = sm_max;
  }

  cal_button.update();
  adj_button.update();

  if (set_low < 0) {
    set_low = 0;
  }
  if (set_high > max_bits) {
    set_high = max_bits;
  }


  if (cal_button.fell()) {
    //test_mode = 0;
    pm3 = cm;
    cal_rst = cm;
    if (set_mode == CALIBRATING) {
      set_mode = IDLE;
      sm_mode = prev_sm_mode;
      smooth_amount = prev_smooth_amount;
      pm3 = cm;
      eepchange = 1;
    }

    else {

      cal_set = 1;
      set_mode = CALIBRATING;
      set_high = 0;
      set_low = max_bits;
      prev_sm_mode = sm_mode;
      setting_out_mode = 0;
      prev_smooth_amount = smooth_amount;
    }
  }

  if (cal_button.rose()) {
    cal_rst = 0;
  }

  if (cal_button.read() == 0) {
    if (cm - cal_rst > 1400 && cal_rst > 0) {
      cal_set = 0;
      set_mode = IDLE;
      set_high = max_bits;
      set_low = 0;
      cal_rst = 0;

      Serial.println("           RST");
    }
  }


  if (adj_button.fell()) {
    pm3 = cm;
    pmb = max_brightness;
    prev_out_mode = out_mode;
    prev_thresh_size = thresh_size;

    if (setting_out_mode == 1) {
      out_mode++;
      if (out_mode > 3) {
        out_mode = 0;
      }
    }
    setting_out_mode = 1;

    set_mode = IDLE;
  }


  if (adj_button.read() == 0) {
    adj_led = 1;
    pm3 = cm;
    setting_out_mode_timer = -1;

    setting_thresh_size = thresh_pot_change;
    setting_bright = smooth_pot_change;
  }

  if (adj_button.rose()) {

    for (byte p = 0; p < 128; p++) {
      MIDI.sendNoteOff(p, 0, midi_chromatic_ch);
      MIDI.sendNoteOff(p, 0, midi_gate_ch);
      MIDI.sendNoteOff(p, 0, midi_cc_ch);
    }

    setting_out_mode_timer = cm;

    setting_thresh_size = 0;
    setting_bright = 0;

    if (
      pmb != max_brightness || prev_out_mode != out_mode || prev_thresh_size != thresh_size) {
      pm3 = cm;
      eepchange = 1;
    }
  }


  if (cm - pm3 > 3000) {
    pm3 = cm;
    //Serial.println(".");
    if (eepchange == 1) {
      //Serial.println("eep");
      eepchange = 0;
      EEPROM.write(0, max_brightness * 255);
      EEPROM.write(1, out_mode);
      EEPROM.write(2, thresh_size >> 8);
      EEPROM.write(3, byte(thresh_size));
      EEPROM.write(4, set_low >> 8);
      EEPROM.write(5, byte(set_low));
      EEPROM.write(6, set_high >> 8);
      EEPROM.write(7, byte(set_high));
      EEPROM.write(8, midi_note_high);
      EEPROM.write(9, midi_note_low);
      EEPROM.write(10, (midi_note_send_rate - 8) / 2);
      EEPROM.commit();
    }
  }


  if (set_mode == CALIBRATING) {

    if (smooth_s1 > set_high) {
      set_high = constrain(smooth_s1, 0, max_bits);
      set_high_change = 40;
    }
    if (smooth_s1 < set_low) {
      set_low = constrain(smooth_s1, 0, max_bits);
      set_low_change = 40;
    }
  }



  if (setting_bright == 1 && cm > 500) {
    adj_led = 1;
    setting_out_mode = 0;
    //setting_thresh_size = 0;
    max_brightness = (pot_raw[1] / float(max_bits)) * .275;
  }

  if (setting_thresh_size == 1 && cm > 500) {
    adj_led = 1;
    setting_out_mode = 0;
    //setting_bright = 0;
    thresh_size = pot_raw[0];
    if (pot_raw[0] < 20) {
      thresh_size = 0;
    }
  }

  if (adj_button.read() == 1 && setting_thresh_size == 0 && setting_bright == 0 && setting_out_mode == 0) {

    adj_led = 0;
  }





  if (cm - pm > 4) {
    pm = cm;

    if (setting_out_mode == 1) {
      adj_led = 1;
      if (setting_out_mode_timer > 0
          && cm - setting_out_mode_timer > 1000) {
        setting_out_mode = 0;
        setting_thresh_size = 0;
        setting_bright = 0;
      }

      for (int j = 0; j < num_leds; j++) {
        leds.setPixelColor(j, leds.Color(0, 0, 0));
      }
      byte areas = (36 / 4);
      set_LED(0, 0, 0, 1);
      set_LED(37, 0, 0, 1);
      set_LED(areas * 1, 0, 0, 1);
      set_LED(areas * 2, 0, 0, 1);
      set_LED(areas * 3, 0, 0, 1);


      if (out_mode == 0) {
        for (int j = areas * 0; j < areas * 1; j++) {
          set_LED(j, .4, 1, 1);
        }
      }
      if (out_mode == 1) {
        for (int j = areas * 1; j < areas * 2; j++) {
          set_LED(j, .5, 1, 1);
        }
      }
      if (out_mode == 2) {

        for (int j = areas * 2; j < areas * 3; j++) {
          set_LED(j, .6, 1, 1);
        }
      }
      if (out_mode == 3) {
        for (int j = areas * 3; j < areas * 4; j++) {
          set_LED(j, .7, 1, 1);
        }
      }
    }

    if (setting_out_mode == 0) {

      for (int j = 0; j < num_leds; j++) {
        leds.setPixelColor(j, leds.Color(0, 0, 0));
      }

      for (int j = trail_len - 1; j > 0; j--) {
        prev_pos[j] = prev_pos[j - 1];
        fade_level[j] = fade_level[j - 1];
      }

      ledout = map(real_fout, 0, 1023, 0, 37);
      if (led_lerp < ledout) {
        led_lerp++;
      }
      if (led_lerp > ledout) {
        led_lerp--;
      }

      prev_pos[0] = ledout;
      if (sm_mode == 0) {
        g1 = .9 + (ledout / grad_amount);
        trail_len = 4;
      }
      if (sm_mode == 1) {
        g1 = .58 - (ledout / grad_amount);
        trail_len = 20;
      }
      if (sm_mode == 2) {
        trail_len = 20;
        g1 = .72 + (ledout / grad_amount);
      }

      if (set_mode != CALIBRATING) {

        fade_level[1] = 1.0;
        for (int trail = 0; trail < trail_len; trail++) {
          if (set_mode != CALIBRATING) {
            set_LED(prev_pos[trail], g1 + trail_off, 1, fade_level[trail]);
          }
          if (trail > 0) {
            gap_size = prev_pos[trail - 1] - prev_pos[trail];
            if (gap_size >= 1) {
              for (byte f = 1; f < gap_size; f++) {
                int loc = prev_pos[trail] + f;
                if (set_mode != CALIBRATING) {
                  set_LED(loc, g1 + trail_off, 1, fade_level[trail]);
                }
              }
            }
            if (gap_size <= -1) {
              for (int g = abs(gap_size - 1); g > 1; g--) {
                int loc = prev_pos[trail] - g;
                if (set_mode != CALIBRATING) {
                  set_LED(loc, g1 + trail_off, 1, fade_level[trail]);
                }
              }
            }
          }
        }

        if (thresh_size == 0) {
          thresh_led = map(thresh, 0, ar_max, 0, 37);
          set_LED(thresh_led, 0, 0, .7);
        } else {
          thresh_start = map(thresh, 0, ar_max, 0, 38);
          thresh_end = map(thresh + thresh_size, 0, ar_max, 0, 38);
          if (thresh_end <= thresh_start + 2) {
            thresh_end = thresh_start + 2;
          }
          for (int th = thresh_start; th < thresh_end; th++) {
            if (ledout == th + 1 || ledout == th - 1) {
              set_LED(th, 0, 0, .15);
            } else {
              set_LED(th, 0, 0, .4);
            }
          }
        }




        set_LED(prev_pos[0], g1, 1, 1);
      }


      for (int i = 0; i < trail_len; i++) {
        fade_level[i] *= fade_amount;
        //Serial.println("fade_level[i]");
        if (fade_level[i] <= .01) {  //an arbitray cuttoff since it will never quite get to 0 through multiplication
          fade_level[i] = 0;
        }
      }
    }

    if (set_mode == CALIBRATING) {
      trail_len = 2;
      if (set_low_change > 0) {
        set_low_change--;
      } else {
        set_low_change = 0;
      }
      if (set_high_change > 0) {
        set_high_change--;
      } else {
        set_high_change = 0;
      }
      set_LED(1, 0, 0, set_low_change / 40.0);
      set_LED(2, 0, 0, set_low_change / 35.0);
      set_LED(35, 0, 0, set_high_change / 45.0);
      set_LED(36, 0, 0, set_high_change / 40.0);

      int b = constrain(led_lerp - 1, 0, 37);
      int a = constrain(led_lerp + 1, 0, 37);
      set_LED(b, cal_color + .05, 1, 1);
      set_LED(led_lerp, cal_color, 1, 1);
      set_LED(a, cal_color + .05, 1, 1);

      set_LED(0, 0, 0, set_low_change / 45.0);

      set_LED(37, 0, 0, set_high_change / 35.0);
    }


    set_LED(38, 0, 0, tout * bright2);
    set_LED(39, 0, 0, (!tout) * bright2);

    if (out_mode == 3 && setting_out_mode == 0) {
      set_LED(41, 0, 0, 0);
    }
    if (setting_out_mode == 1) {
      set_LED(41, 0, 0, blinko);
    }
    if (out_mode != 3 && setting_out_mode == 0) {
      set_LED(41, 0, 0, 1 * bright2);
    }
    set_LED(40, 0, 0, cal_set * bright2);
    set_LED(42, 0, 0, jack_switch * bright2);

    if (set_mode == CALIBRATING) {
      adj_led = 0;
      set_LED(40, cal_color, 1, blinko);
    }



    if (leds.canShow()) {
      leds.show();
    }

    blinko_count++;
    if (blinko_count > 6) {
      blinko_count = 0;
      blinko = !blinko;
    }
  }

  if (cm - pm2 > 40 && test_mode) {
    //if (cm - pm2 > 100 && 1) {
    pm2 = cm;
    blink0 = !blink0;
    digitalWrite(13, blink0);

    printer();
  }

  if (cm - pm4 > midi_note_send_rate / 2 && adj_button.read() == 1) {

    if (midi_fout_change == 2) {
      MIDI.sendNoteOff(midi_current_n, 0, midi_chromatic_ch);
      midi_current_n = midi_n;
      midi_fout_change = 1;
    }
  }

  if (cm - pm4 > midi_note_send_rate && adj_button.read() == 1) {
    pm4 = cm;
    if (midi_fout_change == 1) {
      MIDI.sendNoteOn(midi_current_n, 127, midi_chromatic_ch);
      midi_fout_change = 0;
    }
  }



  if (cm - pm5 > 4 && 1) {
    pm5 = cm;
    p_cc_send = cc_send;
    cc_send = real_fout >> 3;
    //cc_low = cc_send;
    //cc_high = cc_send >> 8;

    if (p_cc_send != cc_send) {
      MIDI.sendControlChange(31, cc_send, midi_cc_ch);
      //MIDI.sendControlChange(63, cc_low, midi_cc_ch); //abletons not liking it
    }

    if (tout_ch == 1 && adj_button.read() == 1) {
      if (tout == 0) {
        MIDI.sendNoteOff(2, 0, midi_gate_ch);
        MIDI.sendNoteOn(1, 127, midi_gate_ch);
      }
      if (tout == 1) {
        MIDI.sendNoteOff(1, 0, midi_gate_ch);
        MIDI.sendNoteOn(2, 127, midi_gate_ch);
      }
    }
    tout_ch = 0;
  }
}


/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////

void timerIsr() {
  if (timer_test) {
    digitalWrite(T2, 1);
  }

  if (MIDI.read()) {
    byte type = MIDI.getType();
    byte d1 = MIDI.getData1();
    byte d2 = MIDI.getData2();
    if (type == 176) {
      if (d1 == 70) {
        midi_note_low = d2;
      }
      if (d1 == 71) {
        midi_note_high = d2;
      }
      if (d1 == 72) {
        midi_note_send_rate = (d2 * 2) + 8;
      }
    }
  }

  pot_tick++;
  if (pot_tick > 2) {
    pot_tick = 0;
  }

  if (pot_tick == 2) {
    prev_jack_switch = jack_switch;
    jack_a = analogRead(JACK_SWITCH_PIN);

    if (jack_a > 100) {
      if (flip_jack) {
        jack_switch = 0;
      } else {
        jack_switch = 1;
      }
    } else {
      if (flip_jack) {
        jack_switch = 1;
      } else {
        jack_switch = 0;
      }
    }
    if (prev_jack_switch != jack_switch) {
      if (set_mode == CALIBRATING) {
        set_high = 0;
        set_low = max_bits;
      }
    }
  }

  if (pot_tick == 0) {
    prev_pot_counter[0]++;
    if (prev_pot_counter[0] > ppc_len - 1) {
      prev_pot_counter[0] = 0;
    }

    prev_pot_raw[0][prev_pot_counter[0]] = pot_raw[0];
    byte gap = 6;
    pot_raw[0] = smooth(pot_tick, 27, analogRead(POT_TOP));
    int ppc2 = prev_pot_counter[0] + 1;
    if (ppc2 > ppc_len - 1) {
      ppc2 -= ppc_len - 1;
    }
    if (1) {  ////////////////////////////////////////////////////////////////////////////////////////////////////////
      if (pot_raw[0] > prev_pot_raw[0][ppc2] + gap || pot_raw[0] < prev_pot_raw[0][ppc2] - gap) {
        thresh_pot_change = 1;
        if (setting_thresh_size != 1) {
          thresh = pot_raw[0];
        }
      } else {
        if (adj_button.read() == 1) {
          thresh_pot_change = 0;
        }
      }
    }
  }

  if (pot_tick == 1) {

    prev_pot_counter[pot_tick]++;
    if (prev_pot_counter[pot_tick] > ppc_len - 1) {
      prev_pot_counter[pot_tick] = 0;
    }
    prev_pot_raw[pot_tick][prev_pot_counter[pot_tick]] = pot_raw[pot_tick];
    byte gap = 4;

    pot_raw[1] = smooth(pot_tick, 27, analogRead(POT_MIDDLE));
    int ppc2 = prev_pot_counter[pot_tick] + 1;
    if (ppc2 > ppc_len - 1) {
      ppc2 -= ppc_len - 1;
    }
    if (1) {  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      if (pot_raw[pot_tick] > prev_pot_raw[pot_tick][ppc2] + gap || pot_raw[pot_tick] < prev_pot_raw[pot_tick][ppc2] - gap) {
        smooth_pot_change = 1;
        if (setting_bright != 1 && 1) {

          if (pot_raw[1] < sm_pot_center - sm_gap) {
            sm_mode = 1;
            sm_change = 10;
          }

          else if (pot_raw[1] > sm_pot_center + sm_gap) {
            sm_mode = 2;
            sm_change = 20;
            smooth_amount = ((pot_raw[1] - (sm_gap / 2) - sm_pot_center));
            //trail_len = (smooth_amount/30)+10;
            //fade_amount = .98 + ((smooth_amount / 480.0) * .02);
          } else {
            sm_mode = 0;
            sm_change = 100;
            smooth_amount = sm_low;
          }

          smooth_amount = constrain(smooth_amount, sm_low, sm_max - 1);
        }
      } else {
        if (adj_button.read() == 1) {
          smooth_pot_change = 0;
        }
      }
    }


    if (set_mode == CALIBRATING) {
      smooth_amount = sm_low;
      sm_mode = 0;
    }
  }


  if (jack_switch == 1) {
    int tr1 = qt_1.measure();
    int tr2 = qt_1.measure();
    tr = (tr1 + tr2) >> 1;
    raw_in = tr;
  }

  if (jack_switch == 0) {
    analogReadResolution(12);
    int ra = analogRead(SENSOR);
    int rb = analogRead(SENSOR);
    analogReadResolution(10);
    sr = (ra + rb) >> 3;
    raw_in = sr;
    raw_in = constrain(raw_in, 0, ar_max);
  }

  raw_in = constrain(raw_in, 0, max_bits);



  if (sm_mode == 1) {
    smooth_amount_s1 = map(smooth_amount, 0, sm_max, 0, s1_max);
    if (smooth_amount_s1 < 4) {
      smooth_amount_s1 = 4;
    }
  }
  if (sm_mode == 2) {
    smooth_amount_s1 = 16;
  }
  smooth_s1_c++;
  if (smooth_s1_c > smooth_amount_s1) {
    smooth_s1_c = 0;
  }
  smooth_bank_s1[smooth_s1_c] = raw_in;

  smooth_s1 = 0;
  for (uint16_t j = 0; j < smooth_amount_s1; j++) {
    smooth_s1 += smooth_bank_s1[j];
  }

  smooth_s1 = smooth_s1 / smooth_amount_s1;

  if (sm_mode == 0) {
    smooth_s1 = raw_in;
  }

  if (set_mode != CALIBRATING) {
    cal_led = 0;
    set_mode_out = map(smooth_s1, set_low, set_high, 0, ar_max);
  }

  if (set_mode == CALIBRATING) {
    adj_led = 0;
    cal_led = 1;
    set_mode_out = map(smooth_s1, set_low, set_high, 0, ar_max);
  }


  sas_count = 0;
  smooth_count++;

  if (smooth_count > sm_max - 1) {
    smooth_count = 0;
  }

  smooth_bank[smooth_count] = set_mode_out;

  sm_out = 0;
  for (uint16_t j = 0; j < smooth_amount_curved; j++) {
    int sma = smooth_count - j;
    if (sma < 0) {
      sma += sm_max - 1;
    }
    sm_out += smooth_bank[sma];
  }

  sm_out = constrain(sm_out / smooth_amount_curved, 0, max_bits);


  if (follow_out < 1.0) {
    follow_out = 1.0;
  }
  if (follow_out > max_bits - 1) {
    follow_out = max_bits - 1;
  }

  prev_follow_out = follow_out;
  if (set_mode_out >= prev_follow_out) {
    follow_out = set_mode_out;
  }
  if (set_mode_out < prev_follow_out) {
    follow_out *= follow_amount;
  }

  if (sm_mode == 0) {
    final_sm_out = set_mode_out;
  }

  if (sm_mode == 1) {
    final_sm_out = follow_out;
  }
  if (sm_mode == 2) {
    final_sm_out = sm_out;
  }
  fout = constrain(final_sm_out, ar_min, ar_max);
  fout = map(fout, ar_min, ar_max, 0, 1023);


  prev_midi_fout = midi_fout;

  if (out_mode == 0) {
    real_fout = 1023 - fout;
    led_fout = real_fout;
    midi_fout = map(ar_max - fout, 0, 1023, midi_note_low, midi_note_high);
  }
  if (out_mode == 1) {
    real_fout = (1023 - fout) / 2;
    led_fout = ar_max - fout;
    midi_fout = map(ar_max - fout, 0, 1023, midi_note_low, midi_note_high);
  }
  if (out_mode == 2) {
    real_fout = fout / 2;
    led_fout = fout;
    midi_fout = map(fout, 0, 1023, midi_note_low, midi_note_high);
  }
  if (out_mode == 3) {
    real_fout = fout;
    led_fout = fout;
    midi_fout = map(fout, 0, 1023, midi_note_low, midi_note_high);
  }

  if (prev_midi_fout != midi_fout) {
    midi_fout_change = 2;
    midi_n = midi_fout;
    pm4 = cm;
  }


  if (test_mode) {
    if (test_dir == 1) {
      test_out += 5;
    }
    if (test_dir == 0) {
      test_out -= 5;
    }
    if (test_out > max_bits) {
      test_out = max_bits;
      test_dir = 0;
    }
    if (test_out < 1 || test_out > 2000) {
      test_out = 0;
      test_dir = 1;
    }
    analogWrite(CV, test_out);
  } else {
    analogWrite(CV, real_fout);
  }
  prev_tout = tout;
  if (lock_out == 0) {
    if (thresh_size > 0) {
      if (fout < thresh) {
        tout = 0;
      } else if (fout > thresh + thresh_size) {
        tout = 0;
      } else {
        tout = 1;
      }
    }

    if (thresh_size == 0) {
      if (fout < thresh) {
        tout = 0;
      } else {
        tout = 1;
      }
    }

    if (prev_tout != tout) {
      tout_ch = 1;
      lock_out = 1;
      lock_count = lock_out_period;
      ftout = tout;
    }
  }
  if (lock_out == 1) {
    lock_count--;
    if (lock_count <= 1) {
      lock_out = 0;
    }
  }
  if (timer_test) {
    digitalWrite(T2, 0);
  } else {
    digitalWrite(T1, !ftout);
    digitalWrite(T2, ftout);
  }
}




void set_LED(int pixel, float fh, float fs, float fv) {
  byte RedLight;
  byte GreenLight;
  byte BlueLight;

  byte h = fh * 255;
  byte s = fs * 255;
  byte v = fv * max_brightness * 255;

  h = (h * 192) / 256;            // 0..191
  unsigned int i = h / 32;        // We want a value of 0 thru 5
  unsigned int f = (h % 32) * 8;  // 'fractional' part of 'i' 0..248 in jumps

  unsigned int sInv = 255 - s;  // 0 -> 0xff, 0xff -> 0
  unsigned int fInv = 255 - f;  // 0 -> 0xff, 0xff -> 0
  byte pv = v * sInv / 256;     // pv will be in range 0 - 255
  byte qv = v * (256 - s * f / 256) / 256;
  byte tv = v * (256 - s * fInv / 256) / 256;

  switch (i) {
    case 0:
      RedLight = v;
      GreenLight = tv;
      BlueLight = pv;
      break;
    case 1:
      RedLight = qv;
      GreenLight = v;
      BlueLight = pv;
      break;
    case 2:
      RedLight = pv;
      GreenLight = v;
      BlueLight = tv;
      break;
    case 3:
      RedLight = pv;
      GreenLight = qv;
      BlueLight = v;
      break;
    case 4:
      RedLight = tv;
      GreenLight = pv;
      BlueLight = v;
      break;
    case 5:
      RedLight = v;
      GreenLight = pv;
      BlueLight = qv;
      break;
  }
  leds.setPixelColor(pixel, RedLight, GreenLight, BlueLight);
}


#define maxarrays 4    //max number of different variables to smooth
#define maxsamples 37  //max number of points to sample and
//reduce these numbers to save RAM

unsigned int smoothArray[maxarrays][maxsamples];

// sel should be a unique number for each occurrence
// samples should be an odd number greater that 7. It's the length of the array. The larger the more smooth but less responsive
// raw_in is the input. positive numbers in and out only.

unsigned int smooth(byte sel, unsigned int samples, unsigned int raw) {
  int j, k, temp, top, bottom;
  long total;
  static int i[maxarrays];
  static int sorted[maxarrays][maxsamples];
  boolean done;

  i[sel] = (i[sel] + 1) % samples;  // increment counter and roll over if necessary. -  % (modulo operator) rolls over variable
  smoothArray[sel][i[sel]] = raw;   // input new data into the oldest slot

  for (j = 0; j < samples; j++) {  // transfer data array into anther array for sorting and averaging
    sorted[sel][j] = smoothArray[sel][j];
  }

  done = 0;            // flag to know when we're done sorting
  while (done != 1) {  // simple swap sort, sorts numbers from lowest to highest
    done = 1;
    for (j = 0; j < (samples - 1); j++) {
      if (sorted[sel][j] > sorted[sel][j + 1]) {  // numbers are out of order - swap
        temp = sorted[sel][j + 1];
        sorted[sel][j + 1] = sorted[sel][j];
        sorted[sel][j] = temp;
        done = 0;
      }
    }
  }

  // throw out top and bottom 15% of samples - limit to throw out at least one from top and bottom
  bottom = max(((samples * 15) / 100), 1);
  top = min((((samples * 85) / 100) + 1), (samples - 1));  // the + 1 is to make up for asymmetry caused by integer rounding
  k = 0;
  total = 0;
  for (j = bottom; j < top; j++) {
    total += sorted[sel][j];  // total remaining indices
    k++;
  }
  return total / k;  // divide by number of samples
}

float fscale(float inputValue, float originalMin, float originalMax, float newBegin, float newEnd, float curve) {

  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;


  // condition curve parameter
  // limit range

  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;

  curve = (curve * -.1);   // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output
  curve = pow(10, curve);  // convert linear scale into lograthimic exponent for other pow function

  /*
   Serial.println(curve * 100, DEC);   // multply by 100 to preserve resolution
   Serial.println();
   */

  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }

  // Zero Refference the values
  OriginalRange = originalMax - originalMin;

  if (newEnd > newBegin) {
    NewRange = newEnd - newBegin;
  } else {
    NewRange = newBegin - newEnd;
    invFlag = 1;
  }

  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal = zeroRefCurVal / OriginalRange;  // normalize to 0 - 1 float

  /*
  Serial.print(OriginalRange, DEC);
   Serial.print("   ");
   Serial.print(NewRange, DEC);
   Serial.print("   ");
   Serial.println(zeroRefCurVal, DEC);
   Serial.println();
   */

  // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine
  if (originalMin > originalMax) {
    return 0;
  }

  if (invFlag == 0) {
    rangedValue = (pow(normalizedCurVal, curve) * NewRange) + newBegin;

  } else  // invert the ranges
  {
    rangedValue = newBegin - (pow(normalizedCurVal, curve) * NewRange);
  }

  return rangedValue;
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~