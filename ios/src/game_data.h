#pragma once

#include <string>
#include <vector>

namespace ios_game_data {
std::string DocumentsDirectory();
std::string GameDirectory();
std::string AssetsDirectory();
bool PrepareFolders(std::string& error);
std::vector<std::string> FindCandidates();
}  // namespace ios_game_data
