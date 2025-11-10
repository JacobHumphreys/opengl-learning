#pragma once

#define ASSERT_BP(x) if (!x) __builtin_trap()
