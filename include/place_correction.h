#pragma once
#include <string>
#include <vector>

std::vector<std::string> normalize_numbers(const std::string &text);
void build_phonetic_index(const std::vector<std::string> &glossary);
std::string correct_place_names(const std::string &segment);
