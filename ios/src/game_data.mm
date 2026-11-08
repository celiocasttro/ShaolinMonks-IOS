#import <Foundation/Foundation.h>
#import <CommonCrypto/CommonDigest.h>
#include "game_data.h"
#include "kernel/disc.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <system_error>

#ifndef IOS_EXPECTED_XBE_SHA1
#define IOS_EXPECTED_XBE_SHA1 ""
#endif

namespace ios_game_data {
namespace fs = std::filesystem;
namespace {
std::string ToUtf8(NSString* value) {
    if (!value) return {};
    const char* text = [value UTF8String];
    return text ? std::string(text) : std::string{};
}
std::string Sha1Bytes(const std::vector<uint8_t>& bytes) {
    CC_SHA1_CTX context;
    CC_SHA1_Init(&context);
    size_t offset = 0;
    while (offset < bytes.size()) {
        const size_t count = std::min<size_t>(bytes.size() - offset, 1024 * 1024);
        CC_SHA1_Update(&context, bytes.data() + offset, static_cast<CC_LONG>(count));
        offset += count;
    }
    unsigned char digest[CC_SHA1_DIGEST_LENGTH];
    CC_SHA1_Final(digest, &context);
    std::ostringstream result;
    result << std::hex << std::setfill('0');
    for (unsigned char byte : digest) result << std::setw(2) << static_cast<unsigned int>(byte);
    return result.str();
}
std::string Sha1File(const fs::path& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) return {};
    CC_SHA1_CTX context;
    CC_SHA1_Init(&context);
    std::array<char, 1024 * 1024> buffer{};
    while (input) {
        input.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
        const auto count = input.gcount();
        if (count > 0) CC_SHA1_Update(&context, buffer.data(), static_cast<CC_LONG>(count));
    }
    if (!input.eof()) return {};
    unsigned char digest[CC_SHA1_DIGEST_LENGTH];
    CC_SHA1_Final(digest, &context);
    std::ostringstream result;
    result << std::hex << std::setfill('0');
    for (unsigned char byte : digest) result << std::setw(2) << static_cast<unsigned int>(byte);
    return result.str();
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
std::string ExpectedXbeSha1() { return IOS_EXPECTED_XBE_SHA1; }
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
    for (const auto& item : InspectCandidates()) found.push_back(item.path);
    return found;
}
std::vector<CandidateInfo> InspectCandidates() {
    std::vector<CandidateInfo> found;
    const auto root = GameDirectory();
    if (root.empty()) return found;
    const std::string expected = ExpectedXbeSha1();
    const fs::path locations[] = {fs::path(root), fs::path(root) / "assets"};
    for (const auto& location : locations) {
        std::error_code ec;
        if (!fs::is_directory(location, ec)) continue;
        for (fs::directory_iterator it(location, ec), end; !ec && it != end; it.increment(ec)) {
            if (!it->is_regular_file(ec)) continue;
            std::string extension = it->path().extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(),
                [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            CandidateInfo item;
            if (extension == ".xbe" && it->path().filename() == "default.xbe") item.type = "XBE";
            else if (extension == ".iso") item.type = "ISO";
            else continue;
            item.path = it->path().string();
            item.size = it->file_size(ec);
            if (ec) { ec.clear(); item.size = 0; }
            if (item.type == "XBE") {
                item.sha1 = Sha1File(it->path());
            } else if (item.type == "ISO") {
                kernel::Disc disc;
                std::string error;
                std::vector<uint8_t> embeddedXbe;
                if (disc.Open(it->path(), error) && disc.Load("default.xbe", embeddedXbe)) {
                    item.containsXbe = true;
                    item.sha1 = Sha1Bytes(embeddedXbe);
                }
            }
            item.releaseCheckAvailable = !expected.empty() && !item.sha1.empty();
            item.releaseMatches = item.releaseCheckAvailable && item.sha1 == expected;
            found.push_back(std::move(item));
        }
    }
    std::sort(found.begin(), found.end(), [](const CandidateInfo& a, const CandidateInfo& b) {
        return a.path < b.path;
    });
    found.erase(std::unique(found.begin(), found.end(), [](const CandidateInfo& a, const CandidateInfo& b) {
        return a.path == b.path;
    }), found.end());
    return found;
}
}  // namespace ios_game_data
