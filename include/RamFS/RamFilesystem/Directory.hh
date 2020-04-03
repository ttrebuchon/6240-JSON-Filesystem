#ifndef INCLUDED_RAMFS_RAM_FILESYSTEM_DIRECTORY_HH
#define INCLUDED_RAMFS_RAM_FILESYSTEM_DIRECTORY_HH

#include <RamFS/common.hh>
#include "Node.hh"
#include <string>
#include <map>
#include <vector>

namespace RamFS
{
	
	class DirectoryNode : public Node
	{
		public:
		
		protected:
		mutable std::map<std::string, node_id_t> m_entries;
		
		Node* lookup_child(typename std::map<std::string, node_id_t>::iterator) const;

		public:
		DirectoryNode(host_t* host, node_id_t id, const json_info& jinfo) :
			Node(host, id, RAM_DIR_NODE, jinfo)
		{
			
		}
		
		
		Node* entry(const std::string& name) const;
		
		bool add_child(Node* n, const std::string& name)
		{
			if (!n || name.empty())
			{
				return false;
			}
			
			size_t i = name.find('/');
			if (i != std::string::npos)
			{
				return false;
			}
			
			if (entry(name))
			{
				return false;
			}
			
			m_entries[name] = n->id();
			return true;
		}


		const std::map<std::string, node_id_t>& entries() const
		{
			return m_entries;
		}

		std::vector<std::pair<std::string, Node*>> get_children()
		{
			std::vector<std::pair<std::string, Node*>> children;
			children.reserve(m_entries.size());
			for (auto it = m_entries.begin(); it != m_entries.end();)
			{
				Node* n = lookup_child(it);
				if (!n)
				{
					it = m_entries.erase(it);
				}
				else
				{
					children.emplace_back(it->first, n);
					++it;
				}
			}

			return children;
		}
	};
}

#endif