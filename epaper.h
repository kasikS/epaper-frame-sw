#ifndef EPAPER_H_
#define EPAPER_H_

#include <stdint.h>

//ePaper Module from Waveshare, it's this one: https://www.waveshare.com/wiki/5.65inch_e-Paper_Module_(F)

// Display resolution
#define EPD_WIDTH       600
#define EPD_HEIGHT      448

/**********************************
Color Index
**********************************/
#define EPD_5IN65F_BLACK   0x0	/// 000
#define EPD_5IN65F_WHITE   0x1	///	001
#define EPD_5IN65F_GREEN   0x2	///	010
#define EPD_5IN65F_BLUE    0x3	///	011
#define EPD_5IN65F_RED     0x4	///	100
#define EPD_5IN65F_YELLOW  0x5	///	101
#define EPD_5IN65F_ORANGE  0x6	///	110
#define EPD_5IN65F_CLEAN   0x7	///	111   unavailable  Afterimage


void SetupEPaperDisplay(void);

void SetupEPaperForData(void);
void SendEPaperData(const uint8_t* data, int len );
void FlushAndDisplayEPaper(void);

void ClearEpaper(uint8_t color);

void EPD_5IN65F_Show7Block(void);
void StopEPaperDisplay(void);

#endif
