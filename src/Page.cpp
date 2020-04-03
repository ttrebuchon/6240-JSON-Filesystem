#include <RamFS/RamFilesystem/Page.hh>
#include <cstring>

namespace RamFS
{
	size_t Page::write(const char* data, size_t len, size_t off)
	{
		if (off + len > 4096)
		{
			len = 4096 - off;
		}
		
		memcpy(this->data + off, data, len);
		if (off < start)
		{
			start = off;
		}
		
		if (off + len > end)
		{
			end = off + len;
		}
		
		return len;
	}
	
	size_t Page::read(char* data, size_t len, size_t off) const
	{
		if (off > end)
		{
			return 0;
		}
		
		if (off + len > end)
		{
			len = end - off;
		}
		
		memcpy(data, this->data + off, len);
		
		return len;
	}
}