

#include "userinterface.h"

#include <stdbool.h>
#include <string.h>

#include "asmUtil.h"
#include "bitmaps.h"
#include "board.h"
#include "comms.h"
#include "cpu.h"
#include "epd.h"
#include "font.h"
#include "lut.h"
#include "printf.h"
#include "screen.h"
#include "settings.h"
#include "sleep.h"
#include "spi.h"
#include "syncedproto.h"  // for APmac / Channel
#include "timer.h"

extern uint8_t __xdata mSelfMac[8];
extern uint8_t __xdata currentChannel;
extern uint8_t __xdata APmac[];

static const uint8_t __code fwVersion = FW_VERSION;
static const char __code fwVersionSuffix[] = FW_VERSION_SUFFIX;

void showSplashScreen() {
    epdSetup();

#if (SCREEN_WIDTH == 152)  // 1.54"
    clearScreen();
    setColorMode(EPD_MODE_NORMAL, EPD_MODE_INVERT);
    selectLUT(1);
    epdPrintBegin(12, 2, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("Starting!");
    epdPrintEnd();

    loadRawBitmap(solum, 8, 34, EPD_COLOR_BLACK);
    loadRawBitmap(hacked, 32, 46, EPD_COLOR_RED);

    epdPrintBegin(5, 136, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_RED);
    epdpr("%02X%02X", mSelfMac[7], mSelfMac[6]);
    epdpr("%02X%02X", mSelfMac[5], mSelfMac[4]);
    epdpr("%02X%02X", mSelfMac[3], mSelfMac[2]);
    epdpr("%02X%02X", mSelfMac[1], mSelfMac[0]);
    epdPrintEnd();

    epdPrintBegin(2, 120, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("zbs154v033 %d.%d.%d%s", fwVersion / 100, (fwVersion % 100) / 10, (fwVersion % 10), fwVersionSuffix);
    epdPrintEnd();
    draw();
#endif

#if (SCREEN_WIDTH == 128)  // 2.9"
    selectLUT(EPD_LUT_NO_REPEATS);
    clearScreen();
    setColorMode(EPD_MODE_NORMAL, EPD_MODE_INVERT);

    epdPrintBegin(0, 295, EPD_DIRECTION_Y, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("Starting!");
    epdPrintEnd();

    epdPrintBegin(80, 295, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("zbs29v033 %d.%d.%d%s", fwVersion / 100, (fwVersion % 100) / 10, (fwVersion % 10), fwVersionSuffix);
    epdPrintEnd();

    epdPrintBegin(105, 270, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_RED);
    epdpr("MAC: %02X:%02X", mSelfMac[7], mSelfMac[6]);
    epdpr(":%02X:%02X", mSelfMac[5], mSelfMac[4]);
    epdpr(":%02X:%02X", mSelfMac[3], mSelfMac[2]);
    epdpr(":%02X:%02X", mSelfMac[1], mSelfMac[0]);
    epdPrintEnd();

    uint8_t __xdata buffer[17];
    spr(buffer, "%02X%02X", mSelfMac[7], mSelfMac[6]);
    spr(buffer + 4, "%02X%02X", mSelfMac[5], mSelfMac[4]);
    spr(buffer + 8, "%02X%02X", mSelfMac[3], mSelfMac[2]);
    spr(buffer + 12, "%02X%02X", mSelfMac[1], mSelfMac[0]);
    printBarcode(buffer, 120, 284);

    loadRawBitmap(solum, 0, 0, EPD_COLOR_BLACK);
    loadRawBitmap(hacked, 16, 12, EPD_COLOR_RED);
    // lutTest();
    //  drawLineVertical(EPD_COLOR_RED, 64, 10, 286);
    //  drawLineVertical(EPD_COLOR_BLACK, 65, 10, 286);

    draw();
    // timerDelay(TIMER_TICKS_PER_SECOND * 4);
#endif
#if (SCREEN_WIDTH == 400)  // 2.9"
    selectLUT(1);
    clearScreen();
    setColorMode(EPD_MODE_NORMAL, EPD_MODE_INVERT);

    epdPrintBegin(300, 296, EPD_DIRECTION_Y, EPD_SIZE_DOUBLE, EPD_COLOR_RED);
    epdpr("Booting!Y");
    epdpr();

    epdPrintBegin(0, 0, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("Starting!");
    epdPrintEnd();

    epdPrintBegin(16, 252, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("zbs42v033 %d.%d.%d%s", fwVersion / 100, (fwVersion % 100) / 10, (fwVersion % 10), fwVersionSuffix);
    epdPrintEnd();
    epdPrintBegin(16, 284, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_RED);
    epdpr("MAC: %02X:%02X", mSelfMac[7], mSelfMac[6]);
    epdpr(":%02X:%02X", mSelfMac[5], mSelfMac[4]);
    epdpr(":%02X:%02X", mSelfMac[3], mSelfMac[2]);
    epdpr(":%02X:%02X", mSelfMac[1], mSelfMac[0]);
    epdPrintEnd();

    loadRawBitmap(solum, 256, 10, EPD_COLOR_BLACK);
    loadRawBitmap(hacked, 264, 22, EPD_COLOR_RED);

    loadRawBitmap(solum, 253, 72, EPD_COLOR_BLACK);
    loadRawBitmap(hacked, 261, 82, EPD_COLOR_RED);

    draw();
#endif
}

void showApplyUpdate() {
    epdSetup();
    setColorMode(EPD_MODE_NORMAL, EPD_MODE_INVERT);

    selectLUT(1);
    clearScreen();
    setColorMode(EPD_MODE_IGNORE, EPD_MODE_NORMAL);
    epdPrintBegin(8, 60, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("Updating!");
    epdPrintEnd();
    drawNoWait();
}

uint8_t __xdata resultcounter = 0;

void showScanningWindow() {
    epdSetup();
    setColorMode(EPD_MODE_NORMAL, EPD_MODE_INVERT);
    selectLUT(EPD_LUT_FAST_NO_REDS);
    clearScreen();
#if (SCREEN_WIDTH == 128)  // 2.9"
    epdPrintBegin(2, 275, EPD_DIRECTION_Y, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("Scanning for APs");
    epdPrintEnd();
    epdPrintBegin(40, 262, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_RED);
    epdpr("Channel - Quality");
    epdPrintEnd();
    loadRawBitmap(receive, 36, 24, EPD_COLOR_BLACK);
#endif
#if (SCREEN_WIDTH == 152)  // 1.54"
    loadRawBitmap(receive, 96, 28, EPD_COLOR_BLACK);
    epdPrintBegin(3, 0, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("Scanning...");
    epdPrintEnd();
#endif
    draw();
    selectLUT(EPD_LUT_FAST);
    resultcounter = 0;
}

void addScanResult(uint8_t channel, uint8_t lqi) {
    if (channel == 11) resultcounter = 0;
#if (SCREEN_WIDTH == 128)
    epdPrintBegin(56 + ((resultcounter % 4) * 16), 282 - (47 * (resultcounter / 4)), EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("%d-%d", channel, lqi);
    epdPrintEnd();
#endif
#if (SCREEN_WIDTH == 152)  // 1.54"
    epdPrintBegin(4 + (47 * (resultcounter / 8)), 31 + (15 * (resultcounter % 8)), EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("%d-%d", channel, lqi);
    epdPrintEnd();
#endif
    resultcounter++;
}

void showAPFound() {
    selectLUT(EPD_LUT_FAST_NO_REDS);
    clearScreen();
#if (SCREEN_WIDTH == 128)
    epdPrintBegin(0, 285, EPD_DIRECTION_Y, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("Waiting for data...");
    epdPrintEnd();
    epdPrintBegin(48, 278, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Found the following AP:");
    epdPrintEnd();
    epdPrintBegin(64, 293, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("AP MAC: %02X:%02X", APmac[7], APmac[6]);
    epdpr(":%02X:%02X", APmac[5], APmac[4]);
    epdpr(":%02X:%02X", APmac[3], APmac[2]);
    epdpr(":%02X:%02X", APmac[1], APmac[0]);
    epdPrintEnd();
    epdPrintBegin(80, 293, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Ch: %d RSSI: %d LQI: %d", currentChannel, mLastRSSI, mLastLqi);
    epdPrintEnd();

    epdPrintBegin(103, 258, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Tag MAC: %02X:%02X", mSelfMac[7], mSelfMac[6]);
    epdpr(":%02X:%02X", mSelfMac[5], mSelfMac[4]);
    epdpr(":%02X:%02X", mSelfMac[3], mSelfMac[2]);
    epdpr(":%02X:%02X", mSelfMac[1], mSelfMac[0]);
    epdPrintEnd();

    uint8_t __xdata buffer[17];
    spr(buffer, "%02X%02X", mSelfMac[7], mSelfMac[6]);
    spr(buffer + 4, "%02X%02X", mSelfMac[5], mSelfMac[4]);
    spr(buffer + 8, "%02X%02X", mSelfMac[3], mSelfMac[2]);
    spr(buffer + 12, "%02X%02X", mSelfMac[1], mSelfMac[0]);
    printBarcode(buffer, 120, 253);
    loadRawBitmap(receive, 36, 14, EPD_COLOR_BLACK);
#endif
#if (SCREEN_WIDTH == 152)  // 1.54"
    epdPrintBegin(25, 0, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("Waiting");
    epdPrintEnd();
    epdPrintBegin(3, 32, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("for data...");
    epdPrintEnd();

    epdPrintBegin(5, 64, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("AP MAC:");
    epdPrintEnd();
    epdPrintBegin(5, 80, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("%02X%02X", APmac[7], APmac[6]);
    epdpr("%02X%02X", APmac[5], APmac[4]);
    epdpr("%02X%02X", APmac[3], APmac[2]);
    epdpr("%02X%02X", APmac[1], APmac[0]);
    epdPrintEnd();

    epdPrintBegin(5, 96, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Ch:%d rssi:%d lqi:%d", currentChannel, mLastRSSI, mLastLqi);
    epdPrintEnd();

    epdPrintBegin(5, 120, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Tag MAC:");
    epdPrintEnd();

    epdPrintBegin(5, 136, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_RED);
    epdpr("%02X%02X", mSelfMac[7], mSelfMac[6]);
    epdpr("%02X%02X", mSelfMac[5], mSelfMac[4]);
    epdpr("%02X%02X", mSelfMac[3], mSelfMac[2]);
    epdpr("%02X%02X", mSelfMac[1], mSelfMac[0]);
    epdPrintEnd();
#endif
    draw();
}

void showNoAP() {
    selectLUT(EPD_LUT_NO_REPEATS);
    clearScreen();
#if (SCREEN_WIDTH == 128)  // 1.54"
    epdPrintBegin(0, 285, EPD_DIRECTION_Y, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("No AP found :(");
    epdPrintEnd();
    epdPrintBegin(48, 285, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("We'll try again in a");
    epdPrintEnd();
    epdPrintBegin(64, 285, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("little while...");
    epdPrintEnd();
    loadRawBitmap(receive, 36, 24, EPD_COLOR_BLACK);
    loadRawBitmap(failed, 42, 26, EPD_COLOR_RED);
#endif
#if (SCREEN_WIDTH == 152)  // 1.54"
    epdPrintBegin(40, 0, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("No AP");
    epdPrintEnd();
    epdPrintBegin(22, 32, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("found :(");
    epdPrintEnd();

    epdPrintBegin(8, 76, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("We'll try again in");
    epdPrintEnd();
    epdPrintBegin(25, 92, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("a little while");
    epdPrintEnd();
#endif
    draw();
}