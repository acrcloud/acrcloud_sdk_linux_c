cmake_minimum_required(VERSION 3.16)
# acrcloud_extr_sdk component from this repo; example_connect() (WiFi / Ethernet / QEMU open_eth) from ESP-IDF examples
set(EXTRA_COMPONENT_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/../../components
    $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(afp_identify_http)
