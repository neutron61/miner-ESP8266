#pragma once

#include <iostream>
#include <string>

extern "C"{
#include "json.h"
}

std::ostream &operator<<(std::ostream &out, json_value *value);

