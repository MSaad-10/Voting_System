#pragma once
#ifndef VALIDATION_H
#define VALIDATION_H

#include <string>

class Validation {
public:
    static int getValidatedInt(const std::string& prompt, int min = INT_MIN, int max = INT_MAX);
    static std::string getValidatedString(const std::string& prompt, int minLength = 1, int maxLength = 100);
    static std::string getNonEmptyString(const std::string& prompt);
};

#endif
