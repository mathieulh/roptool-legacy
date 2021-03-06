#ifndef _TARGET_H_
#define _TARGET_H_

// roptool
#include "TargetManifest.h"
#include "GadgetMap.h"

// std
#include <string>
#include <memory>

// boost
#include <boost/filesystem.hpp>

// windows
#ifdef _WIN32
#include <Windows.h>
#endif

// osx
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

class Target
{
    public:
        virtual const std::string& name(void) = 0;
        virtual void setName(const std::string& name) = 0;
        virtual TargetManifestPtr manifest(void) = 0;
        
        virtual GadgetMapPtr bestGadgetMap(const std::string& regex) = 0;
        
        virtual GadgetPtr getCallerGadget(void) = 0;
        
        virtual bool isFunction(const std::string& function) const = 0;
        
        /* GadgetMapList gadgetMaps(void) = 0;
        GadgetList gadgets(void) = 0; */
    
    protected:
        std::string target_folder(void)
        {
            char data[256];
            memset(data, 0, sizeof(data));
#ifdef _WIN32
            GetModuleFileName(NULL, &data[0], sizeof(data));
#elif defined __linux__
            readlink("/proc/self/exe", data, sizeof(data));
#elif defined __APPLE__
            unsigned int size = 256;
            _NSGetExecutablePath(data, &size);
#endif

            return boost::filesystem::path(data).parent_path().string()+"/pkg";
        }
};

typedef std::shared_ptr<Target> TargetPtr;

#endif // _TARGET_H_
