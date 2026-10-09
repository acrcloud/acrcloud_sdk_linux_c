# demo: streaming extraction, timing and memory

English | [中文](README_zh.md)

Generates 10 seconds of synthetic 8 kHz audio, feeds it to the extractor in 256-sample blocks (a typical I2S DMA buffer size; any block size works) and prints:

- heap used by the session and the minimum free heap
- total extraction CPU time, the worst single block versus its real-time budget, and the real-time factor (CPU load on one core)
- the fingerprint size and the library version
- the task stack high-water mark

```bash
. $IDF_PATH/export.sh
idf.py set-target esp32      # or esp32s3, esp32c3, ...
idf.py build flash monitor
```

After each block the demo calls `vTaskDelay(1)`, as a task waiting for I2S data would. This keeps the IDLE task and the watchdogs running on single-core chips.

To use a real microphone, replace `synth_block()` with an I2S read and convert the data to 8 kHz mono int16.
