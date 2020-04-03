#ifndef INCLUDED_RAMFS_RAM_FILESYSTEM_FILE_DATA_HH
#define INCLUDED_RAMFS_RAM_FILESYSTEM_FILE_DATA_HH

#include <RamFS/common.hh>
#include "Page.hh"
#include <vector>
#include <queue>

namespace RamFS
{
	
	class FileData
	{
		public:
		
		protected:
		
		
		public:
		std::vector<Page*> pages;
		
		~FileData()
		{
			for (auto& p : pages)
			{
				if (p)
				{
					delete p;
					p = nullptr;
				}
			}
			
			pages.clear();
		}
		
		
		Page* page(size_t index)
		{
			Page* p = pages.at(index);
			if (!p)
			{
				p = pages.at(index) = new Page();
				if (index < pages.size() - 1)
				{
					p->end = 4096;
				}
			}
			
			return p;
		}
	};
}

#endif