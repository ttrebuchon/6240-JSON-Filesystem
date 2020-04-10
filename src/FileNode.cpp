#include <RamFS/RamFilesystem.hh>
#include <fcntl.h>



namespace RamFS
{
	
	int FileNode::open_session(int flags, OpenSession** ptr)
	{
		auto session = host()->create_session();
		
		
		session->node = this;
		session->data = this->m_data.get();
		++m_users;

		
		this->set_accessed();
		
		// session->can_read = ((flags & O_RDONLY) != 0);
		// session->can_write = ((flags & O_WRONLY) != 0);
		session->can_read = ((flags & O_WRONLY) == 0);
		session->can_write = ((flags & O_RDONLY) == 0);
		
		if ((flags & O_RDWR) == O_RDWR)
		{
			session->can_read = session->can_write = true;
		}
		
		session->append = ((flags & O_APPEND) != 0);

		session->can_write &= !host()->is_read_only();
		
		
		
		
		if (ptr)
		{
			*ptr = session;
		}
		
		return 0;
	}
	
	size_t FileNode::size() const
	{
		Page* p;
		for (size_t i = m_data->pages.size(); i > 0; --i)
		{
			if ((p = m_data->pages.at(i-1)))
			{
				return (i-1)*4096 + p->end;
			}
		}
		
		return 0;
	}
	
	size_t FileNode::reserved_size() const
	{
		return m_data->pages.size()*4096;
	}
	
	void FileNode::expand_to(size_t nsize)
	{
		auto pages = nsize / 4096;
		if (nsize % 4096 != 0)
		{
			++pages;
		}
		
		if (m_data->pages.size() < pages)
		{
			m_data->pages.resize(pages);
		}
	}

	int FileNode::truncate_to(size_t nsize)
	{
		auto pages = nsize / 4096;
		if (nsize % 4096 != 0)
		{
			++pages;
		}

		if (m_data->pages.size() > pages)
		{
			m_data->pages.resize(pages);
			if (pages > 0)
			{
				off_t page_offset = nsize - pages*4096;
				assert(page_offset >= 0);
				if (page_offset != 0)
				{
					m_data->pages.back()->end = (size_t)page_offset;
				}
			}
			return 0;
		}
		else
		{
			return 0;
		}
	}
}