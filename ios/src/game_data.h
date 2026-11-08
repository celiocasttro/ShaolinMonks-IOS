#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ios_game_data {
struct CandidateInfo {
    std::string path;
    std::string type;
    uint64_t size = 0;
    std::string sha1;
    bool releaseMatches = false;
    bool releaseCheckAvailable = false;
};
std::string DocumentsDirectory();
std::string GameDirectory();
std::string AssetsDirectory();
std::string ExpectedXbeSha1();
bool PrepareFolders(std::string& error);
std::vector<std::string> FindCandidates();
std::vector<CandidateInfo> InspectCandidates();
}  // namespace ios_game_data
