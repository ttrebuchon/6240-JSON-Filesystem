#ifndef INCLUDED_RAMFS_RAM_FILESYSTEM_FILE_HH
#define INCLUDED_RAMFS_RAM_FILESYSTEM_FILE_HH

#include <RamFS/common.hh>
#include "Node.hh"
#include "FileData.hh"
#include <memory>

namespace RamFS
{
	class OpenSession;
	
	class FileNode : public Node
	{
		public:
		
		protected:
		std::shared_ptr<FileData> m_data;
		int m_users = 0;
		
		
		public:
		bool added_newline = false;
		
		FileNode(host_t* host, node_id_t id, const std::shared_ptr<FileData>& data, const json_info& jinfo) :
			Node(host, id, RAM_FILE_NODE, jinfo),
			m_data(data)
		{
			assert(data);
		}
		
		int open_session(int flags, OpenSession** ptr);
		
		
		size_t size() const;
		size_t reserved_size() const;
		
		void expand_to(size_t nsize);
		int truncate_to(size_t nsize);

		const std::shared_ptr<FileData>& get_data() const
		{
			return m_data;
		}

		nlohmann::json& get_file_json()
		{
			return m_jinfo.json;
		}
	};
}

#endif