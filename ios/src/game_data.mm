#import <Foundation/Foundation.h>
#include "game_data.h"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <system_error>

namespace ios_game_data {
namespace fs = std::filesystem;
namespace {
std::string ToUtf8(NSString* value) {
    if (!value) return {};
    const char* text = [value UTF8String];
    return text ? std::string(text) : std::string{};
}
bool IsRegularFile(const fs::path& path) {
    std::error_code ec;
    return fs::is_regular_file(path, ec);
}
}
std::string DocumentsDirectory() {
    @autoreleasepool {
        NSArray<NSURL*>* urls = [[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask];
        return urls.count ? ToUtf8(urls.firstObject.path) : std::string{};
    }
}
std::string GameDirectory() {
    const auto documents = DocumentsDirectory();
    return documents.empty() ? std::string{} : (fs::path(documents) / "ShaolinMonks").string();
}
std::string AssetsDirectory() {
    const auto folder = GameDirectory();
    return folder.empty() ? std::string{} : (fs::path(folder) / "assets").string();
}
bool PrepareFolders(std::string& error) {
    const auto folder = GameDirectory();
    if (folder.empty()) { error = "iOS did not provide an app Documents directory"; return false; }
    std::error_code ec;
    fs::create_directories(fs::path(folder) / "assets", ec);
    if (ec) { error = "Cannot create game-data folder: " + ec.message(); return false; }
    error.clear();
    return true;
}
std::vector<std::string> FindCandidates() {
    std::vector<std::string> found;
    const auto root = GameDirectory();
    if (root.empty()) return found;
    const fs::path locations[] = {fs::path(root), fs::path(root) / "assets"};
    for (const auto& location : locations) {
        const auto xbe = location / "default.xbe";
        if (IsRegularFile(xbe)) found.push_back(xbe.string());
        std::error_code ec;
        if (!fs::is_directory(location, ec)) continue;
        for (fs::directory_iterator it(location, ec), end; !ec && it != end; it.increment(ec)) {
            if (!it->is_regular_file(ec)) continue;
            auto extension = it->path().extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(),
                [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            if (extension == ".iso") found.push_back(it->path().string());
        }
    }
    std::sort(found.begin(), found.end());
    found.erase(std::unique(found.begin(), found.end()), found.end());
    return found;
}
}  // namespace ios_game_data
