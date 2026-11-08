#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>
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
namespace fs=std::filesystem;
namespace {
std::string ToUtf8(NSString* s){const char* p=s?[s UTF8String]:nullptr;return p?std::string(p):std::string{};}
std::string Sha1Bytes(const std::vector<uint8_t>& b){CC_SHA1_CTX c;CC_SHA1_Init(&c);size_t o=0;while(o<b.size()){size_t n=std::min<size_t>(b.size()-o,1024*1024);CC_SHA1_Update(&c,b.data()+o,(CC_LONG)n);o+=n;}unsigned char d[CC_SHA1_DIGEST_LENGTH];CC_SHA1_Final(d,&c);std::ostringstream s;s<<std::hex<<std::setfill('0');for(auto x:d)s<<std::setw(2)<<(unsigned)x;return s.str();}
std::string Sha1File(const fs::path& p){std::ifstream f(p,std::ios::binary);if(!f)return {};CC_SHA1_CTX c;CC_SHA1_Init(&c);std::array<char,1024*1024>b{};while(f){f.read(b.data(),b.size());auto n=f.gcount();if(n>0)CC_SHA1_Update(&c,b.data(),(CC_LONG)n);}if(!f.eof())return {};unsigned char d[CC_SHA1_DIGEST_LENGTH];CC_SHA1_Final(d,&c);std::ostringstream s;s<<std::hex<<std::setfill('0');for(auto x:d)s<<std::setw(2)<<(unsigned)x;return s.str();}
}
std::string DocumentsDirectory(){@autoreleasepool{NSArray<NSURL*>*u=[[NSFileManager defaultManager]URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask];return u.count?ToUtf8(u.firstObject.path):std::string{};}}
std::string GameDirectory(){auto d=DocumentsDirectory();return d.empty()?std::string{}:(fs::path(d)/"ShaolinMonks").string();}
std::string AssetsDirectory(){auto d=GameDirectory();return d.empty()?std::string{}:(fs::path(d)/"assets").string();}
std::string ExpectedXbeSha1(){return IOS_EXPECTED_XBE_SHA1;}
bool PrepareFolders(std::string& e){auto d=GameDirectory();if(d.empty()){e="Documents unavailable";return false;}std::error_code ec;fs::create_directories(fs::path(d)/"assets",ec);fs::create_directories(fs::path(d)/"cheats",ec);if(ec){e=ec.message();return false;}e.clear();return true;}
std::vector<std::string> FindCandidates(){std::vector<std::string>v;for(auto&x:InspectCandidates())v.push_back(x.path);return v;}
std::vector<CandidateInfo> InspectCandidates(){std::vector<CandidateInfo>v;auto root=GameDirectory();if(root.empty())return v;std::string expected=ExpectedXbeSha1();for(auto loc:{fs::path(root),fs::path(root)/"assets"}){std::error_code ec;if(!fs::is_directory(loc,ec))continue;for(fs::directory_iterator i(loc,ec),end;!ec&&i!=end;i.increment(ec)){if(!i->is_regular_file(ec))continue;std::string ext=i->path().extension().string();std::transform(ext.begin(),ext.end(),ext.begin(),[](unsigned char c){return(char)std::tolower(c);});CandidateInfo x;if(ext==".xbe"&&i->path().filename()=="default.xbe")x.type="XBE";else if(ext==".iso")x.type="ISO";else continue;x.path=i->path().filename().string();x.size=i->file_size(ec);if(ec){ec.clear();x.size=0;}if(x.type=="XBE")x.sha1=Sha1File(i->path());else{kernel::Disc disc;std::string err;std::vector<uint8_t>xbe;if(disc.Open(i->path(),err)&&disc.Load("default.xbe",xbe)){x.containsXbe=true;x.sha1=Sha1Bytes(xbe);}}x.releaseCheckAvailable=!expected.empty()&&!x.sha1.empty();x.releaseMatches=x.releaseCheckAvailable&&x.sha1==expected;v.push_back(std::move(x));}}return v;}
} // namespace ios_game_data

@interface SMDocumentPickerDelegate : NSObject<UIDocumentPickerDelegate>
@end
@implementation SMDocumentPickerDelegate
- (void)documentPicker:(UIDocumentPickerViewController*)controller didPickDocumentsAtURLs:(NSArray<NSURL*>*)urls {
    if(!urls.count)return; NSURL*url=urls.firstObject; BOOL access=[url startAccessingSecurityScopedResource];
    NSString*folder=[NSString stringWithUTF8String:GameDirectory().c_str()];
    NSString*sub=[controller.title isEqualToString:@"Importar cheat"]?@"cheats":@"assets";
    NSURL*dest=[NSURL fileURLWithPath:[folder stringByAppendingPathComponent:sub] isDirectory:YES];
    [[NSFileManager defaultManager] createDirectoryAtURL:dest withIntermediateDirectories:YES attributes:nil error:nil];
    NSString*name=url.lastPathComponent; NSURL*out=[dest URLByAppendingPathComponent:name];
    NSError*err=nil; if([[NSFileManager defaultManager] fileExistsAtPath:out.path])[[NSFileManager defaultManager] removeItemAtURL:out error:nil];
    [[NSFileManager defaultManager] copyItemAtURL:url toURL:out error:&err];
    if(err) NSLog(@"Shaolin import failed: %@",err);
    if(access)[url stopAccessingSecurityScopedResource];
}
@end
static SMDocumentPickerDelegate* g_delegate=nil;
static void ShowPicker(BOOL cheat){
    dispatch_async(dispatch_get_main_queue(), ^{
        if(!g_delegate)g_delegate=[[SMDocumentPickerDelegate alloc]init];
        UIDocumentPickerViewController*p=[[UIDocumentPickerViewController alloc]initForOpeningContentTypes:@[UTTypeData] asCopy:NO];
        p.delegate=g_delegate;p.title=cheat?@"Importar cheat":@"Selecionar jogo";
        UIViewController*root=nil;
        for(UIScene*s in UIApplication.sharedApplication.connectedScenes)if(s.activationState==UISceneActivationStateForegroundActive&&[s isKindOfClass:[UIWindowScene class]]){for(UIWindow*w in ((UIWindowScene*)s).windows)if(w.isKeyWindow){root=w.rootViewController;break;}}
        while(root.presentedViewController)root=root.presentedViewController;
        [root presentViewController:p animated:YES completion:nil];
    });
}
void PickGameFile(){ShowPicker(NO);} void PickCheatFile(){ShowPicker(YES);}
namespace ios_game_data {
std::vector<std::string> ListCheatFiles(){std::vector<std::string>v;fs::path p=fs::path(GameDirectory())/"cheats";std::error_code ec;if(!fs::is_directory(p,ec))return v;for(fs::directory_iterator i(p,ec),e;!ec&&i!=e;i.increment(ec)){if(!i->is_regular_file(ec))continue;auto x=i->path().extension().string();std::transform(x.begin(),x.end(),x.begin(),[](unsigned char c){return(char)std::tolower(c);});if(x==".cht"||x==".pnach"||x==".txt")v.push_back(i->path().filename().string());}std::sort(v.begin(),v.end());return v;}
}
