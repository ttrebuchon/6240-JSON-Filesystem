#include <RamFS/RamFilesystem.hh>
#include <RamFS/RamFilesystem/Node.hh>
#include <fcntl.h>



namespace RamFS
{
	Node::Node(host_t* host, node_id_t id, NodeKind kind, const json_info& jinfo) :
		m_kind(kind),
		m_host(host),
		m_id(id),
		m_jinfo(jinfo),
		m_tinfo(host->get_document_time_info())
	{
		assert(m_jinfo.root_doc);
	}

	void Node::set_accessed()
	{
		timespec now;
		clock_gettime(CLOCK_REALTIME, &now);

		this->m_tinfo.accessed = now;
	}

	void Node::set_modified(bool and_changed_time)
	{
		timespec now;
		clock_gettime(CLOCK_REALTIME, &now);

		this->m_tinfo.modified = now;
		if (and_changed_time)
		{
			this->m_tinfo.changed = now;
		}
	}

	void Node::set_changed()
	{
		timespec now;
		clock_gettime(CLOCK_REALTIME, &now);

		this->m_tinfo.changed = now;
	}
}