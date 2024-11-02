#pragma once

#include "machine.h"

class Interpreter {
    ConstPool constPool;
    Machine machine;

public:
    void interpret(std::string input);
};