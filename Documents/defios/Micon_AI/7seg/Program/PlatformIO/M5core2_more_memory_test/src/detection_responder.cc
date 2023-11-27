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

#include "detection_responder.h"
#include "tensorflow/lite/micro/micro_log.h"

#include <stdio.h>

#include "tensorflow/lite/micro/debug_log.h"
// #include <M5Stack.h>
#include <M5Core2.h>
#include "model_settings.h"

void RespondToDetection(float *result)
{
  char s[128];
  if (showDisplay)
  {
    M5.Lcd.setCursor(10, 170);
    M5.Lcd.setTextFont(2);
    M5.Lcd.setTextSize(1);
  }
  // for (int i = 0; i < 10; i++)
  for (int i = 0; i < 11; i++)
  {
    sprintf(s, "%d :%3.5f  ", i, result[i]);
    DebugLog(s);
    if (showDisplay)
    {
      M5.Lcd.printf("%d: %1.3f ", i, result[i]);
      if (i == 4)
      {
        M5.Lcd.setCursor(10, 190);
      }
      if(i == 9)
      {
        M5.Lcd.setCursor(10, 210);
      }
    }
  }
  DebugLog("\r\n");
}
