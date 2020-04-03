
#include <RamFS/RamFilesystem.hh>
#include <cassert>
#include <errno.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <libgen.h>
#include <cstring>
#include <linux/limits.h>

namespace RamFS
{
	
	
	
	RamFilesystem::RamFilesystem(const std::string& JSON_path) :
		json_path(JSON_path),
		m_sessions(),
		m_root(nullptr),
		m_pages(),
		m_available_sessions(),
		m_id_counter(1),
		m_nodes(),
		m_json_doc()
	{
		

		std::ifstream json_in(JSON_path);
		assert(json_in.good());

		json_in >> m_json_doc;

		struct stat doc_stat;
		if (::stat(JSON_path.c_str(), &doc_stat) == 0)
		{
			m_doc_times.accessed = doc_stat.st_atim;
			m_doc_times.changed = doc_stat.st_ctim;
			m_doc_times.modified = doc_stat.st_mtim;
		}
		else
		{
			timespec now;
			clock_gettime(CLOCK_REALTIME, &now);
			m_doc_times.accessed = m_doc_times.changed = m_doc_times.modified = now;
		}
		

		node_id_t root_id = m_id_counter++;
		m_root = new DirectoryNode(this, root_id, json_info(&m_json_doc, typename json_info::jpointer_type("")));
		m_nodes[root_id] = m_root;

		this->initial_populate();
	}
	
	
	
	Node* RamFilesystem::get_node(const char* path) const
	{
		assert(path != nullptr);
		char new_path[PATH_MAX];
		/*if (!realpath(path, new_path))
		{
			return nullptr;
		}*/
		
		strncpy(new_path, path, PATH_MAX);
		
		std::list<std::string> parts;
		char* it = new_path;
		char* last = new_path;
		if (*it == '/')
		{
			++it;
			++last;
		}
		for (; *it != '\0'; ++it)
		{
			if (*it == '/')
			{
				if (it - last > 0)
				{
					parts.emplace_back(last, it - last);
				}
				last = it + 1;
			}
		}
		
		if (it - last > 0)
		{
			parts.emplace_back(last, it - last);
		}


		if (parts.empty())
		{
			// Referring to root
			return m_root;
		}
		
		
		DirectoryNode* dnode = m_root;
		Node* node = m_root;
		size_t i = 0;
		for (const auto& part : parts)
		{
			++i;
			if (part == ".")
			{
				continue;
			}
			
			node = dnode->entry(part);
			if (!node)
			{
				return nullptr;
			}
			
			if (node->has_flag(RAM_DIR_NODE))
			{
				if (node->has_flag(RAM_LINK_NODE))
				{
					if (((LinkNode*)node)->resolve(&node) != 0)
					{
						return nullptr;
					}
					
					assert(node->has_flag(RAM_DIR_NODE));
				}
				
				dnode = (DirectoryNode*)node;
			}
			else
			{
				dnode = nullptr;
				break;
			}
			
		}
		
		if (i != parts.size())
		{
			return nullptr;
		}
		else
		{
			return node;
		}
		
		
	}

	Node* RamFilesystem::get_node(node_id_t id) const
	{
		auto it = m_nodes.find(id);
		if (it != m_nodes.end())
		{
			return it->second;
		}
		else
		{
			return nullptr;
		}
	}






	void RamFilesystem::initial_populate()
	{
		assert(m_json_doc.is_object());
		this->populate_obj(m_root, m_json_doc);

		std::cout << "Initial JSON: \n" << m_json_doc.dump(4) << std::endl;
	}

	void RamFilesystem::populate_from_json(DirectoryNode* dir, const std::string& name, nlohmann::json& val)
	{
		assert(!name.empty());
		if (name.empty())
		{
			return;
		}

		try
		{
			if (val.is_object() || val.is_array())
			{
				node_id_t new_id = allocate_id();
				auto node = new DirectoryNode(this, new_id, json_info(&m_json_doc, dir->json_pointer() / name));
				m_nodes[new_id] = node;

				dir->add_child(node, name);
				
				if (val.is_object())
				{
					populate_obj(node, val);
				}
				else
				{
					populate_array(node, val);
				}
			}
			else if (val.is_string() || val.is_boolean() || val.is_number())
			{
				node_id_t new_id = allocate_id();
				std::shared_ptr<FileData> fdata = allocate_file_data();
				auto node = new FileNode(this, new_id, fdata, json_info(&m_json_doc, dir->json_pointer() / name));
				m_nodes[new_id] = node;
		
				bool add_res = dir->add_child(node, name);
				assert(add_res);
				assert(dir->entry(name));

				populate_field(node, val);
			}
			else if (val.is_null())
			{
				// Do nothing
			}
		}
		catch (const std::exception& ex)
		{
			std::cout << std::string(10, '\n') << "Exception Caught: " << ex.what() << std::string(10, '\n') << std::endl;
		}
	}

	void RamFilesystem::populate_obj(DirectoryNode* dir, nlohmann::json& j)
	{
		assert(dir);

		assert(j.is_object());

		for (auto kv = j.begin(); kv != j.end(); ++kv)
		{
			if (kv.key().empty())
			{
				continue;
			}

			populate_from_json(dir, kv.key(), kv.value());
		}
	}

	void RamFilesystem::populate_array(DirectoryNode* dir, nlohmann::json& j)
	{
		assert(dir);

		assert(j.is_array());

		size_t len = j.size();

		for (size_t i = 0; i < len; ++i)
		{
			populate_from_json(dir, std::to_string(i), j[i]);
		}
	}

	void RamFilesystem::populate_field(FileNode* file, nlohmann::json& j)
	{
		assert(file);
		auto data = file->get_data();
		std::string str_contents;

		if (j.is_string())
		{
			str_contents = j.get<std::string>();
		}
		else if (j.is_number())
		{
			if (j.is_number_float())
			{
				double n = j.get<double>();
				str_contents = std::to_string(n);
			}
			else if (j.is_number_integer())
			{
				long n = j.get<long>();
				str_contents = std::to_string(n);
			}
			else if (j.is_number_unsigned())
			{
				unsigned long n = j.get<unsigned long>();
				str_contents = std::to_string(n);
			}
			else
			{
				// TODO
				NOT_IMPLEMENTED;
			}
			
		}
		else if (j.is_boolean())
		{
			bool b = j.get<bool>();
			if (b)
			{
				str_contents = "true";
			}
			else
			{
				str_contents = "false";
			}
		}
		else
		{
			// TODO
			NOT_IMPLEMENTED;
		}

		if (str_contents.empty())
		{
			return;
		}

		if (str_contents[str_contents.length() - 1] != '\n')
		{
			str_contents += '\n';
			file->added_newline = true;
		}


		file->expand_to(str_contents.length());

		OpenSession* session = nullptr;
		int rc = file->open_session(O_WRONLY, &session);
		if (rc != 0)
		{
			assert(!session);
			return;
		}
		assert(session);

		session->write(str_contents.c_str(), str_contents.length(), 0);
		close_session(session);
	}


	
	int RamFilesystem::set_session(OpenSession* s, fuse_file_info* fi)
	{
		assert(s);
		assert(fi);
		fi->fh = s->handle;
		
		return 0;
	}
	
	Node* RamFilesystem::resolve_if_link(Node* n) const
	{
		if (!n)
		{
			return nullptr;
		}
		
		
		Node* result = nullptr;
		while (n && n->has_flag(RAM_LINK_NODE))
		{
			if (((LinkNode*)n)->resolve(&result) != 0)
			{
				return nullptr;
			}
			else
			{
				n = result;
			}
		}
		
		return n;
	}
	
	int RamFilesystem::prepare_for_create(const char* path, DirectoryNode** parent, std::string* name)
	{
		auto path_len = strlen(path);
		char path_copy[path_len + 1];
		strncpy(path_copy, path, path_len+1);
		
		char* dir = ::dirname(path_copy);
		
		auto parentn = get_node(dir);
		parentn = resolve_if_link(parentn);
		if (!parentn)
		{
			return -ENOENT;
		}
		
		if (!parentn->has_flag(RAM_DIR_NODE))
		{
			return -ENOENT;
		}
		
		auto parentd = (DirectoryNode*)parentn;
		
		if (parent)
		{
			*parent = parentd;
		}
		
		
		if (name)
		{
			strncpy(path_copy, path, path_len+1);
			*name = std::string(::basename(path_copy));
		}
		
		return 0;
	}
	
	std::shared_ptr<FileData> RamFilesystem::allocate_file_data()
	{
		auto data = std::make_shared<FileData>();
		
		return data;
	}
	
	OpenSession* RamFilesystem::create_session()
	{
		if (m_available_sessions.empty())
		{
			for (int i = 0; i < 10; ++i)
			{
				session_handle j = m_session_counter++;
				m_available_sessions.push(j);
				m_sessions[j].handle = j;
			}
		}
		
		session_handle handle = m_available_sessions.front();
		m_available_sessions.pop();
		
		auto session = &m_sessions[handle];
		
		*session = OpenSession();
		session->handle = handle;
		
		return session;
	}

	void RamFilesystem::close_session(OpenSession* session)
	{
		assert(session);
		if (!session)
		{
			return;
		}

		m_available_sessions.push(session->handle);
	}
	
	OpenSession* RamFilesystem::session(struct fuse_file_info* fi)
	{
		if (fi)
		{
			auto it = m_sessions.find(fi->fh);
			if (it != m_sessions.end())
			{
				return &it->second;
			}
		}
		
		return nullptr;
	}

	int RamFilesystem::flush_session(OpenSession* s)
	{
		assert(s);
		if (!s)
		{
			return -EINVAL;
		}

		auto file = s->node;
		assert(file);

		auto fdata = file->get_data();
		if (!fdata)
		{
			return 0;
		}

		size_t len = file->size();
		char str_data[len + 1];
		s->read(str_data, len, 0);
		if (len == 0 || str_data[len-1] != '\0')
		{
			str_data[len] = '\0';
		}
		else
		{
			--len;
		}

		

		if (len > 1)
		{
			if (str_data[len-1] == '\n' && file->added_newline)
			{
				str_data[len-1] = '\0';
				--len;
			}
		}


		
		

		if (file->get_file_json().is_string() || file->get_file_json().is_null())
		{
			if (is_text(str_data, len))
			{
				file->get_file_json() = std::string(str_data);
			}
			else
			{
				char encoded[base64_length(len)];
				encode_base64(str_data, len, encoded, sizeof(encoded));
				file->get_file_json() = std::string(encoded);
			}
			
		}
		else if (file->get_file_json().is_number())
		{
			std::cout << file->json_pointer() << std::endl;
			


			// TODO
			NOT_IMPLEMENTED
			assert(false);
		}
		else if (file->get_file_json().is_boolean())
		{
			std::cout << file->json_pointer() << std::endl;

			if (strncmp(str_data, "true", 4) == 0)
			{
				file->get_file_json() = true;
			}
			else if (strncmp(str_data, "false", 5) == 0)
			{
				file->get_file_json() = false;
			}
			else
			{
				file->get_file_json() = std::string(str_data);
			}
		}

		

		file->sync_json();

		

		

		this->update_source_json();

		return 0;
	}

	int RamFilesystem::fill_stat_common(Node* n, struct stat* buf)
	{
		// TODO

		buf->st_mode = 0;

		// TODO: More st_mode flags for the actual mode itself
		mode_t dummy_mask = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH;
		buf->st_mode |= dummy_mask;


		// TODO: User/Group flags


		auto time_info = n->get_time_info();
		buf->st_atim = time_info.accessed;	// Last Access
		buf->st_mtim = time_info.modified;	// Last Modify
		buf->st_ctim = time_info.changed;	// Last Status Change

		return 0;
	}

	int RamFilesystem::fill_stat_file(FileNode* file, struct stat* buf)
	{
		buf->st_mode |= S_IFREG;

		buf->st_nlink = file->get_data().use_count();

		buf->st_size = file->size();
		buf->st_blocks = file->reserved_size() / 512;

		// TODO

		return 0;
	}

	int RamFilesystem::fill_stat_directory(DirectoryNode* dir, struct stat* buf)
	{
		buf->st_mode |= S_IFDIR;

		buf->st_nlink = 1;
		buf->st_size = 512;
		buf->st_blocks = 1;
		

		// TODO

		return 0;
	}

	int RamFilesystem::fill_stat_link(LinkNode* link, struct stat* buf)
	{
		buf->st_mode |= S_IFLNK;

		buf->st_nlink = 1;
		buf->st_size = 512;
		buf->st_blocks = 1;

		// TODO

		return 0;
	}

	void RamFilesystem::delete_node(Node* n)
	{
		assert(n);
		if (n)
		{
			node_id_t id = n->id();
			auto it = m_nodes.find(id);
			if (it != m_nodes.end())
			{
				m_nodes.erase(it);
			}

			auto ptr = n->json_pointer().parent_pointer();
			this->m_json_doc[ptr].erase(n->json_pointer().back());
			delete n;
			this->update_source_json();
		}
	}

	void RamFilesystem::update_source_json()
	{
		std::ofstream os(json_path);
		os << m_json_doc.dump(4) << std::flush;
		os.close();
	}

	bool RamFilesystem::is_text(const char* data, size_t len)
	{
		for (size_t i = 0; i < len; ++i)
		{
			if (data[i] == '\0')
			{
				return false;
			}
			else
			{
				// TODO
			}
		}

		return true;
	}

	static char base64_charset[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	static unsigned char get_bits(char c, int left, int count)
	{
		return ((unsigned char)(   ((unsigned char)c) << left) >> (8 - count));
	}

	size_t RamFilesystem::base64_length(size_t data_len)
	{
		return ((data_len * 8) / 6) + ((data_len * 8) % 3) + 1;
	}

	size_t RamFilesystem::encode_base64(const char* data, size_t data_len, char* buf, size_t buf_len)
	{
		size_t buf_indx = 0;
		size_t padding = 0;
		int val = 0;
		size_t bit_count;
		size_t index;

		for (size_t i = 0; i < data_len; i += 3)
		{
			val = 0;
			bit_count = 0;
			for (size_t j = i; j <= i + 2 && j < data_len; ++j)
			{
				val <<= 8;
				val |= data[j];
				bit_count += 8;
			}

			padding = bit_count % 3;

			if (buf_indx >= buf_len)
			{
				return 0;
			}

			while (bit_count > 0 && buf_indx < buf_len)
			{
				if (bit_count >= 6)
				{
					auto tmp = bit_count - 6;
					index = (val >> tmp) & 63;
					bit_count -= 6;
				}
				else
				{
					auto tmp = 6 - bit_count;
					index = (val << tmp) & 63;
					bit_count = 0;
				}

				buf[buf_indx++] = base64_charset[index];
			}
		}

		if (buf_indx + padding >= buf_len)
		{
			return 0;
		}

		for (size_t i = 0; i < padding; ++i)
		{
			buf[buf_indx++] = '=';
		}

		if (buf_indx < buf_len)
		{
			buf[buf_indx++] = '\0';
			return buf_indx;
		}
		else
		{
			buf[buf_len - 1] = '\0';
			return 0;
		}
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	int RamFilesystem::getattr(const char* path, struct stat* statbuf)
	{
		// Ignore st_dev
		// Ignore st_blksize
		// Ignore st_ino

		assert(statbuf);


		auto node = this->get_node(path);
		if (!node)
		{
			return -ENOENT;
		}

		int rc = fill_stat_common(node, statbuf);
		if (rc != 0)
		{
			return rc;
		}

		if (node->has_flag(RAM_LINK_NODE))
		{
			rc = fill_stat_link((LinkNode*)node, statbuf);
		}
		else if (node->has_flag(RAM_DIR_NODE))
		{
			rc = fill_stat_directory((DirectoryNode*)node, statbuf);
		}
		else if (node->has_flag(RAM_FILE_NODE))
		{
			rc = fill_stat_file((FileNode*)node, statbuf);
		}

		
		return rc;
	}
	
	
	int RamFilesystem::readlink(const char* path, char* link, size_t size)
	{
		// TODO
		NOT_IMPLEMENTED
		assert(false);
		return -1;
	}
		
	
	int RamFilesystem::mknod(const char* path, mode_t mode, dev_t dev)
	{
		DirectoryNode* parent;
		std::string name;
		if (mode & S_IFREG)
		{
			int rc = prepare_for_create(path, &parent, &name);
			if (rc != 0)
			{
				return rc;
			}
			
			node_id_t new_id = allocate_id();
			auto node = new FileNode(this, new_id, allocate_file_data(), json_info(&m_json_doc, parent->json_pointer() / name));
			m_nodes[new_id] = node;
			bool result = parent->add_child(node, name);
			assert(result);
			return 0;
		}
		
		NOT_IMPLEMENTED
		return -1;
	}
		
	
	int RamFilesystem::mkdir(const char* path, mode_t mode)
	{
		mode |= S_IFDIR;

		std::string name;
		DirectoryNode* parent = nullptr;
		int rc = prepare_for_create(path, &parent, &name);
		if (rc != 0)
		{
			return rc;
		}

		assert(parent);
		assert(!name.empty());
		
		
		
		
		node_id_t new_id = allocate_id();
		auto node = new DirectoryNode(this, new_id, json_info(&m_json_doc, parent->json_pointer() / name));
		m_nodes[new_id] = node;
		
		parent->add_child(node, name);
		
		// TODO: Permissions
		
		return 0;
	}
		
	
	int RamFilesystem::unlink(const char* path)
	{

		auto node = get_node(path);
		if (!node)
		{
			return -ENOENT;
		}


		if (node->has_flag(RAM_LINK_NODE))
		{
			// TODO
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
		else if (node->has_flag(RAM_FILE_NODE))
		{
			this->delete_node(node);
			return 0;
		}
		else if (node->has_flag(RAM_DIR_NODE))
		{
			// TODO
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
		else
		{
			// TODO
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	
	int RamFilesystem::rmdir(const char* path)
	{
		// TODO
		NOT_IMPLEMENTED
		assert(false);
		return -1;
	}
		
	
	int RamFilesystem::symlink(const char* path, const char* link)
	{
		// TODO
		NOT_IMPLEMENTED
		assert(false);
		return -1;
	}
		
	
	int RamFilesystem::rename(const char* path, const char* new_path)
	{
		// TODO
		NOT_IMPLEMENTED
		assert(false);
		return -1;
	}
		
	
	int RamFilesystem::link(const char* path, const char* new_path)
	{
		// TODO
		NOT_IMPLEMENTED
		assert(false);
		return -1;
	}
		
	
	int RamFilesystem::chmod(const char* path, mode_t mode)
	{
		NOT_IMPLEMENTED
		return -1;
	}
		
	
	int RamFilesystem::chown(const char* path, uid_t uid, gid_t gid)
	{
		// TODO
		NOT_IMPLEMENTED
		assert(false);
		return -1;
	}
		
	
	int RamFilesystem::truncate(const char* path, off_t new_size)
	{
		auto node = get_node(path);
		node = resolve_if_link(node);
		if (!node)
		{
			return -ENOENT;
		}

		if (!node->has_flag(RAM_FILE_NODE))
		{
			return -EISDIR;
		}

		FileNode* file = (FileNode*)node;

		if (new_size < 0)
		{
			return -EINVAL;
		}
		
		if (file->size() < (size_t)new_size)
		{
			file->expand_to((size_t)new_size);
			return 0;
		}
		else
		{
			return file->truncate_to((size_t)new_size);
		}

		
	}
		
	
	/*int RamFilesystem::utime(const char* path, utimbuf* buf)
	{
		return -1;
	}*/
		
	
	int RamFilesystem::open(const char* path, fuse_file_info* info)
	{
		std::cout << __func__ << std::endl;
		int rc = 0;
		auto node = get_node(path);
		node = resolve_if_link(node);
		const auto flags = info->flags;
		if (node)
		{
			if (node->has_flag(RAM_FILE_NODE))
			{
				FileNode* file = (FileNode*)node;
				
				OpenSession* session;
				rc = file->open_session(flags, &session);
				if (rc != 0)
				{
					return rc;
				}
				
				
				
				rc = this->set_session(session, info);
				return rc;
			}
			else //if (node->has_flag(RAM_DIR_NODE))
			{
				return -EISDIR;
			}
		}
		else
		{
			return -ENOENT;
		}
		return -1;
	}
		
	
	int RamFilesystem::read(const char* path, char* buf, size_t size, off_t offset, fuse_file_info* fi)
	{
		auto s = session(fi);
		if (!s)
		{
			return -EINVAL;
		}
		
		assert(s->node);
		return s->read(buf, size, offset);
	}
		
	
	int RamFilesystem::write(const char* path, const char* buf, size_t size, off_t offset, fuse_file_info* fi)
	{
		auto s = session(fi);
		if (!s)
		{
			return -EINVAL;
		}
		
		assert(s->node);
		return s->write(buf, size, offset);
	}
		
	
	int RamFilesystem::statfs(const char* path, struct statvfs*)
	{
		NOT_IMPLEMENTED
		// TODO
		assert(false);
		return -1;
	}
		
	
	int RamFilesystem::flush(const char* path, fuse_file_info* fi)
	{
		// At the moment, not doing anything here
		return 0;
	}
		
	
	int RamFilesystem::release(const char* path, fuse_file_info* fi)
	{
		int rc = 0;
		auto session = this->session(fi);
		if (!session)
		{
			return -EINVAL;
		}

		if (session->can_write)
		{
			rc = this->flush_session(session);
		}
		
		m_available_sessions.push(fi->fh);
		return rc;
	}
		
	
	int RamFilesystem::fsync(const char* path, int data_sync, fuse_file_info*)
	{
		std::cout << std::string(20, '\n') << std::endl;

		while (true) ;

		NOT_IMPLEMENTED
		// TODO
		assert(false);
		return -1;
	}
	
	
	
	int RamFilesystem::opendir(const char* path, fuse_file_info* fi)
	{
		std::cout << __func__ << std::endl;
		std::cout << "\t" << path << std::endl;

		auto node = get_node(path);
		if (!node)
		{
			return -ENOENT;
		}

		if (!node->has_flag(RAM_DIR_NODE))
		{
			return -ENOTDIR;
		}

		node = resolve_if_link(node);
		if (!node)
		{
			return -ENOENT;
		}

		// Make doubly sure the targeted node is *actually* a directory
		if (!node->has_flag(RAM_DIR_NODE))
		{
			return -ENOTDIR;
		}


		// TODO: Permission check
		// DirectoryNode* dir = (DirectoryNode*)node;



		fi->fh = (uintptr_t)node;

		std::cout << "opendir() returning 0" << std::endl;

		return 0;
	}
		
	
	int RamFilesystem::readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, fuse_file_info* fi)
	{
		assert(fi);
		
		DirectoryNode* dir = nullptr;
		if (fi->fh != 0)
		{
			dir = (DirectoryNode*)(Node*)fi->fh;
		}
		else
		{
			auto n = get_node(path);
			n = resolve_if_link(n);
			if (n)
			{
				if (n->has_flag(RAM_DIR_NODE))
				{
					dir = (DirectoryNode*)n;
				}
			}

			if (!dir)
			{
				return -ENOENT;
			}
		}

		int rc;

		for (const auto& pair : dir->get_children())
		{
			rc = filler(buf, pair.first.c_str(), nullptr, 0);
			std::cout << "Filling buf with " << pair.first.c_str() << std::endl;
			if (rc == 1)
			{
				return -ENOMEM;
			}
		}

		std::cout << "readdir() returning 0" << std::endl;

		return 0;
	}
		
	
	int RamFilesystem::releasedir(const char* path, fuse_file_info*)
	{
		// At the moment, not doing anything here
		return 0;
	}
		
	
	int RamFilesystem::fsyncdir(const char* path, int data_sync, fuse_file_info*)
	{
		NOT_IMPLEMENTED
		// TODO
		assert(false);
		return -1;
	}
		
	
	
	int RamFilesystem::access(const char* path, int mask)
	{
		auto node = get_node(path);
		if (!node)
		{
			return -ENOENT;
		}

		std::cout << "access() mask = " << mask << std::endl;
		std::cout << std::flush;

		

		if (mask == F_OK)
		{
			return 0;
		}



		if (mask & R_OK)
		{
			// TODO
		}

		if (mask & W_OK)
		{
			// TODO
		}

		if (mask & X_OK)
		{
			// TODO
		}

		NOT_IMPLEMENTED
		// TODO
		assert(false);
	}
		
	
	int RamFilesystem::ftruncate(const char* path, off_t offset, fuse_file_info*)
	{
		NOT_IMPLEMENTED
		// TODO
		assert(false);
		return -1;
	}

	int RamFilesystem::utimens(const char* path, const struct timespec tv[2])
	{
		if (!tv)
		{
			return -EACCES;
		}

		assert(path);

		if (this->is_read_only())
		{
			return -EROFS;
		}

		Node* node = get_node(path);
		if (!node)
		{
			return -ENOENT;
		}

		node = resolve_if_link(node);
		assert(node);

		
		TimeInfo info = node->get_time_info();
		info.accessed = tv[0];
		info.modified = tv[1];

		node->set_time_info(info);


		return 0;
	}
		
	
	int RamFilesystem::fgetattr(const char* path, struct stat* statbuf, fuse_file_info*)
	{
		NOT_IMPLEMENTED
		// TODO
		assert(false);
		return -1;
	}
	
}