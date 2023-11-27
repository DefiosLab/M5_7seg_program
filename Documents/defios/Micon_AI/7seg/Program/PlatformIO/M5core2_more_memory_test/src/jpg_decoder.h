#include <stdio.h>
#include "rom/tjpgd.h"
#include <M5Display.h>
typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t maxWidth;
    uint16_t maxHeight;
    uint16_t offX;
    uint16_t offY;
    jpeg_div_t scale;
    const void *src;
    uint16_t *lcdBuf;  // LCDに表示するデータ形式(RGB565 grayscale)
    uint8_t *inputBuf; // モデルに入力するデータ形式(grayscale 8bit)
    uint32_t buf_size;
    uint wfbuf;
    size_t len;
    size_t index;
    uint16_t outWidth;
    uint16_t outHeight;
} jpg_file_decoder_t;

jpg_file_decoder_t decodeJpg(const uint8_t *jpg_data, size_t jpg_len);

