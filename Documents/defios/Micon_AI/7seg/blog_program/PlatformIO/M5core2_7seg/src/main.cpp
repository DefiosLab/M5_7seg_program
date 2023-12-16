
#include "detection_responder.h"
#include "image_provider.h"
#include "model_settings.h"
#include "model_data.h"

#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include <esp_heap_caps.h>
// #include <M5Stack.h>
#include <M5Core2.h>
// #include "zero_data.h"
// #include "nine_data.h"
// #include "five_data.h"
#include "uart_frame.h"

// Globals, used for compatibility with Arduino-style sketches.
namespace {
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;

//メモリの割り当て
// An area of memory to use for input, output, and intermediate arrays.
// constexpr int kTensorArenaSize = 20 * 1024;
constexpr int kTensorArenaSize = 80 * 1024;
static uint8_t tensor_arena[kTensorArenaSize]; // Maybe we should move this to external
}  // namespace



void displayInit(){
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(100,10);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("7seg DEMO 2");
  M5.Lcd.setCursor(50,50);
  M5.Lcd.println("INPUT DATA : camera");
  M5.Lcd.setCursor(100,150);
  M5.Lcd.println("RESULT");
  // M5.Lcd.setCursor(40,220);
  // M5.Lcd.setTextSize(2);
  // M5.Lcd.println("zero");
  // M5.Lcd.setCursor(130,220);
  // M5.Lcd.setTextSize(2);
  // M5.Lcd.println("five");
  M5.Lcd.setCursor(230,220);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("camera");
  // M5.Lcd.fillRect(180,80,130,80,WHITE);
  M5.Lcd.fillRect(220,100,32,32,BLUE);
}

void showInputDataType(const char* str){
  M5.Lcd.setTextFont(0);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(50,50);
    M5.Lcd.printf("INPUT DATA : %s\n",str);
}

void btnController(){
  M5.update();
  // if (M5.BtnA.wasReleased() ){
  //   input_data_flag = 0;
  //   showInputDataType("zero  ");
  // }else if(M5.BtnB.wasReleased()){
  //   input_data_flag = 5;
  //   showInputDataType("five  ");
  // }else if (M5.BtnC.wasReleased()){
  //   input_data_flag = -1;
  //   showInputDataType("camera");
  // }

  if (M5.BtnC.wasPressed()){
    input_data_flag = -1;
    showInputDataType("camera");
  }
}


void setup() {

  // 1. モデルの読み込み
  model = tflite::GetModel(lstm_model_data);

  // 2. 演算子リゾルバの設定
  static tflite::MicroMutableOpResolver<128> micro_op_resolver;
  micro_op_resolver.AddAbs();
  micro_op_resolver.AddAdd();
  micro_op_resolver.AddAddN();
  micro_op_resolver.AddArgMax();
  micro_op_resolver.AddArgMin();
  micro_op_resolver.AddAssignVariable();
  micro_op_resolver.AddAveragePool2D();
  micro_op_resolver.AddBatchToSpaceNd();
  micro_op_resolver.AddBroadcastArgs();
  micro_op_resolver.AddBroadcastTo();
  micro_op_resolver.AddCallOnce();
  micro_op_resolver.AddCast();
  micro_op_resolver.AddCeil();
  micro_op_resolver.AddCircularBuffer();
  micro_op_resolver.AddConcatenation();
  micro_op_resolver.AddConv2D();
  micro_op_resolver.AddCos();
  micro_op_resolver.AddCumSum();
  micro_op_resolver.AddDepthToSpace();
  micro_op_resolver.AddDepthwiseConv2D();
  micro_op_resolver.AddDequantize();
  micro_op_resolver.AddDetectionPostprocess();
  micro_op_resolver.AddDiv();
  micro_op_resolver.AddElu();
  micro_op_resolver.AddEqual();
  micro_op_resolver.AddEthosU();
  micro_op_resolver.AddExp();
  micro_op_resolver.AddExpandDims();
  micro_op_resolver.AddFill();
  micro_op_resolver.AddFloor();
  micro_op_resolver.AddFloorDiv();
  micro_op_resolver.AddFloorMod();
  micro_op_resolver.AddFullyConnected();
  micro_op_resolver.AddGather();
  micro_op_resolver.AddGatherNd();
  micro_op_resolver.AddGreater();
  micro_op_resolver.AddGreaterEqual();
  micro_op_resolver.AddHardSwish();
  micro_op_resolver.AddIf();
  micro_op_resolver.AddL2Normalization();
  micro_op_resolver.AddL2Pool2D();
  micro_op_resolver.AddLeakyRelu();
  micro_op_resolver.AddLess();
  micro_op_resolver.AddLessEqual();
  micro_op_resolver.AddLog();
  micro_op_resolver.AddLogicalAnd();
  micro_op_resolver.AddLogicalNot();
  micro_op_resolver.AddLogicalOr();
  micro_op_resolver.AddLogistic();
  micro_op_resolver.AddLogSoftmax();
  micro_op_resolver.AddMaxPool2D();
  micro_op_resolver.AddMaximum();
  micro_op_resolver.AddMean();
  micro_op_resolver.AddMinimum();
  micro_op_resolver.AddMirrorPad();
  micro_op_resolver.AddMul();
  micro_op_resolver.AddNeg();
  micro_op_resolver.AddNotEqual();
  micro_op_resolver.AddPack();
  micro_op_resolver.AddPad();
  micro_op_resolver.AddPadV2();
  micro_op_resolver.AddPrelu();
  micro_op_resolver.AddQuantize();
  micro_op_resolver.AddReadVariable();
  micro_op_resolver.AddReduceMax();
  micro_op_resolver.AddRelu();
  micro_op_resolver.AddRelu6();
  micro_op_resolver.AddReshape();
  micro_op_resolver.AddResizeBilinear();
  micro_op_resolver.AddResizeNearestNeighbor();
  micro_op_resolver.AddRound();
  micro_op_resolver.AddRsqrt();
  micro_op_resolver.AddSelectV2();
  micro_op_resolver.AddShape();
  micro_op_resolver.AddSin();
  micro_op_resolver.AddSlice();
  micro_op_resolver.AddSoftmax();
  micro_op_resolver.AddSpaceToBatchNd();
  micro_op_resolver.AddSpaceToDepth();
  micro_op_resolver.AddSplit();
  micro_op_resolver.AddSplitV();
  micro_op_resolver.AddSqrt();
  micro_op_resolver.AddSquare();
  micro_op_resolver.AddSquaredDifference();
  micro_op_resolver.AddSqueeze();
  micro_op_resolver.AddStridedSlice();
  micro_op_resolver.AddSub();
  micro_op_resolver.AddSum();
  micro_op_resolver.AddSvdf();
  micro_op_resolver.AddTanh();
  micro_op_resolver.AddTranspose();
  micro_op_resolver.AddTransposeConv();
  micro_op_resolver.AddUnidirectionalSequenceLSTM();
  micro_op_resolver.AddUnpack();
  micro_op_resolver.AddVarHandle();
  micro_op_resolver.AddWhile();
  micro_op_resolver.AddZerosLike();

  // static tflite::MicroMutableOpResolver<4> micro_op_resolver;
  // micro_op_resolver.AddUnidirectionalSequenceLSTM();
  // micro_op_resolver.AddReshape();
  // micro_op_resolver.AddSoftmax();
  // micro_op_resolver.AddFullyConnected();

  // 3. インタプリタの作成
  static tflite::MicroInterpreter static_interpreter(
      model, micro_op_resolver, tensor_arena, kTensorArenaSize);
  interpreter = &static_interpreter;

  // 4. テンソルにメモリの割当
  TfLiteStatus allocate_status = interpreter->AllocateTensors();

  input = interpreter->input(0);


  // 5. カメラの初期設定
  M5.begin();
  Serial.begin(1500000);
  camera_init();

    if(showDisplay){
    displayInit();
  }
}

void loop() {
  if(showDisplay){
    btnController();
  }

    // Get image from provider.
  if (kTfLiteOk != GetImage(kNumCols, kNumRows, kNumChannels, input->data.f)) {
    MicroPrintf("Image capture failed.");
  }

  // Run the model on this input and make sure it succeeds.
  if (kTfLiteOk != interpreter->Invoke()) {
    MicroPrintf("Invoke failed.");
  }

  TfLiteTensor* output = interpreter->output(0);

  // Respond to detection
  RespondToDetection(output->data.f);
  
  // メモリ消費率の確認
  // MicroPrintf("heap_caps_get_free_size(MALLOC_CAP_DMA):%d", heap_caps_get_free_size(MALLOC_CAP_DMA) );
  // MicroPrintf("heap_caps_get_largest_free_block(MALLOC_CAP_DMA):%d", heap_caps_get_largest_free_block(MALLOC_CAP_DMA) );
}