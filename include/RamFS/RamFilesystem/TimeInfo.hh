#ifndef INCLUDED_RAMFS_RAM_FILESYSTEM_TIME_INFO_HH
#define INCLUDED_RAMFS_RAM_FILESYSTEM_TIME_INFO_HH

#include "../common.hh"
#include <ctime>

namespace RamFS
{
	struct TimeInfo
	{
		timespec modified;
		timespec accessed;
		timespec changed;
	};
}

#endif