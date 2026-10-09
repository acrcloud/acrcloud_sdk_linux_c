idf_component_register(SRCS "main.c" "acr_identify.c"
                       INCLUDE_DIRS "."
                       REQUIRES acrcloud_extr_sdk esp_http_client esp-tls mbedtls esp_netif esp_timer nvs_flash
                                protocol_examples_common
                       EMBED_FILES "x.wav")
