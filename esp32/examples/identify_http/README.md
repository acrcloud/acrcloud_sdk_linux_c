# identify_http: fingerprint extraction and HTTP recognition on ESP32

English | [中文](README_zh.md)

Flow:

```
x.wav (embedded in firmware) → afp_extr streaming extraction in blocks → fingerprint
→ HMAC-SHA1 signature → HTTPS multipart POST /v1/identify → print result
```

## Fingerprint

x.wav has a LIST chunk before its "data" chunk, so a reader that skips a fixed 44-byte header would treat the LIST chunk as audio. This example parses the RIFF chunks and goes straight to the data chunk.

## Configuration

`idf.py menuconfig`:
- **ACRCloud identify**: URL, access_key, access_secret, SNTP server. The signature contains a timestamp, so the clock must be synchronized after boot.
- **Example Connection Configuration**: WiFi SSID / password.

Do not put credentials in files under version control. Use a local `sdkconfig.defaults.local` instead (already listed in .gitignore):

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

Note: if the credentials are left empty, the compiler removes the networking code as dead code, and at run time the example only prints a hint and exits.

## Running in QEMU (no board needed)

```bash
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.defaults.qemu;sdkconfig.defaults.local" set-target esp32
idf.py build
cd build && esptool.py --chip esp32 merge_bin --fill-flash-size 4MB -o flash.bin @flash_args
qemu-system-xtensa -nographic -machine esp32 -m 4M -drive file=flash.bin,if=mtd,format=raw -nic user,model=open_eth
```

(Install QEMU with `idf_tools.py install qemu-xtensa`. It needs the system library libslirp0.)

Measured output:

```
I identify: x.wav: 80000 samples (10.00 s)
I identify: session: 120120 bytes heap
I identify: fingerprint: 2184 bytes, extraction CPU 770 ms for 10.0 s audio
I identify: HTTP 200, 7682 ms, 1416 bytes response
I identify: min free heap 187788
```

Timings in QEMU do not represent real hardware: the emulator does not run at real clock cycles, and the RSA operations of the TLS handshake are particularly slow under emulation.

## Resources

- **Firmware size**: the WiFi build is about 1.1 MB (both ESP32 and S3), more than the default 1 MB app partition. `sdkconfig.defaults` therefore selects `PARTITION_TABLE_SINGLE_APP_LARGE` (1.5 MB partition, 25% free). This includes the embedded 10-second test audio x.wav, which goes away once you use a microphone instead.
- **Memory order**: the fingerprint is extracted first, and `afp_extr_destroy` frees the session before WiFi and TLS start. A real product usually keeps one session and reuses it with `afp_extr_reset()` while WiFi is connected; then make sure about 120 KB (ESP32 / S3 / P4) or 111 KB (other chips) of heap is free when creating it (no large contiguous block is needed).
- **Task stack**: the TLS handshake runs in app_main, so the main task stack is set to 10 KB.

## Files

- `main/acr_identify.c`: signature (mbedtls HMAC-SHA1 + base64) and the multipart request (esp_http_client + certificate bundle). The server's certificate chain ends at the USERTrust RSA root, which is in the default IDF certificate bundle.
- `main/main.c`: wav parsing, streaming extraction, networking, SNTP time sync, result printing. Networking uses `example_connect()` from IDF's `protocol_examples_common` only so that the example runs out of the box; replace it with your own WiFi management in product code.
