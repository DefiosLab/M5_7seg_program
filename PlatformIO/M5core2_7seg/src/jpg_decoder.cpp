
#include "jpg_decoder.h"
// #include <M5Stack.h>
#include <M5Core2.h>
#include "model_settings.h"

#define jpgColor(c)                                  \
    (((uint16_t)(((uint8_t *)(c))[0] & 0xF8) << 8) | \
     ((uint16_t)(((uint8_t *)(c))[1] & 0xFC) << 3) | \
     ((((uint8_t *)(c))[2] & 0xF8) >> 3))

uint16_t rgb565Grayscale(uint16_t input){
    uint16_t red = input  & 0xF800;
    uint16_t green = input & 0x07E0;
    uint16_t blue = input & 0x001F;
    return (0.2126 * (red >> 11)) + (0.7152 * (green >> 5) / 2.0) + (0.0722 * blue);

}

// jpegデータの読み取りを行う関数
static uint32_t jpgRead(JDEC *decoder, uint8_t *buf, uint32_t len)
{
    jpg_file_decoder_t *jpeg = (jpg_file_decoder_t *)decoder->device;
    if (buf)
    {
        memcpy(buf, (const uint8_t *)jpeg->src + jpeg->index, len);
    }
    jpeg->index += len;
    return len;
}

static uint32_t jpgWrite(JDEC *decoder, void *bitmap, JRECT *rect)
{
    jpg_file_decoder_t *jpeg = (jpg_file_decoder_t *)decoder->device;
    uint16_t x = rect->left;
    uint16_t y = rect->top;
    uint16_t w = rect->right + 1 - x;
    uint16_t h = rect->bottom + 1 - y;
    uint16_t oL = 0, oR = 0;
    uint8_t *data = (uint8_t *)bitmap;

    if (rect->right < jpeg->offX)
    {
        return 1;
    }
    if (rect->left >= (jpeg->offX + jpeg->outWidth))
    {
        return 1;
    }
    if (rect->bottom < jpeg->offY)
    {
        return 1;
    }
    if (rect->top >= (jpeg->offY + jpeg->outHeight))
    {
        return 1;
    }
    if (rect->top < jpeg->offY)
    {
        uint16_t linesToSkip = jpeg->offY - rect->top;
        data += linesToSkip * w * 3;
        h -= linesToSkip;
        y += linesToSkip;
    }
    if (rect->bottom >= (jpeg->offY + jpeg->outHeight))
    {
        uint16_t linesToSkip =
            (rect->bottom + 1) - (jpeg->offY + jpeg->outHeight);
        h -= linesToSkip;
    }
    if (rect->left < jpeg->offX)
    {
        oL = jpeg->offX - rect->left;
    }
    if (rect->right >= (jpeg->offX + jpeg->outWidth))
    {
        oR = (rect->right + 1) - (jpeg->offX + jpeg->outWidth);
    }

    uint32_t pixIndex = 0;
    uint16_t line;
    uint16_t tmp;
    uint16_t *lcdBufDst = &(jpeg->lcdBuf[(y * jpeg->wfbuf + x)]);
    uint8_t *inputBufDst = &(jpeg->inputBuf[(y * jpeg->wfbuf + x)]);
    line = w - (oL + oR);
    tmp = line;
    // Serial.printf("addr : %p,lcdBufDst : %p, x : %d, y : %d, h:%d, line:%d oR : %d, oL : %d\n",lcdBufDst,&lcdBufDst[pixIndex],x,y,h,line,oR,oL);
    while (h--)
    {
        data += 3 * oL;
        line = w - (oL + oR);
        pixIndex = 0;
        while (line--)
        {
            // RGB565
            lcdBufDst[pixIndex] = jpgColor(data);

            // RGB565 grayscale
            uint16_t grayscale = rgb565Grayscale(lcdBufDst[pixIndex]);
            lcdBufDst[pixIndex] = (grayscale << 11) + (grayscale << 6) + grayscale;

            // grayscale 8bit
            inputBufDst[pixIndex] = uint8_t(0.2126 * data[0] + 0.7152 * data[1] + 0.0722 * data[2]);
            // inputBufDst[pixIndex] = inputBufDst[pixIndex] / 255;
            pixIndex++;
            data += 3;
        }
        // Serial.printf("addr : %p, lcdBufDst: %p\n",&lcdBufDst[pixIndex],lcdBufDst);
        lcdBufDst = &lcdBufDst[pixIndex] + jpeg->wfbuf - tmp;
        inputBufDst = &inputBufDst[pixIndex] + jpeg->wfbuf - tmp;
        data += 3 * oR;
    }
    return 1;
}

// デコードを行う関数
static bool jpgDecode(jpg_file_decoder_t *jpeg,
                      uint32_t (*reader)(JDEC *, uint8_t *, uint32_t))
{

    static uint8_t work[3100];
    JDEC decoder;

    // 1. 前準備(データの読み取り)
    JRESULT jres = jd_prepare(&decoder, reader, work, 3100, jpeg);
    if (jres != JDR_OK)
    {
        Serial.printf("jd_prepare failed! %d\n", jres);
        return false;
    }

    // 2. 切り抜きやリサイズ等の取得する画像サイズの設定とチェック
    uint16_t jpgWidth = decoder.width / (1 << (uint8_t)(jpeg->scale));
    uint16_t jpgHeight = decoder.height / (1 << (uint8_t)(jpeg->scale));

    if (jpeg->offX >= jpgWidth || jpeg->offY >= jpgHeight)
    {
        Serial.println("Offset Outside of JPEG size");
        return false;
    }

    size_t jpgMaxWidth = jpgWidth;
    size_t jpgMaxHeight = jpgHeight;

    jpeg->outWidth =
        (jpgMaxWidth > jpeg->maxWidth) ? jpeg->maxWidth : jpgMaxWidth;
    jpeg->outHeight =
        (jpgMaxHeight > jpeg->maxHeight) ? jpeg->maxHeight : jpgMaxHeight;


    // 3. メモリの確保
    jpeg->lcdBuf = (uint16_t *)malloc(2 * jpeg->outWidth * jpeg->outHeight);
    jpeg->inputBuf = (uint8_t *)malloc(2 * jpeg->outWidth * jpeg->outHeight);
    if (jpeg->lcdBuf == NULL)
    {
        Serial.println("malloc failed");
        return false;
    }
    // Serial.printf("width: %d, height : %d \n",jpeg->outWidth,jpeg->outHeight);
    jpeg->wfbuf = jpeg->outWidth;

    // 4. デコード結果をバッファに格納する
    jres = jd_decomp(&decoder, jpgWrite, (uint8_t)jpeg->scale);
    if (jres != JDR_OK)
    {
        Serial.printf("jd_decomp failed! %d\n", jres);
        return false;
    }
    return true;
}

jpg_file_decoder_t decodeJpg(const uint8_t *jpg_data, size_t jpg_len)
{
    // 1. デコードに必要な構造体
    jpg_file_decoder_t jpeg;

    jpeg.src = jpg_data;
    jpeg.len = jpg_len;
    jpeg.index = 0;
    jpeg.x = 0;
    jpeg.y = 0;
    jpeg.maxWidth = kNumCols;
    jpeg.maxHeight = kNumRows;
    jpeg.offX = 0;
    jpeg.offY = 0;
    jpeg.scale = JPEG_DIV_8; // 1/8に縮小

    // 2. jpgデータをデコード
    jpgDecode(&jpeg, jpgRead);
    return jpeg;
}
