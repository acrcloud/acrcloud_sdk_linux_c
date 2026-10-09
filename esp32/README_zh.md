# AFP Extractor SDK for ESP32

[English](README.md) | 中文

在 ESP32 系列芯片上从 8 kHz 单声道 PCM 中提取音频指纹，提取结果原样发送给 ACRCloud identify 接口即可识别。

- `components/acrcloud_extr_sdk/`：ESP-IDF 组件，包含公共头文件和各芯片的**预编译静态库**（不含源码）
- `examples/demo/`：流式提取、测速、内存统计
- `examples/identify_http/`：提取指纹后通过 HTTPS 调用 ACRCloud identify，返回识别结果

## 支持的芯片

| lib 目录 | 芯片 | 说明 |
|---|---|---|
| `esp32` | ESP32 | |
| `esp32_psram` | ESP32 + PSRAM | 开启 `CONFIG_SPIRAM_CACHE_WORKAROUND` 时**自动选用**（这个版本用 `-mfix-esp32-psram-cache-issue` 编译） |
| `esp32s3` | ESP32-S3 | |
| `esp32p4` | ESP32-P4 | |
| `esp32s2` `esp32c2` `esp32c3` `esp32c6` `esp32h2` | | 无硬件 FPU 的芯片，自动使用针对它们优化的实现 |

库会根据芯片自动选择最合适的实现，不需要任何配置。在 ESP32 / S3 / P4 上会使用 [esp-dsp](https://components.espressif.com/components/espressif/esp-dsp)，第一次构建时 ESP-IDF 组件管理器会自动下载并链接，工程里不需要额外添加任何东西；其他芯片不使用 esp-dsp。

要求 ESP-IDF v5.x（已在 v5.3.1 上验证）。

## 集成

把 `components/acrcloud_extr_sdk` 拷进你工程的 `components/` 目录，或者在工程的 CMakeLists.txt 里加上：

```cmake
set(EXTRA_COMPONENT_DIRS /path/to/afp_extr_sdk/components)
```

然后在你的组件里 `REQUIRES acrcloud_extr_sdk`。

## 使用

```c
#include "afp_extr.h"

afp_extr_config_t   cfg = AFP_EXTR_CONFIG_DEFAULT;   // 也可以直接传 NULL 使用默认值
afp_extr_session_t *s = afp_extr_create(&cfg);
while (recording) {
    afp_extr_feed(s, pcm, n);          // 8 kHz 单声道 int16，块大小任意（例如 I2S 每次读到的数据）
}
if (afp_extr_finish(s) == 0) {         // 少于 1 秒音频时返回 -1
    const void *fp;
    size_t      len;
    afp_extr_get_fingerprint(s, &fp, &len);   // fp 在 afp_extr_reset() / afp_extr_destroy() 之前一直有效
    /* 发送 fp / len，见 examples/identify_http */
}
afp_extr_destroy(s);
```

需要反复识别（例如每 10 秒一次）时，只创建一次 session，之后用 `afp_extr_reset()` 复用。不会重新申请内存，即使 WiFi/TLS 正在占用堆，这部分内存也始终可用：

```c
afp_extr_session_t *s = afp_extr_create(&cfg);   // 开机时创建一次
for (;;) {
    /* 喂 10 秒音频 */
    if (afp_extr_finish(s) == 0 && afp_extr_get_fingerprint(s, &fp, &len) == 0) {
        memcpy(buf, fp, len);                    // reset 之后 fp 失效，先拷贝
    }
    afp_extr_reset(s);                           // 配置不变，可以立刻喂下一段
    /* 上传 buf 的同时，下一段已经在提取了 */
}
```

- `afp_extr_create()` 接受一个 `afp_extr_config_t`（传 NULL 即使用默认值）。**务必**先用 `AFP_EXTR_CONFIG_DEFAULT` 初始化，再只改需要的字段：

  | 字段 | 默认值 | 含义 |
  |---|---|---|
  | `version_` | `AFP_EXTR_CONFIG_VERSION` | 结构体布局版本，由 `AFP_EXTR_CONFIG_DEFAULT` 设置，不要修改。 |
  | `is_local_` | 1 | 指纹模式。0 生成的指纹更小；identify 接口两种都接受。 |
  | `filter_energy_min_` | 10 | 指纹中保留特征的最低强度。 |
  | `silence_energy_threshold_` | 10 | 预留，保持默认值。 |
  | `silence_rate_threshold_` | 1.0 | 预留，保持默认值。 |
  | `pad0_`、`reserved_` | 0 | 留给以后新增的字段，必须保持为 0。 |

  结构体固定 64 字节。以后的版本只会从 `reserved_` 里划出新字段（0 表示默认值），按这个版本写的代码换用新库后仍然可以正常工作。`version_` 为 0 或高于库支持的版本、或者 `pad0_` / `reserved_` 不为 0 时，`afp_extr_create()` 返回 NULL。

- 指纹是一段不透明的字节串，原样作为 identify 请求（`data_type=fingerprint`）的 `sample` 发送即可，见 `examples/identify_http`。10 秒音频的指纹一般在 2 KB 左右。
- 出错时返回 NULL 或 -1，库本身不打印任何信息。
- 单核芯片（C2/C3/C6/H2/S2）上，不要在高优先级任务里不让出 CPU 地连续喂大量数据（比如从文件或内存一次性灌入），否则 IDLE 任务和看门狗会饿死。从 I2S 实时读取时，读取本身会阻塞，不存在这个问题。
- 多个 session 可以分别在不同任务里同时使用，但同一个 session 不能被两个任务同时调用。
- 可以用 `afp_extr_version()` 和头文件里的 `AFP_EXTR_VERSION` 比对，确认链接进来的库和头文件版本一致。
- 如果音频在文件里：`afp_extr_feed_wav_file()` 接受 8 kHz / 单声道 / 16-bit 的 wav，会正确跳过 LIST 等附加块，可以从 SPIFFS / FATFS / SD 卡读取。

## 资源占用

- **RAM**：每个 session 的堆内存在 ESP32 / S3 / P4 上约 120 KB，其他芯片约 111 KB，全部在内部 RAM 中（这正是速度快的原因）。拆成多次分配，单次最大 12 KB（ESP32 / S3 / P4）或 8 KB（其他芯片），所以在碎片化的堆上（例如 WiFi/BLE 运行时）也能正常创建。内存一直保留到 `afp_extr_destroy()`，`afp_extr_reset()` 会复用它。
- **任务栈**：库内部使用不到 2.5 KB。
- **Flash**：约 30–40 KB，视芯片而定。
- **CPU**：取决于芯片、主频和 flash 配置。请在你的板子上运行 `examples/demo`，它会打印处理 10 秒音频的 CPU 耗时和实时系数（单核占用比例）。

## 示例

```bash
. $IDF_PATH/export.sh
cd examples/demo           # 或 examples/identify_http（先看它的 README_zh.md 配置 WiFi 和凭据）
idf.py set-target esp32
idf.py build flash monitor
```
