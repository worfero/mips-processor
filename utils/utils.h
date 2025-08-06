#include <vector>
#include <stdint.h>
#include <string>
#include <iomanip>
#include <fstream>
#include <iostream>

#ifndef UTILS_H
#define UTILS_H

unsigned get_bits(unsigned num, unsigned lsbit, unsigned msbit);
std::vector<uint32_t> readFile();

#endif // UTILS_H