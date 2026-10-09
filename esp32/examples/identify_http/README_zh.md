# identify_http：ESP32 上提取指纹并用 HTTP 识别

[English](README.md) | 中文

流程如下：

```
x.wav（嵌入固件）→ afp_extr 分块流式提取 → 指纹
→ HMAC-SHA1 签名 → HTTPS multipart POST /v1/identify → 打印结果
```

## 指纹

x.wav 在 "data" 之前有一个 LIST 块，如果固定跳过 44 字节的文件头读取，会把 LIST 块当成音频读进去。本示例按 RIFF 块结构解析，直接找到 data 块。

## 配置

`idf.py menuconfig`：
- **ACRCloud identify**：URL、access_key、access_secret、SNTP 服务器。签名里包含时间戳，所以开机后必须先对时。
- **Example Connection Configuration**：WiFi SSID / 密码。

凭据不要写进入库的文件。可以放在本地的 `sdkconfig.defaults.local`（已在 .gitignore 中）：

```
CONFIG_ACR_ACCESS_KEY="..."
CONFIG_ACR_ACCESS_SECRET="..."
CONFIG_EXAMPLE_WIFI_SSID="..."
CONFIG_EXAMPLE_WIFI_PASSWORD="..."
```

```bash
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.defaults.local" set-target esp32
idf.py build flash monitor
```

注意：凭据留空时，联网代码会被编译器当死代码删掉，运行时只会打印一条提示，然后退出。

## 在 QEMU 中运行（不需要板子）

```bash
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.defaults.qemu;sdkconfig.defaults.local" set-target esp32
idf.py build
cd build && esptool.py --chip esp32 merge_bin --fill-flash-size 4MB -o flash.bin @flash_args
qemu-system-xtensa -nographic -machine esp32 -m 4M -drive file=flash.bin,if=mtd,format=raw -nic user,model=open_eth
```

（`idf_tools.py install qemu-xtensa` 安装 QEMU；它依赖系统库 libslirp0。）

实测输出：

```
I identify: x.wav: 80000 samples (10.00 s)
I identify: session: 120120 bytes heap
I identify: fingerprint: 2184 bytes, extraction CPU 770 ms for 10.0 s audio
I identify: HTTP 200, 7682 ms, 1416 bytes response
I identify: min free heap 187788
```

QEMU 里的耗时不代表真机：模拟器不按真实时钟周期运行，TLS 握手中的 RSA 运算在模拟下尤其慢。

## 资源

- **固件大小**：WiFi 版约 1.1 MB（ESP32 / S3 都是），超过默认的 1 MB app 分区，所以 `sdkconfig.defaults` 改用了 `PARTITION_TABLE_SINGLE_APP_LARGE`（1.5 MB 分区，余量 25%）。其中包含嵌入的测试音频 x.wav（10 秒），换成麦克风后这部分就没有了。
- **内存顺序**：先提取指纹，`afp_extr_destroy` 释放 session 后再启动 WiFi 和 TLS。实际产品里通常只创建一个 session，WiFi 连着的同时用 `afp_extr_reset()` 反复复用；创建时需要保证剩余堆还有约 120 KB（ESP32 / S3 / P4）或 111 KB（其他芯片），不需要大的连续块。
- **任务栈**：TLS 握手在 app_main 中进行，所以 main 任务栈设成 10 KB。

## 文件

- `main/acr_identify.c`：签名（mbedtls HMAC-SHA1 + base64）和 multipart 请求（esp_http_client + 证书包）。服务器证书链的根是 USERTrust RSA，在 IDF 默认证书包里。
- `main/main.c`：wav 解析、流式提取、联网、SNTP 对时、打印结果。联网用的是 IDF 自带的 `protocol_examples_common`（`example_connect()`），只是为了让示例能直接跑起来；产品代码里换成自己的 WiFi 管理即可。
