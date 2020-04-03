#ifndef INCLUDED_RAMFS_RAM_FILESYSTEM_PAGE_HH
#define INCLUDED_RAMFS_RAM_FILESYSTEM_PAGE_HH

#include <RamFS/common.hh>
#include "Node.hh"

namespace RamFS
{
	
	class Page
	{
		public:
		
		protected:
		
		
		public:
		size_t start = 0;
		size_t end = 0;
		uint8_t data[4096] = {0};
		
		
		size_t write(const char* data, size_t len, size_t off);
		size_t read(char* data, size_t len, size_t off) const;
	};
}

#endif