# demo：流式提取、测速、内存统计

[English](README.md) | 中文

生成 10 秒 8 kHz 合成音频，按每块 256 个样本送入提取器（这是 I2S DMA 常见的缓冲大小，块大小任意都可以），并打印：

- session 占用的堆内存，以及最小剩余堆
- 提取的 CPU 总耗时、单块最长耗时与该块的实时预算对比、实时系数（单核占用）
- 指纹大小和库版本
- 任务栈最高水位

```bash
. $IDF_PATH/export.sh
idf.py set-target esp32      # 或 esp32s3、esp32c3 等
idf.py build flash monitor
```

demo 每送完一块调用一次 `vTaskDelay(1)`，和等待 I2S 数据的任务行为一致，保证单核芯片上的 IDLE 任务和看门狗正常运行。

换成真实麦克风时，把 `synth_block()` 换成 I2S 读取，并转换成 8 kHz 单声道 int16。
