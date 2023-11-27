# ７seg認識モデルをM5stackで動かす


## ファイル

- 7seg_demo.mov : 7seg認識モデルをM5stackで動かしたデモ動画
- quantize.ipynb : kerasで学習させたモデル(h5)をtfliteに変換するコード (google colabで実行することを想定)
- 7seg.h5 : keradで学習させたモデル 
- 7seg.tflite : ダイナミックレンジで量子化してモデル
- 7seg_optimize.tflite : ダイナミックレンジで量子化しDEFAULTで最適化したモデル
- 7seg.png : 7seg.tfliteをnetronで開いた画像
- 7seg_optimize.png : 7segoptimize.tfliteをnetronで開いた画像
- test_images : 学習画像。完全整数化の際のキャリブレーション画像として使うことを想定
