#pragma once

// volk 必须先于 VMA，避免 vulkan.h 原型与 volk 符号冲突
// clang-format off
#include <volk.h>
#include <vk_mem_alloc.h>
// clang-format on
