#ifndef INCLUDED_RAMFS_RAM_FILESYSTEM_NODE_HH
#define INCLUDED_RAMFS_RAM_FILESYSTEM_NODE_HH

#include <RamFS/common.hh>
#include "json_info.hh"
#include "TimeInfo.hh"

namespace RamFS
{
	enum NodeKind
	{
		RAM_FILE_NODE = 0x1,
		RAM_DIR_NODE = 0x2,
		RAM_LINK_NODE = 0x4,
	};
	
	class RamFilesystem;
	
	class Node
	{
		public:
		typedef RamFilesystem host_filesystem;
		typedef host_filesystem host_t;
		
		protected:
		NodeKind m_kind;
		host_t* m_host;
		node_id_t m_id;
		json_info m_jinfo;
		TimeInfo m_tinfo;
		
		public:
		Node(host_t* host, node_id_t id, NodeKind kind, const json_info& jinfo);
		virtual ~Node() = default;
		

		ALWAYS_INLINE bool has_flag(NodeKind f) const
		{
			return (m_kind & f) != 0;
		}
		
		ALWAYS_INLINE NodeKind flags() const noexcept
		{
			return m_kind;
		}
		
		ALWAYS_INLINE host_t* host() const noexcept
		{
			return m_host;
		}

		ALWAYS_INLINE node_id_t id() const noexcept
		{
			return m_id;
		}

		ALWAYS_INLINE typename json_info::jpointer_type json_pointer() const
		{
			return m_jinfo.jpointer;
		}

		ALWAYS_INLINE const nlohmann::json& get_json() const
		{
			return m_jinfo.json;
		}

		void sync_json()
		{
			(*m_jinfo.root_doc)[m_jinfo.jpointer] = m_jinfo.json;
		}

		ALWAYS_INLINE TimeInfo get_time_info() const
		{
			return m_tinfo;
		}

		ALWAYS_INLINE void set_time_info(TimeInfo info)
		{
			m_tinfo = info;
		}

		void set_accessed();
		void set_modified(bool and_changed_time = true);
		void set_changed();
	};
	
	class LinkNode;
	class DirectoryNode;
	class FileNode;
	class DeviceNode;
}

#endif