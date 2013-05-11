/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "glcd.h"

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// GLCD graph variable from main.c
extern unsigned short tmp_start;

// GLCD graph variable
unsigned short CS = 0, yadd_off, current = 0;
int sign = 0, disp = 0, tmp = 0;

// AxisData Buffer typedef struct from stm32f4xx_it.h
extern AXISDATA mAxisData;
extern AXISBUF  mAxisBuf;

/* GLCD Raw Data ---------------------------------------*/
unsigned char intro_logo[1024] = {
   0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xFF , 0xFD , 0xE7 , 0x80 , 0x0C , 0x00 , 0x7F 
 , 0xFF , 0xFF , 0xFB , 0xDB , 0xFF , 0xDD , 0xEF , 0x7F 
 , 0xFF , 0xFF , 0xF7 , 0xBD , 0xFF , 0xBD , 0xEF , 0x7F 
 , 0xFF , 0xFF , 0xF7 , 0xBD , 0xFF , 0x7D , 0xEF , 0x7F 
 , 0xFF , 0xFF , 0xF7 , 0xBD , 0xFF , 0x7D , 0xEF , 0x7F 
 , 0xFF , 0xFF , 0xF7 , 0xBD , 0xFF , 0xBD , 0xEF , 0x7F 
 , 0xFF , 0xFF , 0xFB , 0x7B , 0xFF , 0xDD , 0xEF , 0x7F 
 , 0xFF , 0xFF , 0xFC , 0xF7 , 0x80 , 0x0D , 0xEF , 0x7F 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xFF , 0xB8 , 0x3F , 0xC3 , 0xFE , 0x1F , 0xFF 
 , 0xFF , 0xFF , 0x77 , 0xFF , 0xB5 , 0xFD , 0xEF , 0xFF 
 , 0xFF , 0xFF , 0x77 , 0xFF , 0xB5 , 0xFD , 0xEF , 0xFF 
 , 0xFF , 0xFF , 0x77 , 0xFF , 0xB5 , 0xFD , 0xEF , 0xFF 
 , 0xFF , 0xFF , 0x77 , 0xFF , 0xB5 , 0xFD , 0xEF , 0xFF 
 , 0xF0 , 0x01 , 0x80 , 0x3F , 0xD3 , 0xFE , 0xDF , 0xFF 
 , 0xFF , 0xFB , 0xFF , 0xFF , 0xFF , 0xFC , 0x0F , 0xFF 
 , 0xFF , 0xF7 , 0xFF , 0xFF , 0xFF , 0xFD , 0xFF , 0xFF 
 , 0xFF , 0xEF , 0xFB , 0x3F , 0xC3 , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xEF , 0xF6 , 0xDF , 0xBD , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xF7 , 0xF6 , 0xDF , 0xBD , 0xFC , 0x0F , 0xFF 
 , 0xFF , 0xFB , 0xF6 , 0xDF , 0xBD , 0xFF , 0xDF , 0xFF 
 , 0xF0 , 0x01 , 0xF6 , 0xDF , 0xBD , 0xFF , 0xEF , 0xFF 
 , 0xFF , 0xFF , 0xF9 , 0xBF , 0xDB , 0xFF , 0xEF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0x81 , 0xFF , 0xEF , 0xFF 
 , 0xF8 , 0x7F , 0xFF , 0xFF , 0xBF , 0xFF , 0xDF , 0xFF 
 , 0xF7 , 0xBF , 0xFF , 0xBF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xF7 , 0xBF , 0xFF , 0xBF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xF7 , 0xBF , 0xFF , 0xBF , 0xD9 , 0xFF , 0xEF , 0xFF 
 , 0xF7 , 0xBF , 0xF8 , 0x0F , 0xB6 , 0xFF , 0xEF , 0xFF 
 , 0xFB , 0x7F , 0xF7 , 0xBF , 0xB6 , 0xFF , 0xEF , 0xFF 
 , 0xF8 , 0x3F , 0xF7 , 0xBF , 0xB6 , 0xFE , 0x03 , 0xFF 
 , 0xF7 , 0xFF , 0xFB , 0xBF , 0xB6 , 0xFD , 0xEF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xCD , 0xFD , 0xEF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFE , 0xEF , 0xFF 
 , 0xFC , 0xFF , 0xF8 , 0x7F , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFB , 0x7F , 0xF6 , 0xBF , 0xE1 , 0xFF , 0xFF , 0xFF 
 , 0xF7 , 0xBF , 0xF6 , 0xBF , 0xDF , 0xFC , 0x00 , 0x7F 
 , 0xF7 , 0xBF , 0xF6 , 0xBF , 0xBF , 0xFF , 0xDF , 0xFF 
 , 0xF7 , 0xBF , 0xF6 , 0xBF , 0xBF , 0xFF , 0xEF , 0xFF 
 , 0xFB , 0x7F , 0xFA , 0x7F , 0xBF , 0xFF , 0xEF , 0xFF 
 , 0xF0 , 0x01 , 0xFF , 0xFF , 0xDF , 0xFF , 0xEF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xC1 , 0xFF , 0xEF , 0xFF 
 , 0xFF , 0xFF , 0xF0 , 0x3F , 0xBF , 0xFC , 0x1F , 0xFF 
 , 0xF8 , 0x7F , 0xFF , 0x7F , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xF6 , 0xBF , 0xFF , 0xBF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xF6 , 0xBF , 0xFF , 0xBF , 0x81 , 0xFF , 0x01 , 0xFF 
 , 0xF6 , 0xBF , 0xF0 , 0x7F , 0xFB , 0xFE , 0xFE , 0xFF 
 , 0xF6 , 0xBF , 0xFF , 0xBF , 0xFD , 0xFD , 0xFF , 0x7F 
 , 0xFA , 0x7F , 0xFF , 0xBF , 0xFD , 0xFD , 0xFF , 0x7F 
 , 0xFF , 0xFF , 0xF0 , 0x7F , 0xFD , 0xFD , 0xBF , 0x7F 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFB , 0xFD , 0x7F , 0x7F 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFE , 0xFE , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFD , 0x01 , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xC3 , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xB5 , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xB5 , 0xFF , 0x0F , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xB5 , 0xFE , 0xFF , 0xFF 
 , 0xF0 , 0x01 , 0xFF , 0xFF , 0xB5 , 0xFD , 0xFF , 0xFF 
 , 0xF7 , 0xBD , 0xFF , 0xFF , 0xD3 , 0xFD , 0xFF , 0xFF 
 , 0xF7 , 0xBD , 0xFF , 0xFF , 0xFF , 0xFD , 0xFF , 0xFF 
 , 0xF7 , 0xBD , 0xFF , 0xFF , 0xFF , 0xFE , 0xFF , 0xFF 
 , 0xF7 , 0xBD , 0xFF , 0xFF , 0x81 , 0xFE , 0x0F , 0xFF 
 , 0xF7 , 0xBD , 0xFF , 0xFF , 0xFB , 0xFD , 0xFF , 0xFF 
 , 0xFB , 0x5B , 0xFF , 0xFF , 0xFD , 0xFF , 0xFF , 0xFF 
 , 0xFC , 0xE7 , 0xFF , 0xFF , 0xFD , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0x83 , 0xFE , 0x1F , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFD , 0xFD , 0xEF , 0xFF 
 , 0xEE , 0x0F , 0xFF , 0xFF , 0xFD , 0xFD , 0xEF , 0xFF 
 , 0xDD , 0xFF , 0xFF , 0xFF , 0x83 , 0xFD , 0xEF , 0xFF 
 , 0xDD , 0xFF , 0xFF , 0xFF , 0xFF , 0xFD , 0xEF , 0xFF 
 , 0xDD , 0xFF , 0xFF , 0xFF , 0xFF , 0xFE , 0xDF , 0xFF 
 , 0xDD , 0xFF , 0xFF , 0xFF , 0xC3 , 0xFC , 0x0F , 0xFF 
 , 0xE0 , 0x0F , 0xFF , 0xFF , 0xB5 , 0xFD , 0xFF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xB5 , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xB5 , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xB5 , 0xFC , 0x00 , 0x7F 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xD3 , 0xFF , 0x7F , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0x3F , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFE , 0xDF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0x81 , 0xFD , 0xEF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFB , 0xFF , 0xFF , 0xFF 
 , 0xF7 , 0xFD , 0xFF , 0xFF , 0xFD , 0xFF , 0xFF , 0xFF 
 , 0xF7 , 0xFD , 0xFF , 0xFF , 0xFD , 0xFE , 0x1F , 0xFF 
 , 0xF7 , 0xFD , 0xFF , 0xFF , 0xFD , 0xFD , 0xAF , 0xFF 
 , 0xF0 , 0x01 , 0xFF , 0xFF , 0x83 , 0xFD , 0xAF , 0xFF 
 , 0xF7 , 0xFD , 0xFF , 0xFF , 0xFF , 0xFD , 0xAF , 0xFF 
 , 0xF7 , 0xFD , 0xFF , 0xFF , 0xFF , 0xFD , 0xAF , 0xFF 
 , 0xF7 , 0xFD , 0xFF , 0xFF , 0xFD , 0xFE , 0x9F , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFD , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFD , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xC0 , 0x7F , 0xFF , 0xFF 
 , 0xF0 , 0x01 , 0xFF , 0xFF , 0xBD , 0xFF , 0xFF , 0xFF 
 , 0xF7 , 0xFD , 0xFF , 0xFF , 0xBD , 0xFF , 0xFF , 0xFF 
 , 0xF7 , 0xFD , 0xFF , 0xFF , 0xDD , 0xFF , 0xFF , 0xFF 
 , 0xF7 , 0xFD , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xF7 , 0xFD , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xF7 , 0xFD , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFB , 0xFB , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFC , 0x07 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFD , 0xE7 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFB , 0xDB , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xF7 , 0xBD , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xF7 , 0xBD , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xF7 , 0xBD , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xF7 , 0xBD , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFB , 0x7B , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFC , 0xF7 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xF0 , 0x01 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0x7D , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0x7D , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0x7D , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0x7D , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0x7D , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xBB , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xC7 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
};

unsigned char basicif[1024] = {
   0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0x80 , 0x01 , 0x00 , 0x01 , 0x00 , 0x01 , 0xFF , 0xFF 
 , 0x84 , 0x61 , 0x04 , 0x41 , 0x00 , 0x41 , 0xFB , 0xBF 
 , 0x88 , 0x91 , 0x06 , 0x41 , 0x00 , 0x81 , 0xFD , 0x7F 
 , 0x88 , 0x91 , 0x05 , 0x41 , 0x07 , 0x01 , 0xFE , 0xFF 
 , 0x88 , 0x91 , 0x04 , 0xC1 , 0x00 , 0x81 , 0xFD , 0x7F 
 , 0x87 , 0xF1 , 0x04 , 0x41 , 0x00 , 0x41 , 0xFB , 0xBF 
 , 0x80 , 0x01 , 0x00 , 0x01 , 0x00 , 0x01 , 0xFF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x80 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x80 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x80 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x93 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0xA4 , 0x80 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0xA4 , 0x80 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0xA4 , 0x80 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x99 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x80 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x80 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x80 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0xA0 , 0x80 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0xBF , 0x80 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0xA0 , 0x80 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x80 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x80 , 0x00 , 0x00 , 0x01 
 , 0x80 , 0x00 , 0x00 , 0x00 , 0x80 , 0x00 , 0x00 , 0x01 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
};

unsigned char plot_x[72] = {
   0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0x80 , 0x01 , 0x00 , 0x01 , 0x00 , 0x01 , 0xFF , 0xFF 
 , 0x84 , 0x61 , 0x04 , 0x41 , 0x00 , 0x41 , 0xFB , 0xBF 
 , 0x88 , 0x91 , 0x06 , 0x41 , 0x00 , 0x81 , 0xFD , 0x7F 
 , 0x88 , 0x91 , 0x05 , 0x41 , 0x07 , 0x01 , 0xFE , 0xFF 
 , 0x88 , 0x91 , 0x04 , 0xC1 , 0x00 , 0x81 , 0xFD , 0x7F 
 , 0x87 , 0xF1 , 0x04 , 0x41 , 0x00 , 0x41 , 0xFB , 0xBF 
 , 0x80 , 0x01 , 0x00 , 0x01 , 0x00 , 0x01 , 0xFF , 0xFF 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
};


unsigned char plot_y[72] = {
   0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0x80 , 0x01 , 0x00 , 0x01 , 0xFF , 0xFF , 0x00 , 0x01 
 , 0x84 , 0x61 , 0x04 , 0x41 , 0xFF , 0xBF , 0x04 , 0x41 
 , 0x88 , 0x91 , 0x06 , 0x41 , 0xFF , 0x7F , 0x02 , 0x81 
 , 0x88 , 0x91 , 0x05 , 0x41 , 0xF8 , 0xFF , 0x01 , 0x01 
 , 0x88 , 0x91 , 0x04 , 0xC1 , 0xFF , 0x7F , 0x02 , 0x81 
 , 0x87 , 0xF1 , 0x04 , 0x41 , 0xFF , 0xBF , 0x04 , 0x41 
 , 0x80 , 0x01 , 0x00 , 0x01 , 0xFF , 0xFF , 0x00 , 0x01 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
};


unsigned char plot_z[72] = {
   0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
 , 0x80 , 0x01 , 0xFF , 0xFF , 0x00 , 0x01 , 0x00 , 0x01 
 , 0x84 , 0x61 , 0xFB , 0xBF , 0x00 , 0x41 , 0x04 , 0x41 
 , 0x88 , 0x91 , 0xF9 , 0xBF , 0x00 , 0x81 , 0x02 , 0x81 
 , 0x88 , 0x91 , 0xFA , 0xBF , 0x07 , 0x01 , 0x01 , 0x01 
 , 0x88 , 0x91 , 0xFB , 0x3F , 0x00 , 0x81 , 0x02 , 0x81 
 , 0x87 , 0xF1 , 0xFB , 0xBF , 0x00 , 0x41 , 0x04 , 0x41 
 , 0x80 , 0x01 , 0xFF , 0xFF , 0x00 , 0x01 , 0x00 , 0x01 
 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
};

unsigned char name_logo[1024] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0xE0,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0xA0,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x00,0x00,0x00,
0x00,0x00,0x00,0xE0,0x20,0x20,0xA0,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0xA0,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,
0xF8,0xF0,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0xC0,0xC0,0x40,0x68,0x30,0x20,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0x60,0x30,0x1C,0x8E,0xC3,
0xE1,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,
0x00,0x00,0x00,0xFF,0x00,0x00,0xFF,0x03,0x06,0x0C,0x18,0x30,0x60,0xC0,0x80,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0xE0,0xB0,0x18,0x0C,0x06,
0x03,0x01,0x03,0x06,0x0C,0x18,0xB0,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0xFF,0xF0,0xFC,0xFE,0xFF,0x83,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0xC0,0xE0,0xF0,0xF0,0xF8,0xF8,0xFC,0xFF,0xF9,0xF8,0xFC,0xFE,0xFF,0xFF,0xFF,
0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,
0x00,0x00,0x00,0xFF,0x00,0x00,0x7F,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x41,0x43,
0x46,0x4C,0x58,0x70,0x60,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x06,0x0C,
0x18,0x30,0x18,0x0C,0x06,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0xFF,0x07,0x1F,0x3F,0x7F,0xFF,0xFE,0xFC,0xFC,0xF8,0xF0,0xF0,0xF8,0xFC,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,0x3F,0x1F,
0x1F,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,
0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0xF8,0x08,0x08,0x08,0x08,0x08,0x08,
0x08,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xE0,0x78,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x1F,0x10,0x10,0x10,0x10,0x10,0x13,0x17,0x17,0x1F,0x1F,0x1F,0x1F,0x1F,
0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x17,0x17,0x17,0x13,0x11,0x10,0x10,0x10,0x10,
0x10,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,
0x00,0x00,0x00,0xFF,0x00,0x00,0x0C,0x1C,0x34,0x77,0xE0,0xC0,0x80,0x00,0x80,0xC0,
0xE0,0x77,0x3C,0x1C,0x0C,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x0E,0x0E,0x0A,0x1B,
0xF1,0xC0,0xF1,0x1B,0x0A,0x0E,0x0E,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x72,0xE2,0xC2,0x02,0x02,0x02,0x02,0x02,
0x02,0xFC,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0xF8,0xF8,0x20,0x20,0x20,0x20,0xF8,0xF8,0x00,0x00,0x00,0xC8,0xC8,0x48,0x48,
0x78,0x78,0x80,0x80,0xF8,0xF8,0x00,0x00,0x00,0x00,0xF8,0xF8,0x80,0x00,0x00,0x00,
0xF8,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,
0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x81,0x83,0x03,0x01,0x81,
0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,
0x80,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x70,0x7F,0x6F,0x40,0x00,0x00,0x00,0x00,
0x00,0x7F,0x00,0x00,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,
0x08,0x09,0x09,0x09,0x09,0x09,0x09,0x09,0x09,0x08,0x08,0x00,0x07,0x07,0x04,0x04,
0x04,0x04,0x04,0x00,0x3F,0x3F,0x00,0x04,0x06,0x07,0x03,0x01,0x03,0x06,0x04,0x00,
0x3F,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,
0x00,0x00,0x00,0x7F,0x00,0x00,0x3F,0x73,0x41,0x00,0x00,0x01,0x01,0x03,0x03,0x03,
0x01,0x00,0x00,0x41,0x73,0x3F,0x00,0x00,0x00,0x00,0x08,0x1C,0x1E,0x33,0x61,0x61,
0x03,0x07,0x3E,0x7C,0x60,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

/* Private functions ---------------------------------------------------------*/

void GLCD_Configuration(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* RCC configuration -------------------------------------------------------*/
  /* Enable GLCD pin */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
  
  /* GPIO configuration ------------------------------------------------------*/
  /* RS, R/W, E pin configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* D0, D1, D2, D3, D4, D5, D6, D7 pin configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | 
                                GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* CS1, CS2, RESET pin configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  // take 1 us to reset GLCD
  GLCD_RESET_LOW();
  delay_us(1);
  GLCD_RESET_HIGH();
  
  // initial work to be done at least once at boot
  GLCD_Handler(RS_READ, CS12, DISPLAY_ON);  // display on
  GLCD_Handler(RS_READ, CS12, START_LINE);  // locate start line
  
  /* Clear GLCD screen */
  GLCD_Clear();
  
  /* GLCD Introduction */
  GLCD_Intro();
  
  /* GLCD Basic Interface */
  GLCD_BasicIF();
  
  // for continuous GLCD display
  //GLCD_CustomIF();
}

void GLCD_Handler(char rs, char cs, uint16_t data) {
  GLCD_ENABLE_LOW();
  delay_us(1);
  GLCD_RW_LOW();
  delay_us(1);
  
  switch(rs) {
  case RS_WRITE : GLCD_RS_HIGH(); break;
  case RS_READ : GLCD_RS_LOW(); break;
  default: delay_us(1); break;
  }
  
  switch(cs) {
  case CS1: GLCD_CS2_LOW(); GLCD_CS1_HIGH(); break;
  case CS2: GLCD_CS1_LOW(); GLCD_CS2_HIGH(); break;
  case CS12: GLCD_CS1_HIGH(); GLCD_CS2_HIGH(); break;
  default: delay_us(1); break;
  }
  
  GLCD_ENABLE_HIGH();  // E Rise
  delay_us(1);
  
  GPIO_Write(GPIOB, data << 8);
  delay_us(1);
  
  GLCD_ENABLE_LOW();
  delay_us(1);
  GLCD_RW_HIGH();
  delay_us(10);
}

void GLCD_Clear(void) {
  char i, j;
  for(i = 0; i < 8; i++) {
    GLCD_Handler(RS_READ, CS12, PAGE_SET | i);    // page set
    for(j = 0; j < 64; j++) {
      GLCD_Handler(RS_READ, CS12, ADDR_SET | j);  // address set
      GLCD_Handler(RS_WRITE, CS12, 0x00);         // write data
    }
  }
}

void GLCD_Data(char data) {
  char i, j;
  for(i = 0; i < 8; i++) {
    GLCD_Handler(RS_READ, CS12, PAGE_SET | i);    // page set
    for(j = 0; j < 64; j++) {
      GLCD_Handler(RS_READ, CS12, ADDR_SET | j);  // address set
      GLCD_Handler(RS_WRITE, CS12, data);         // write data
    }
  }
}

void GLCD_Intro(void) {
  char i, j;
  for(i = 0; i < 8; i++) {
    GLCD_Handler(RS_READ, CS1, PAGE_SET | i);       // page set
    for(j = 0; j < 64; j++) {
      GLCD_Handler(RS_READ, CS1, ADDR_SET | j);     // address set
      GLCD_Handler(RS_WRITE, CS1, intro_logo[8*j-i-1]);  // write data
    }
  }
  
  for(i = 0; i < 8; i++) {
    GLCD_Handler(RS_READ, CS2, PAGE_SET | i);       // page set
    for(j = 64; j < 128; j++) {
      GLCD_Handler(RS_READ, CS2, ADDR_SET | j);     // address set
      GLCD_Handler(RS_WRITE, CS2, intro_logo[8*j-i-1]);  // write data
    }
  }
  
  // Just enough time before main menu
  delay_ms(1000);
}

void GLCD_BasicIF(void) {
  char i, j;
  for(i = 0; i < 8; i++) {
    GLCD_Handler(RS_READ, CS1, PAGE_SET | i);       // page set
    for(j = 0; j < 64; j++) {
      GLCD_Handler(RS_READ, CS1, ADDR_SET | j);     // address set
      GLCD_Handler(RS_WRITE, CS1, basicif[8*(j+1)-i-1]);  // write data
    }
  }
  
  for(i = 0; i < 8; i++) {
    GLCD_Handler(RS_READ, CS2, PAGE_SET | i);       // page set
    for(j = 64; j < 128; j++) {
      GLCD_Handler(RS_READ, CS2, ADDR_SET | j);     // address set
      GLCD_Handler(RS_WRITE, CS2, basicif[8*(j+1)-i-1]);  // write data
    }
  }
  
  // Just enough time before main menu
  //Delay_ms(100);
}

void GLCD_CustomIF(void) {
  char i, j;
  for(i = 0; i < 8; i++) {
    GLCD_Handler(RS_READ, CS1, PAGE_SET | i);               // page set
    for(j = 0; j < 64; j++) {
      GLCD_Handler(RS_READ, CS1, ADDR_SET | j);             // address set
      GLCD_Handler(RS_WRITE, CS1, name_logo[(i*128)+j]);    // write data
    }
  }
  
  for(i = 0; i < 8; i++) {
    GLCD_Handler(RS_READ, CS2, PAGE_SET | i);               // page set
    for(j = 64; j < 128; j++) {
      GLCD_Handler(RS_READ, CS2, ADDR_SET | j);             // address set
      GLCD_Handler(RS_WRITE, CS2, name_logo[(i*128)+j]);    // write data
    }
  }
}

void SelectAxisX(void) {
  int i, j;
  for(i = 1; i <= 8; i++) {
  GLCD_Handler(RS_READ, CS1, PAGE_SET | i);       // page set
  for(j = 1; j <= 9; j++) {
    GLCD_Handler(RS_READ, CS1, ADDR_SET | j);     // address set
    GLCD_Handler(RS_WRITE, CS1, plot_x[8*(j+1)-i-1]);  // write data
    }
  }
  
  // generate waveform
  for(j=0; j < 100; j++) {
    if((tmp_start - j) < 0) {
      current = 99 - (j - tmp_start - 1);    			
    } else {
      current = tmp_start - j;
      tmp = mAxisBuf.tmp_data_x_lcd[current];	
      
      if(j > 54) {
        CS = CS2;
        yadd_off = 0x4000;
      } else {
        CS = CS1;
        yadd_off = 0x4900;
      }
      
      if(tmp & 0x2000) {
        tmp = (~tmp+1) & 0x1FFF;
        sign = 1;
      } else {
        tmp = tmp & 0x1FFF;
        sign = 0;
      }

      disp = tmp / 128;
      
      if(sign == 0) {
        GLCD_Handler(RS_READ, CS, 0xB800 + ((3 - (disp >> 3))));
        GLCD_Handler(RS_READ, CS, yadd_off + ((j % 55)));
        GLCD_Handler(RS_WRITE, CS, (0xFF00 << (7 - (disp % 8))) & 0xFF00);
        
        for(i=0xBB00; i > (0xB800 + ((3-(disp / 8)))); i=i-0x0100) {
          GLCD_Handler(RS_READ, CS, i);
          GLCD_Handler(RS_READ, CS, yadd_off + ((j%55)));
          GLCD_Handler(RS_WRITE, CS, 0xFF00);
        }
        
        for(i=0xB800; i < (0xB800 + ((3-(disp >> 3)))); i=i+0x0100) {
          GLCD_Handler(RS_READ, CS, i);
          GLCD_Handler(RS_READ, CS, yadd_off + ((j%55)));
          GLCD_Handler(RS_WRITE, CS, 0x0000);
        }
        
        for(i=0xBC00; i < 0xC000; i=i+0x0100) {
          GLCD_Handler(RS_READ, CS, i);
          GLCD_Handler(RS_READ, CS, yadd_off + ((j%55)));
          GLCD_Handler(RS_WRITE, CS, 0x0000);
        }
        
      } else {
        
        GLCD_Handler(RS_READ, CS, 0xBC00 + ((disp >> 3)));
        GLCD_Handler(RS_READ, CS, yadd_off + ((j%55)));
        GLCD_Handler(RS_WRITE, CS, 0x00FF << (disp%8+1));
        
        for(i=0xBC00; i<(0xBC00 + ((disp >> 3))); i=i+0x0100) {
          GLCD_Handler(RS_READ, CS, i);
          GLCD_Handler(RS_READ, CS, yadd_off + ((j%55)));
          GLCD_Handler(RS_WRITE, CS, 0xFF00);
        }
        
        for(i=0xBF00; i>(0xBC00 + ((disp >> 3))); i=i-0x0100) {
          GLCD_Handler(RS_READ, CS, i);
          GLCD_Handler(RS_READ, CS, yadd_off + ((j%55)));
          GLCD_Handler(RS_WRITE, CS, 0x0000);
        }
        
        for(i=0xB800; i<0xBC00; i=i+0x0100) {
          GLCD_Handler(RS_READ, CS, i);
          GLCD_Handler(RS_READ, CS, yadd_off + ((j%55)));
          GLCD_Handler(RS_WRITE, CS, 0x0000);
        }
      }
    }
  }
}

void SelectAxisY(void) {
  char i, j;
  for(i = 1; i <= 8; i++) {
  GLCD_Handler(RS_READ, CS1, PAGE_SET | i);       // page set
  for(j = 1; j <= 9; j++) {
    GLCD_Handler(RS_READ, CS1, ADDR_SET | j);     // address set
    GLCD_Handler(RS_WRITE, CS1, plot_y[8*(j+1)-i-1]);  // write data
    }
  } 
}

void SelectAxisZ(void) {
  char i, j;
  for(i = 1; i <= 8; i++) {
  GLCD_Handler(RS_READ, CS1, PAGE_SET | i);       // page set
  for(j = 1; j <= 9; j++) {
    GLCD_Handler(RS_READ, CS1, ADDR_SET | j);     // address set
    GLCD_Handler(RS_WRITE, CS1, plot_z[8*(j+1)-i-1]);  // write data
    }
  } 
}

