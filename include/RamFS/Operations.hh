#ifndef INCLUDED_RAMFS_OPERATIONS_HH
#define INCLUDED_RAMFS_OPERATIONS_HH

#include <RamFS/common.hh>

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <climits>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>

#include "detail/default_constructible.hh"



namespace RamFS
{
	template <class FS>
	class FUSEOperations
	{
		public:
		typedef FS implementation_type;
		
		protected:
		static FUSEOperations* get_ops_object();
		
		fuse_operations* fuse_ops = nullptr;
		bool owns_fs = false;
		
		public:
		FS* fs = nullptr;
		
		
		
		static void set_fuse_operations(fuse_operations*);
		
		static int filesystem_main(int argc, char** argv, FS& imp);
		
		int main(int argc, char** argv);
		
		
		
		
		
		
		/*
		*
		* FUSE Operation Forwarding
		*
		*/
		
		
		static int getattr(const char* path, struct stat* statbuf);
		
		static int readlink(const char* path, char* link, size_t size);
		
		//getdir = nullptr
		
		static int mknod(const char* path, mode_t mode, dev_t dev);
		
		static int mkdir(const char* path, mode_t mode);
		
		static int unlink(const char* path);
		
		static int rmdir(const char* path);
		
		static int symlink(const char* path, const char* link);
		
		static int rename(const char* path, const char* new_path);
		
		static int link(const char* path, const char* new_path);
		
		static int chmod(const char* path, mode_t mode);
		
		static int chown(const char* path, uid_t uid, gid_t gid);
		
		static int truncate(const char* path, off_t new_size);
		
		static int utimens(const char* path, const struct timespec tv[2]);
		
		static int open(const char* path, fuse_file_info*);
		
		static int read(const char* path, char* buf, size_t size, off_t offset, fuse_file_info*);
		
		static int write(const char* path, const char* buf, size_t size, off_t offset, fuse_file_info*);
		
		static int statfs(const char* path, struct statvfs*);
		
		static int flush(const char* path, fuse_file_info*);
		
		static int release(const char* path, fuse_file_info*);
		
		static int fsync(const char* path, int data_sync, fuse_file_info*);
		
		static int opendir(const char* path, fuse_file_info*);
		
		
		static int readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, fuse_file_info*);
		
		
		static int releasedir(const char* path, fuse_file_info*);
		
		static int fsyncdir(const char* path, int data_sync, fuse_file_info*);
		
		static void* init(fuse_conn_info*);
		
		static void destroy(void* user_data);
		
		static int access(const char* path, int mask);
		
		static int ftruncate(const char* path, off_t offset, fuse_file_info*);
		
		static int fgetattr(const char* path, struct stat* statbuf, fuse_file_info*);
		
		static int lock(const char* path, fuse_file_info*, int cmd, struct flock*);

		static int ioctl(const char* path, int cmd, void* arg, struct fuse_file_info*, unsigned int flags, void* data);

		static int poll(const char* path, fuse_file_info*, struct fuse_pollhandle*, unsigned*);

		static int write_buf(const char* path, struct fuse_bufvec* buf, off_t off, fuse_file_info*);

		static int read_buf(const char* path, struct fuse_bufvec** bufp, size_t size, off_t off, fuse_file_info*);
		
		static int flock(const char* path, struct fuse_file_info*, int op);

		static int fallocate(const char* path, int, off_t, off_t, struct fuse_file_info*);
	};
}

#include "Operations.hpp"

#endif