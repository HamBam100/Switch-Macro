/*
Nintendo Switch Fightstick - Macro Player
Based on LUFA library and HORI Pokken Tournament Pro Pad design
*/

//avrdude -v -patmega32u4 -cavr109 -PCOM6 -b57600 -D -Uflash:w:Joystick.hex:i

// Set the initial macro mode here
#define INITIAL_MACRO MACRO_A // MACRO_STARTUP, MACRO_MAIN, MACRO_DOWN, MACRO_A

#include "Joystick.h"

// Button definitions
#define SWITCH_Y       0x0001
#define SWITCH_B       0x0002
#define SWITCH_A       0x0004
#define SWITCH_X       0x0008
#define SWITCH_L       0x0010
#define SWITCH_R       0x0020
#define SWITCH_ZL      0x0040
#define SWITCH_ZR      0x0080
#define SWITCH_SELECT  0x0100
#define SWITCH_START   0x0200
#define SWITCH_LCLICK  0x0400
#define SWITCH_RCLICK  0x0800
#define SWITCH_HOME    0x1000
#define SWITCH_CAPTURE 0x2000

// Macro step structure
typedef struct {
    uint16_t buttons;  // Button combination to press
    uint8_t  ly;       // Left Stick Y (0=Up, 128=Center, 255=Down)
    uint8_t  lx;       // Left Stick X (0=Left, 128=Center, 255=Right)
    uint16_t duration; // Duration in frames (~60 frames/sec)
} MacroStep_t;

// Macro mode enum
typedef enum {
    MACRO_STARTUP,  // Running startup sequence
    MACRO_MAIN,     // Running main loop
    MACRO_DOWN,     // Running Down loop
    MACRO_A         // Running A loop
} MacroMode_t;

// --- STARTUP SEQUENCE (runs once on boot) ---
const MacroStep_t StartupSequence[] = {
    { 0,            128,  128,  120 },  // Wait 2 seconds
    { SWITCH_A,     128,  128,  10  },  // Press A
    { 0,            128,  128,  30  },  // Wait 0.5 sec
    { SWITCH_A,     128,  128,  10  },  // Press A again
    { 0,            128,  128,  60  },  // Wait 1 sec before main macro
};
const size_t StartupLength = sizeof(StartupSequence) / sizeof(StartupSequence[0]);

// --- MAIN MACRO SEQUENCE (loops indefinitely) ---
const MacroStep_t MainMacro[] = {
    { 0,             128,  128,  90   },  // Wait 1.5 sec (neutral position)
    { SWITCH_START,  128,  128,  5    },  // Press START (+) for ~83ms
    { 0,             128,  128,  135  },  // Wait 2.25 sec (neutral)
    { SWITCH_Y,      128,  128,  5    },  // Press Y for ~83ms
    { 0,             128,  128,  45   },  // Wait 0.75 sec (neutral)
    { 0,             128,  255,  8    },  // Hold left stick RIGHT for ~133ms
    { 0,             128,  128,  5    },  // Wait ~83ms (neutral)
    { 0,             128,  255,  8    },  // Hold left stick RIGHT for ~133ms
    { 0,             128,  128,  5    },  // Wait ~83ms (neutral)
    { 0,             128,  255,  8    },  // Hold left stick RIGHT for ~133ms
    { 0,             128,  128,  5    },  // Wait ~83ms (neutral)
    { 0,             128,  255,  8    },  // Hold left stick RIGHT for ~133ms
    { 0,             128,  128,  5    },  // Wait ~83ms (neutral)
    { 0,             128,  255,  8    },  // Hold left stick RIGHT for ~133ms
    { 0,             128,  128,  5    },  // Wait ~83ms (neutral)
    { 0,             128,  255,  8    },  // Hold left stick RIGHT for ~133ms
    { 0,             128,  128,  5    },  // Wait ~83ms (neutral)
    { 0,             255,  128,  8    },  // Hold left stick DOWN for ~133ms
    { 0,             128,  128,  5    },  // Wait ~83ms (neutral)
    { 0,             255,  128,  8    },  // Hold left stick DOWN for ~133ms
    { 0,             128,  128,  5    },  // Wait ~83ms (neutral)
    { 0,             255,  128,  8    },  // Hold left stick DOWN for ~133ms
    { 0,             128,  128,  10   },  // Wait ~167ms (neutral)
    { SWITCH_A,      128,  128,  5    },  // Press A for ~83ms
    { 0,             128,  128,  100  },  // Wait ~1.67 sec (neutral)
    { SWITCH_A,      128,  128,  5    },  // Press A for ~83ms
    { 0,             128,  128,  520  },  // Wait ~8.67 sec (neutral)
    { 0,             255,  128,  10   },  // Hold left stick DOWN for ~167ms
    { SWITCH_L,      128,  128,  5    },  // Press L for ~83ms
};
const size_t MainMacroLength = sizeof(MainMacro) / sizeof(MainMacro[0]);

// --- DOWN MACRO SEQUENCE (loops indefinitely) ---
const MacroStep_t DownMacro[] = {
    { 0,  255,  128,  90 }  // Hold left stick DOWN for 1.5 sec
};
const size_t DownMacroLength = sizeof(DownMacro) / sizeof(DownMacro[0]);

// --- A MACRO SEQUENCE (loops indefinitely) ---
const MacroStep_t AMacro[] = {
    { 0,         128,  128,  5 },  // Wait ~83ms (neutral position)
    { SWITCH_A,  128,  128,  5 }   // Press A for ~83ms
};
const size_t AMacroLength = sizeof(AMacro) / sizeof(AMacro[0]);

// Global macro state
volatile MacroMode_t CurrentMode = INITIAL_MACRO;
volatile size_t      CurrentStep = 0;
volatile uint16_t    StepTimer = 0;

// Main entry point
int main(void) {
    SetupHardware();
    GlobalInterruptEnable();
    
    // Initialize with selected macro mode
    CurrentMode = INITIAL_MACRO;
    CurrentStep = 0;
    
    // Set initial timer based on mode
    if (CurrentMode == MACRO_STARTUP) {
        StepTimer = StartupSequence[0].duration;
    } else if (CurrentMode == MACRO_DOWN) {
        StepTimer = DownMacro[0].duration;
    } else if (CurrentMode == MACRO_A) {
        StepTimer = AMacro[0].duration;
    } else {
        StepTimer = MainMacro[0].duration;
    }
    
    for (;;) {
        HID_Task();
        USB_USBTask();
    }
}

// Hardware setup
void SetupHardware(void) {
    // Disable watchdog
    MCUSR &= ~(1 << WDRF);
    wdt_disable();
    
    // Disable clock division
    clock_prescale_set(clock_div_1);
    
    // Setup ports for buttons (if using physical buttons)
    DDRD  &= ~0xFF;
    PORTD |=  0xFF;
    DDRB  &= ~0xFF;
    PORTB |=  0xFF;
    
    // Initialize USB
    USB_Init();
}

// USB Event Handlers
void EVENT_USB_Device_Connect(void) {
    // Device connected
}

void EVENT_USB_Device_Disconnect(void) {
    // Device disconnected
}

void EVENT_USB_Device_ConfigurationChanged(void) {
    bool ConfigSuccess = true;
    ConfigSuccess &= Endpoint_ConfigureEndpoint(JOYSTICK_OUT_EPADDR, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);
    ConfigSuccess &= Endpoint_ConfigureEndpoint(JOYSTICK_IN_EPADDR, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);
}

void EVENT_USB_Device_ControlRequest(void) {
    switch (USB_ControlRequest.bRequest) {
        case HID_REQ_GetReport:
            if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE)) {
                USB_JoystickReport_Input_t JoystickInputData;
                GetNextReport(&JoystickInputData);
                Endpoint_ClearSETUP();
                Endpoint_Write_Control_Stream_LE(&JoystickInputData, sizeof(JoystickInputData));
                Endpoint_ClearOUT();
            }
            break;
            
        case HID_REQ_SetReport:
            if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE)) {
                USB_JoystickReport_Output_t JoystickOutputData;
                Endpoint_ClearSETUP();
                Endpoint_Read_Control_Stream_LE(&JoystickOutputData, sizeof(JoystickOutputData));
                Endpoint_ClearIN();
            }
            break;
    }
}

// HID Task - handles IN/OUT endpoints
void HID_Task(void) {
    if (USB_DeviceState != DEVICE_STATE_Configured)
        return;
    
    // Handle OUT endpoint
    Endpoint_SelectEndpoint(JOYSTICK_OUT_EPADDR);
    if (Endpoint_IsOUTReceived()) {
        if (Endpoint_IsReadWriteAllowed()) {
            USB_JoystickReport_Output_t JoystickOutputData;
            Endpoint_Read_Stream_LE(&JoystickOutputData, sizeof(JoystickOutputData), NULL);
        }
        Endpoint_ClearOUT();
    }
    
    // Handle IN endpoint
    Endpoint_SelectEndpoint(JOYSTICK_IN_EPADDR);
    if (Endpoint_IsINReady()) {
        USB_JoystickReport_Input_t JoystickInputData;
        GetNextReport(&JoystickInputData);
        Endpoint_Write_Stream_LE(&JoystickInputData, sizeof(JoystickInputData), NULL);
        Endpoint_ClearIN();
    }
}

// Generate the next report (called each USB poll)
void GetNextReport(USB_JoystickReport_Input_t* const ReportData) {
    // Clear report
    memset(ReportData, 0, sizeof(USB_JoystickReport_Input_t));
    
    // Get current macro based on mode
    const MacroStep_t* CurrentMacro;
    size_t MacroLength;

    
    if (CurrentMode == MACRO_STARTUP) {
        CurrentMacro = StartupSequence;
        MacroLength = StartupLength;
    } else if (CurrentMode == MACRO_DOWN) {
        CurrentMacro = DownMacro;
        MacroLength = DownMacroLength;
    } else if (CurrentMode == MACRO_A) {
        CurrentMacro = AMacro;
        MacroLength = AMacroLength;
    } else {
        CurrentMacro = MainMacro;
        MacroLength = MainMacroLength;
    }
    
    // Load current step data
    ReportData->Button = CurrentMacro[CurrentStep].buttons;
    ReportData->LX = CurrentMacro[CurrentStep].lx;
    ReportData->LY = CurrentMacro[CurrentStep].ly;
    ReportData->RX = 128;  // Right stick centered
    ReportData->RY = 128;
    ReportData->HAT = 0x08;  // HAT centered
    
    // Decrement step timer
    if (StepTimer > 0) {
        StepTimer--;
        return;
    }
    
    // Timer expired - advance to next step
    CurrentStep++;
    
    // Check if sequence is complete
    if (CurrentStep >= MacroLength) {
        if (CurrentMode == MACRO_STARTUP) {
            // Startup complete - switch to main macro
            CurrentMode = MACRO_MAIN;
            CurrentStep = 0;
            StepTimer = MainMacro[0].duration;
        } else {
            // Other macros loop back to start
            CurrentStep = 0;
            StepTimer = CurrentMacro[0].duration;
        }
    } else {
        // Load next step duration
        StepTimer = CurrentMacro[CurrentStep].duration;
    }
}