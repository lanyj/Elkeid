#ifndef GO_PROBE_BUILD_INFO_H
#define GO_PROBE_BUILD_INFO_H

#include <string>
#include <elfio/elfio.hpp>
#include <go/type/basic.h>
#include <list>
#include <vector>
#include <common/singleton.h>

struct CModule {
    std::string path;
    std::string version;
    std::string sum;
    CModule *replace;

    ~CModule() {
        if (replace) {
            delete replace;
            replace = nullptr;
        }
    }
};

struct CModuleInfo {
    std::string path;
    CModule main;
    std::list<CModule> deps;
};

class CBuildInfo {
#define gBuildInfo SINGLETON_(CBuildInfo)
public:
    bool load();
    bool load(const std::string& file);

private:
    bool readModuleInfo(const go::string *info);

public:
    go::endian mEndian;
    unsigned long mPtrSize;

public:
    std::string mVersion;
    CModuleInfo mModuleInfo;
};


#endif //GO_PROBE_BUILD_INFO_H
