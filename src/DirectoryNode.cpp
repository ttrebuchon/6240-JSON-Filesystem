#include <RamFS/RamFilesystem.hh>
#include <fcntl.h>



namespace RamFS
{

	Node* DirectoryNode::lookup_child(typename std::map<std::string,node_id_t>::iterator it) const
	{
		if (it == m_entries.end())
		{
			return nullptr;
		}

		return host()->get_node(it->second);
	}


	Node* DirectoryNode::entry(const std::string& name) const
	{
		auto it = m_entries.find(name);
		if (it != m_entries.end())
		{
			Node* n = host()->get_node(it->second);
			if (!n)
			{
				m_entries.erase(it);
				return nullptr;
			}
			else
			{
				return n;
			}
		}
		else
		{
			return nullptr;
		}
	}
}