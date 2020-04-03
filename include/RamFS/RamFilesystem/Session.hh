#ifndef INCLUDED_RAMFS_RAM_FILESYSTEM_SESSION_HH
#define INCLUDED_RAMFS_RAM_FILESYSTEM_SESSION_HH

#include <RamFS/common.hh>
#include "Node.hh"

namespace RamFS
{
	
	class FileData;
	
	struct OpenSession
	{
		uint64_t handle;
		
		FileNode* node;
		FileData* data;
		
		bool can_read : 1;
		bool can_write : 1;
		bool append : 1;
		
		
		int write(const char* buf, size_t len, off_t off);
		int read(char* buf, size_t len, off_t off) const;
	};
}

#endif