#include <RamFS/RamFilesystem.hh>
#include <fcntl.h>


namespace RamFS
{
	#define PG (4096)
	
	int OpenSession::write(const char* buf, size_t len, off_t _off)
	{
		assert(this->data);
		assert(_off >= 0);
		size_t off = (size_t)_off;
		
		if (len + off > node->reserved_size())
		{
			node->expand_to(len + off);
		}
		
		auto it = off;
		if (off % PG != 0)
		{
			auto pg = off / PG;
			auto pg_off = off % PG;
			/*auto rem = std::min((size_t)(PG - pg_off), len);
			memcpy(data->page(pg)->data + pg_off, buf, rem);
			it += rem;*/
			it += data->page(pg)->write(buf, len, pg_off);
		}
		
		//for (size_t p = it / PG; p < (len + off) / PG; ++p)
		while (it < len + off)
		{
			size_t p = it / PG;
			assert(it % PG == 0);
			it += data->page(p)->write(buf + (p*PG - off), len + off - it, it - p*PG);
			/*memcpy(data->page(p)->data, buf + (p*PG - off), PG);
			it += PG;*/
			
		}
		
		/*if (off + len % PG != 0 && ((off + len)/PG != off/PG))
		{
			auto pg = (off + len)/PG;
			auto rem = len - (pg*PG - off);
			memcpy(data->page(pg)->data, buf + pg*PG - off, rem);
			it += rem;
		}*/
		
		assert(it == off + len);
		
		node->set_modified();
		return len;
	}
	
	
	
	
	
	
	int OpenSession::read(char* buf, size_t len, off_t _off) const
	{
		assert(this->data);
		assert(_off >= 0);
		size_t off = (size_t)_off;
		const size_t sz = node->size();
		
		if (off > sz)
		{
			return 0;
		}
		else if (len + off > sz)
		{
			len = sz - off;
		}
		
		auto it = off;
		if (off % PG != 0)
		{
			auto pg = off / PG;
			auto pg_off = off % PG;
			/*auto rem = std::min((size_t)(PG - pg_off), len);
			memcpy(data->page(pg)->data + pg_off, buf, rem);
			it += rem;*/
			it += data->page(pg)->read(buf, len, pg_off);
		}
		
		//for (size_t p = it / PG; p < (len + off) / PG; ++p)
		while (it < len + off)
		{
			size_t p = it / PG;
			assert(it % PG == 0);
			it += data->page(p)->read(buf + (p*PG - off), len + off - it, it - p*PG);
			/*memcpy(data->page(p)->data, buf + (p*PG - off), PG);
			it += PG;*/
			
		}
		
		/*if (off + len % PG != 0 && ((off + len)/PG != off/PG))
		{
			auto pg = (off + len)/PG;
			auto rem = len - (pg*PG - off);
			memcpy(data->page(pg)->data, buf + pg*PG - off, rem);
			it += rem;
		}*/
		
		assert(it == off + len);
		

		node->set_accessed();
		return len;
	}
	
}