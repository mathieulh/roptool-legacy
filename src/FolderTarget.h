#ifndef _FOLDER_TARGET_H_
#define _FOLDER_TARGET_H_

// roptool
#include "Target.h"
#include "Gadget.h"

// std
#include <string>
#include <vector>

// boost
#include <boost/filesystem.hpp>

class FolderTarget : public Target
{
	public:
		FolderTarget(void) { }
		FolderTarget(const std::string& name) { setName(name); }
		
		// accessor/mutator
		const std::string& name(void) { return m_name; }
		void setName(const std::string& name);
		
		// 
		TargetManifestPtr manifest(void) { return m_manifest; }
		
	private:
		std::string m_name;
		std::string m_path;
		GadgetPtrList m_gadgets;
		TargetManifestPtr m_manifest;
		
		void readManifest(void);
		void readGadgets(void);
		
		typedef std::vector<boost::filesystem::path> DirectoryList;
		DirectoryList read_directory(const boost::filesystem::path& dir);
};

#endif // _FOLDER_TARGET_H_