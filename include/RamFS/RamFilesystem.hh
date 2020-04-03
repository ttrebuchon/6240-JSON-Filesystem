#ifndef INCLUDED_RAMFS_RAM_FILESYSTEM_HH
#define INCLUDED_RAMFS_RAM_FILESYSTEM_HH

#include <RamFS/common.hh>
#include "RamFilesystem/Node.hh"
#include "RamFilesystem/Directory.hh"
#include "RamFilesystem/File.hh"
#include "RamFilesystem/Link.hh"
//#include "RamFilesystem/Device.hh"
#include "RamFilesystem/Session.hh"
#include "RamFilesystem/Page.hh"
#include "RamFilesystem/TimeInfo.hh"

#include <map>
#include <set>
#include <queue>
#include <string>
#include <nlohmann/json.hpp>



namespace RamFS
{
	class RamFilesystem
	{
		public:
		typedef uint64_t session_handle;
		
		
		protected:
		std::string json_path;
		std::map<session_handle, OpenSession> m_sessions;
		DirectoryNode* m_root;
		std::set<Page*> m_pages;
		std::queue<session_handle> m_available_sessions;
		session_handle m_session_counter = 1;
		node_id_t m_id_counter = 1;
		std::map<node_id_t, Node*> m_nodes;
		nlohmann::json m_json_doc;
		TimeInfo m_doc_times;
		
		
		public:
		RamFilesystem(const std::string& JSON_path);
		
		DirectoryNode* root() const
		{
			return m_root;
		}

		TimeInfo get_document_time_info() const
		{
			return m_doc_times;
		}

		bool is_read_only() const
		{
			return false;
		}
		
		Node* get_node(const char* path) const;
		Node* get_node(node_id_t) const;
		node_id_t allocate_id()
		{
			return m_id_counter++;
		}

		
		
		protected:
		
		void initial_populate();
		void populate_from_json(DirectoryNode* parent, const std::string& name, nlohmann::json&);
		void populate_obj(DirectoryNode* dir, nlohmann::json&);
		void populate_array(DirectoryNode* dir, nlohmann::json&);
		void populate_field(FileNode* file, nlohmann::json&);
		
		int set_session(OpenSession*, fuse_file_info*);
		
		Node* resolve_if_link(Node*) const;
		
		int prepare_for_create(const char* path, DirectoryNode** parent, std::string* name);
		
		std::shared_ptr<FileData> allocate_file_data();
		OpenSession* create_session();
		void close_session(OpenSession*);
		OpenSession* session(struct fuse_file_info*);
		int flush_session(OpenSession*);

		int fill_stat_common(Node* n, struct stat*);
		int fill_stat_file(FileNode*, struct stat*);
		int fill_stat_directory(DirectoryNode*, struct stat*);
		int fill_stat_link(LinkNode*, struct stat*);
		
		void delete_node(Node* n);

		void update_source_json();

		static bool is_text(const char* data, size_t len);
		static size_t base64_length(size_t data_len);
		static size_t encode_base64(const char* data, size_t data_len, char* buf, size_t buf_len);

		static time_t current_time();
		
		
		public:
		
		/*
		*
		* All of the implementation
		* functions
		*
		*/
		
		int getattr(const char* path, struct stat* statbuf);
		int readlink(const char* path, char* link, size_t size);
		//getdir = nullptr
		int mknod(const char* path, mode_t mode, dev_t dev);
		int mkdir(const char* path, mode_t mode);
		int unlink(const char* path);
		int rmdir(const char* path);
		int symlink(const char* path, const char* link);
		int rename(const char* path, const char* new_path);
		int link(const char* path, const char* new_path);
		int chmod(const char* path, mode_t mode);
		int chown(const char* path, uid_t uid, gid_t gid);
		int truncate(const char* path, off_t new_size);
		int utimens(const char* path, const struct timespec tv[2]);
		int open(const char* path, fuse_file_info*);
		int read(const char* path, char* buf, size_t size, off_t offset, fuse_file_info*);
		int write(const char* path, const char* buf, size_t size, off_t offset, fuse_file_info*);
		int statfs(const char* path, struct statvfs*);
		int flush(const char* path, fuse_file_info*);
		int release(const char* path, fuse_file_info*);
		int fsync(const char* path, int data_sync, fuse_file_info*);
		int opendir(const char* path, fuse_file_info*);
		int readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, fuse_file_info*);
		int releasedir(const char* path, fuse_file_info*);
		int fsyncdir(const char* path, int data_sync, fuse_file_info*);
		void init(fuse_conn_info*);
		void destroy(void* user_data);
		int access(const char* path, int mask);
		int ftruncate(const char* path, off_t offset, fuse_file_info*);
		int fgetattr(const char* path, struct stat* statbuf, fuse_file_info*);
		
		int lock(const char* path, fuse_file_info*, int cmd, struct flock*);
		int ioctl(const char* path, int cmd, void* arg, struct fuse_file_info*, unsigned int flags, void* data);
		int poll(const char* path, fuse_file_info*, struct fuse_pollhandle*, unsigned*);
		int write_buf(const char* path, struct fuse_bufvec* buf, off_t off, fuse_file_info*);
		int read_buf(const char* path, struct fuse_bufvec** bufp, size_t size, off_t off, fuse_file_info*);
		int flock(const char* path, struct fuse_file_info*, int op);
		int fallocate(const char* path, int, off_t, off_t, struct fuse_file_info*);
		
		
		
		friend class FileNode;
		
	};
}

#endif