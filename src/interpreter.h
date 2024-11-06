#pragma once

#include "machine.h"

class Interpreter {
    ConstPool constPool;
    Machine machine;

public:
    Value interpret(std::string input);
};

