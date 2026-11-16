/*
  Switch Macro - Arduino IDE version
  Board: Arduino Pro Micro / Leonardo (ATmega32U4)
  Library: "NintendoSwitchControlLibrary" by lefmarna (install via Library Manager)

  IMPORTANT: the board's USB VID/PID must be changed to the HORI Pokken pad
  (VID 0x0F0D, PID 0x0092) in boards.txt, otherwise the Switch ignores it.
  See the setup notes in the chat reply.

  Edit the macros below, pick INITIAL_MACRO, then Upload as normal.
*/

#include <NintendoSwitchControlLibrary.h>

// ---- Choose which macro runs on boot: MACRO_STARTUP, MACRO_MAIN, MACRO_DOWN, MACRO_A ----
#define INITIAL_MACRO MACRO_STARTUP

// ---- Button bit definitions (same values as your original Joystick.c) ----
#define SW_Y       0x0001
#define SW_B       0x0002
#define SW_A       0x0004
#define SW_X       0x0008
#define SW_L       0x0010
#define SW_R       0x0020
#define SW_ZL      0x0040
#define SW_ZR      0x0080
#define SW_SELECT  0x0100   // minus
#define SW_START   0x0200   // plus
#define SW_LCLICK  0x0400
#define SW_RCLICK  0x0800
#define SW_HOME    0x1000
#define SW_CAPTURE 0x2000

struct MacroStep {
  uint16_t buttons;   // button mask
  uint8_t  ly;        // 0 = up, 128 = center, 255 = down
  uint8_t  lx;        // 0 = left, 128 = center, 255 = right
  uint16_t duration;  // frames (~60 per second)
};

enum MacroMode { MACRO_STARTUP, MACRO_MAIN, MACRO_DOWN, MACRO_A };



// ---- MAIN (loops): OoT jumpslash fly glitch ----
// Tune these timings (frames, ~60 per second) until the glitch is consistent.
#define T_ZL_HOLD    1   // hold ZL (lock-on)
#define T_ZL_GAP     3    // ZL released between hold and press
#define T_FWD_A      0    // stick forward + A
#define T_R_HOLD     12   // hold R while stick is pushed left/right
#define T_BUFFER     9    // buffer frame: R let go, stick still held
#define T_NUT_BUFFER     10    // buffer frame: R let go, stick still held
#define T_ZR_PRESS   1    // ZR press
#define T_RESET      8   // neutral wait before the loop repeats

#define UP    0     // ly value for stick forward
#define LEFT  0     // lx value for stick left
#define RIGHT 255   // lx value for stick right
#define MID   128

// ---- STARTUP (runs once, then switches to MAIN) ----
const MacroStep StartupSequence[] = {
  { SW_R | SW_L,    MID, MID,   5  },
  { 0,              MID, MID,   5  },  
  { SW_R | SW_L,    MID, MID,   5  },  
  { 0,              MID, MID,   5  },  
  { SW_R | SW_L,    MID, MID,   5  },
  { 0,              MID, MID,   5  }, 
  { SW_R | SW_L,    MID, MID,   5  },
  { 0,              MID, MID,   5  },  
  { SW_R | SW_L,    MID, MID,   5  },  
  { 0,              MID, MID,   5  },  
  { SW_R | SW_L,    MID, MID,   5  },
  { SW_A,           MID, MID,   5  },  // hold ZL
  { 0,              MID, MID,   20  },  // hold ZL
  { SW_A,           MID, MID,   5  },  // hold ZL
  { 0,              MID, MID,   60  },  // hold ZL
  { SW_ZL,          MID, MID,   T_ZL_HOLD  },  // hold ZL
  { SW_ZL | SW_ZR,  MID, MID,   T_ZR_PRESS },  // press ZL
  { 0,  MID, MID,   20 },
  { SW_ZL,          180, MID,   1  },
  { SW_ZL | SW_A,   UP,  MID,   T_FWD_A    },  // move forward and press A (ZL kept held)
  { SW_R,   MID, MID,   T_R_HOLD   },  // hold R,
  { SW_R,   MID, RIGHT, T_R_HOLD   },  // hold R, stick right
  { 0,      MID, MID, 6   },  // buffer frame: let go of R
  { SW_R,   MID, MID,   T_R_HOLD   },  // hold R
  { SW_R,   MID, LEFT,  T_R_HOLD   },  // hold R, stick left
  { SW_ZR,  MID, MID,   1 },  // press ZR
  { 0,      MID, MID,  0   },  // buffer frame: let go of R
  { SW_ZR,  MID, MID,   0 },  // press ZR
  { 0,      MID, MID,  0   },  // buffer frame: let go of R
  { SW_ZR,  MID, MID,   0 },  // press ZR
  { 0,      MID, MID,  0   },  // buffer frame: let go of R
  { SW_ZR,  MID, MID,   0 },  // press ZR
  { 0,      MID, MID,  0   },  // buffer frame: let go of R
  { SW_ZR,  MID, MID,   0 },  // press ZR
  { 0,      MID, MID,  0   },  // buffer frame: let go of R
  { SW_ZR,  MID, MID,   0 },  // press ZR
  { 0,      MID, MID,  0   },  // buffer frame: let go of R
  { SW_ZR,  MID, MID,   0 },  // press ZR
  { 0,      MID, MID,  0   },  // buffer frame: let go of R

};

const MacroStep MainMacro[] = {
  { SW_R,   MID, MID,   T_R_HOLD   },  // hold R,
  { SW_R,   MID, RIGHT, T_R_HOLD   },  // hold R, stick right
  { 0,      MID, MID, 8   },  // buffer frame: let go of R
  { SW_R,   MID, MID,   T_R_HOLD   },  // hold R
  { SW_R,   MID, LEFT,  T_R_HOLD   },  // hold R, stick left
  { 0,          MID, MID,  3   },  // buffer frame: let go of R
  { SW_ZR,  MID, MID,   0 },  // press ZR
  { 0,      MID, MID,  0   },  // buffer frame: let go of R
  { SW_ZR,  MID, MID,   0 },  // press ZR
  { 0,      MID, MID,  0   },  // buffer frame: let go of R
  { SW_ZR,  MID, MID,   0 },  // press ZR
  { 0,      MID, MID,  0   },  // buffer frame: let go of R
  { SW_ZR,  MID, MID,   0 },  // press ZR
  { 0,      MID, MID,  0   },  // buffer frame: let go of R
  { SW_ZR,  MID, MID,   0 },  // press ZR
  { 0,      MID, MID,  0   },  // buffer frame: let go of R
  { SW_ZR,  MID, MID,   0 },  // press ZR
  { 0,      MID, MID,  0   },  // buffer frame: let go of R
  { SW_ZR,      MID, MID,  0   },  // buffer frame: let go of R
};

// ---- DOWN (loops) ----
const MacroStep DownMacro[] = {
  { 0, 255, 128, 90 },
};

// ---- A (loops) ----
const MacroStep AMacro[] = {
  { 0,    128, 128, 5 },
  { SW_A, 128, 128, 5 },
};

#define LEN(a) (sizeof(a) / sizeof((a)[0]))

// ---- Engine ----
static const uint32_t FRAME_US = 16667;   // 60 fps

MacroMode       currentMode = INITIAL_MACRO;
const MacroStep* currentMacro;
size_t          macroLength;
size_t          currentStep = 0;
uint16_t        framesLeft = 0;
uint32_t        nextFrameAt = 0;

// Order matches bits 0..13 of the button mask
const uint16_t BUTTON_MAP[14] = {
  Button::Y, Button::B, Button::A, Button::X,
  Button::L, Button::R, Button::ZL, Button::ZR,
  Button::MINUS, Button::PLUS, Button::LCLICK, Button::RCLICK,
  Button::HOME, Button::CAPTURE
};

void selectMacro(MacroMode m) {
  currentMode = m;
  switch (m) {
    case MACRO_STARTUP: currentMacro = StartupSequence; macroLength = LEN(StartupSequence); break;
    case MACRO_DOWN:    currentMacro = DownMacro;       macroLength = LEN(DownMacro);       break;
    case MACRO_A:       currentMacro = AMacro;          macroLength = LEN(AMacro);          break;
    default:            currentMacro = MainMacro;       macroLength = LEN(MainMacro);       break;
  }
  currentStep = 0;
  framesLeft = currentMacro[0].duration;
}

void applyStep(const MacroStep &s) {
  for (uint8_t i = 0; i < 14; i++) {
    if (s.buttons & (1u << i)) SwitchControlLibrary().pressButton(BUTTON_MAP[i]);
    else                       SwitchControlLibrary().releaseButton(BUTTON_MAP[i]);
  }
  SwitchControlLibrary().moveLeftStick(s.lx, s.ly);
  SwitchControlLibrary().moveRightStick(128, 128);
  SwitchControlLibrary().sendReport();
}

void advanceStep() {
  currentStep++;
  if (currentStep >= macroLength) {
    if (currentMode == MACRO_STARTUP) {
      selectMacro(MACRO_MAIN);
      return;
    }
    currentStep = 0;
  }
  framesLeft = currentMacro[currentStep].duration;
}

void setup() {
  selectMacro(INITIAL_MACRO);
  nextFrameAt = micros();
}

void loop() {
  uint32_t now = micros();
  if ((int32_t)(now - nextFrameAt) < 0) return;
  nextFrameAt += FRAME_US;

  applyStep(currentMacro[currentStep]);

  if (framesLeft > 0) framesLeft--;
  else advanceStep();
}
