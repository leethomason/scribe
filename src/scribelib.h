#pragma once

#include "func.h"
#include "environment.h"

// Stateless class! Held in a global var.
// Also, stdlib shouldn't have state.

void AttachStdLib(FFI&, Environment& env);
