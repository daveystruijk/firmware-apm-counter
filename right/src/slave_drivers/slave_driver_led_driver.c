#include "slave_drivers/slave_driver_led_driver.h"
#include "led_driver.h"

#define BUFFER_SIZE (LED_DRIVER_LED_COUNT + 1)

uint8_t ledsBuffer[BUFFER_SIZE] = {FRAME_REGISTER_PWM_FIRST};
uint8_t ledDriverStates[2] = {0};

uint8_t ledControlBufferLeft[] = {
    FRAME_REGISTER_LED_CONTROL_FIRST,
    0b01111111, // key row 1
    0b00111111, // display row 1
    0b01011111, // keys row 2
    0b00111111, // display row 2
    0b01011111, // keys row 3
    0b00111111, // display row 3
    0b01111101, // keys row 4
    0b00011111, // display row 4
    0b00101111, // keys row 5
    0b00011111, // display row 5
    0b00000000, // keys row 6
    0b00011111, // display row 6
    0b00000000, // keys row 7
    0b00011111, // display row 7
    0b00000000, // keys row 8
    0b00011111, // display row 8
    0b00000000, // keys row 9
    0b00011111, // display row 9
};

uint8_t ledControlBufferRight[] = {
    FRAME_REGISTER_LED_CONTROL_FIRST,
    0b01111111, // key row 1
    0b00000000, // no display
    0b01111111, // keys row 2
    0b00000000, // no display
    0b01111111, // keys row 3
    0b00000000, // no display
    0b01111111, // keys row 4
    0b00000000, // no display
    0b01111010, // keys row 5
    0b00000000, // no display
    0b00000000, // keys row 6
    0b00000000, // no display
    0b00000000, // keys row 7
    0b00000000, // no display
    0b00000000, // keys row 8
    0b00000000, // no display
    0b00000000, // keys row 9
    0b00000000, // no display
};

uint8_t setFunctionFrameBuffer[] = {LED_DRIVER_REGISTER_FRAME, LED_DRIVER_FRAME_FUNCTION};
uint8_t setShutdownModeNormalBuffer[] = {LED_DRIVER_REGISTER_SHUTDOWN, SHUTDOWN_MODE_NORMAL};
uint8_t setFrame1Buffer[] = {LED_DRIVER_REGISTER_FRAME, LED_DRIVER_FRAME_1};

void LedSlaveDriver_Init() {
    ledControlBufferLeft[7] |= 0b00000010; // Enable the LED of the ISO key.
    SetLeds(0xff);
}

void LedSlaveDriver_Update(uint8_t ledDriverId) {
    uint8_t *ledDriverPhase = ledDriverStates + ledDriverId;
    uint8_t ledDriverAddress = ledDriverId ? I2C_ADDRESS_LED_DRIVER_LEFT : I2C_ADDRESS_LED_DRIVER_RIGHT;
    uint8_t *ledControlBuffer = ledDriverId ? ledControlBufferLeft : ledControlBufferRight;

    switch (*ledDriverPhase) {
        case LedDriverPhase_SetFunctionFrame:
            I2cAsyncWrite(ledDriverAddress, setFunctionFrameBuffer, sizeof(setFunctionFrameBuffer));
            *ledDriverPhase = LedDriverPhase_SetShutdownModeNormal;
            break;
        case LedDriverPhase_SetShutdownModeNormal:
            I2cAsyncWrite(ledDriverAddress, setShutdownModeNormalBuffer, sizeof(setShutdownModeNormalBuffer));
            *ledDriverPhase = LedDriverPhase_SetFrame1;
            break;
        case LedDriverPhase_SetFrame1:
            I2cAsyncWrite(ledDriverAddress, setFrame1Buffer, sizeof(setFrame1Buffer));
            *ledDriverPhase = LedDriverPhase_InitLedControlRegisters;
            break;
        case LedDriverPhase_InitLedControlRegisters:
            I2cAsyncWrite(ledDriverAddress, ledControlBuffer, sizeof(ledControlBufferLeft));
            *ledDriverPhase = LedDriverPhase_Initialized;
            break;
        case LedDriverPhase_Initialized:
            I2cAsyncWrite(ledDriverAddress, ledsBuffer, BUFFER_SIZE);
            break;
    }
}

void SetLeds(uint8_t ledBrightness)
{
    memset(ledsBuffer+1, ledBrightness, LED_DRIVER_LED_COUNT);
}
