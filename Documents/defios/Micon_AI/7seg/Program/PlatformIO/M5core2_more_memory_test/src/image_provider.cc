
/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include <cstdlib>
#include <cstring>
#include <iostream>
// #include <M5Stack.h>
#include <M5Core2.h>

// #include "zero_data.h"
// #include "nine_data.h"
// #include "five_data.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "esp_timer.h"

// #include "model_settings.h"
#include "image_provider.h"
#include "uart_frame.h"
#include "jpg_decoder.h"

static const char *TAG = "app_camera";

static uint16_t *display_buf; // buffer to hold data to be sent to display
int input_data_flag = -1;
typedef struct _JpegFrame_t
{
  uint8_t *buf;
  uint32_t size;
} JpegFrame_t;

static QueueHandle_t jpeg_fream_queue = NULL;

void frame_recv_callback(int cmd, const uint8_t *buf, int len)
{
  JpegFrame_t frame;
  frame.buf = (uint8_t *)malloc(sizeof(uint8_t) * len);
  memcpy(frame.buf, buf, len);
  frame.size = len;

  if (xQueueSend(jpeg_fream_queue, &frame, 0) != pdTRUE)
  {
    free(frame.buf);
  }
}
// カメラの初期設定
void camera_init()
{
  uart_frame_init(13, 14, 1500000);
  // uart_frame_init(36, 26, 1500000);
  jpeg_fream_queue = xQueueCreate(2, sizeof(JpegFrame_t));
}

// Get an image from the camera module
TfLiteStatus GetImage(int image_width, int image_height, int channels, float *image_data)
{
  // MicroPrintf("Image Captured!!");
  JpegFrame_t frame;
  jpg_file_decoder_t jpeg;
  int time_last = 0;
  int recieved_time;
  time_last = millis();

  // 1. 画像データ(jpeg)を受信
  xQueueReceive(jpeg_fream_queue, &frame, portMAX_DELAY);
  recieved_time = millis() - time_last;
  time_last = millis();

  // 2. デコードを実行
  jpeg = decodeJpg(frame.buf, frame.size);

  // 3. LCDにリサイズとクリップした画像を表示
  M5.Lcd.drawBitmap(222, 102, (uint32_t)jpeg.outWidth, (uint32_t)jpeg.outHeight, (uint16_t *)jpeg.lcdBuf);

  // 4. モデルにデータを入力
  for (int i = 0; i < jpeg.maxHeight * jpeg.maxWidth; i++)
  {
    // if (input_data_flag == 0)
    // {
    //   image_data[i] = (float)zero_data[i] / 255;
    // }
    // else if (input_data_flag == 5)
    // {
    //   image_data[i] = (float)five_data[i] / 255;
    // }
    // 精度を向上させるために閾値以下の場合は0(黒),それ以外はそのまま
    if (jpeg.inputBuf[i] > 220)
    {
      image_data[i] = (float)jpeg.inputBuf[i] / 255; // 0 ~ 1の間に正規化
    }
    else
    {
      image_data[i] = (float)0.00;
    }
  }

  // 5. メモリの開放
  if (jpeg.lcdBuf)
  {
    free(jpeg.lcdBuf);
    jpeg.lcdBuf = NULL;
  }
  if (jpeg.inputBuf)
  {
    free(jpeg.inputBuf);
    jpeg.inputBuf = NULL;
  }

  MicroPrintf("==received time:%d(ms)--decode time :%d(ms)--received image data size:%d(bytes)==", recieved_time, millis() - time_last, frame.size);
  free(frame.buf);
  return kTfLiteOk;
}
