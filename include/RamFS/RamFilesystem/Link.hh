#ifndef INCLUDED_RAMFS_RAM_FILESYSTEM_LINK_HH
#define INCLUDED_RAMFS_RAM_FILESYSTEM_LINK_HH

#include <RamFS/common.hh>
#include "Node.hh"
#include <cassert>

namespace RamFS
{
	
	class LinkNode : public Node
	{
		public:
		
		protected:
		
		public:
		
		int resolve(Node**) const
		{
			bool TODO = false;
			assert(TODO);
			return -1;
		}
	};
}

#endif