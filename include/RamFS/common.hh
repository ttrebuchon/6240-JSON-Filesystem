#ifndef INCLUDED_RAMFS_COMMON_HH
#define INCLUDED_RAMFS_COMMON_HH

#include <cstdint>
#ifndef FUSE_SIMULATION
        #ifdef FUSE_USE_VERSION
                #undef FUSE_USE_VERSION
        #endif
        #define FUSE_USE_VERSION 26
        #include <fuse.h>
#endif

#include <fstream>
#include <iostream>

#define NOT_IMPLEMENTED \
std::cerr << __func__ << "\n\n" << "NOT IMPLEMENTED!" << std::endl;



namespace RamFS
{
        typedef uint64_t node_id_t;
}


extern "C" {
	
	struct utimbuf;
	struct fuse_file_info;
	struct statvfs;
	struct fuse_conn_info;
	struct stat;
	struct fuse_operations;
}


#ifdef __GNUC__
	#define __gcc_attribute__(...) __attribute__(__VA_ARGS__)
#else
	#define __gcc_attribute__(...) 
#endif

#define ALWAYS_INLINE __gcc_attribute__((__always_inline__))




#ifdef FUSE_SIMULATION
extern "C" {
	
	
struct fuse;
 
 
 
 enum fuse_fill_dir_flags {
         FUSE_FILL_DIR_PLUS = (1 << 1),
 };
 
 typedef int (*fuse_fill_dir_t) (void *buf, const char *name,
                                 const struct stat *stbuf, off_t off,
                                 enum fuse_fill_dir_flags flags);
	
	
	
struct fuse_file_info
{
	int flags;
	unsigned int writepage : 1;
	unsigned int direct_io : 1;
	unsigned int keep_cache : 1;
	unsigned int flush : 1;
	unsigned int nonseekable : 1;
	unsigned int padding : 27;
	uint64_t fh;
	uint64_t lock_owner;
	uint64_t poll_events;
};

struct fuse_context
{
	struct fuse* fuse;
	uid_t uid;
	gid_t gid;
	pid_t pid;
	void* private_data;
	mode_t umask;
};

struct fuse_conn_info {
         unsigned proto_major;
 
         unsigned proto_minor;
 
         unsigned max_write;
 
         unsigned max_read;
 
         unsigned max_readahead;
 
         unsigned capable;
 
         unsigned want;
 
         unsigned max_background;
 
         unsigned congestion_threshold;
 
         unsigned time_gran;
 
         unsigned reserved[22];
};



struct fuse_operations
{
         int (*getattr) (const char *, struct stat *, struct fuse_file_info *fi);
 
         int (*readlink) (const char *, char *, size_t);
 
         int (*mknod) (const char *, mode_t, dev_t);
 
         int (*mkdir) (const char *, mode_t);
 
         int (*unlink) (const char *);
 
         int (*rmdir) (const char *);
 
         int (*symlink) (const char *, const char *);
 
         int (*rename) (const char *, const char *, unsigned int flags);
 
         int (*link) (const char *, const char *);
 
         int (*chmod) (const char *, mode_t, struct fuse_file_info *fi);
 
         int (*chown) (const char *, uid_t, gid_t, struct fuse_file_info *fi);
 
         int (*truncate) (const char *, off_t, struct fuse_file_info *fi);
 
         int (*open) (const char *, struct fuse_file_info *);
 
         int (*read) (const char *, char *, size_t, off_t,
                      struct fuse_file_info *);
 
         int (*write) (const char *, const char *, size_t, off_t,
                       struct fuse_file_info *);
 
         int (*statfs) (const char *, struct statvfs *);
 
         int (*flush) (const char *, struct fuse_file_info *);
 
         int (*release) (const char *, struct fuse_file_info *);
 
         int (*fsync) (const char *, int, struct fuse_file_info *);
 
         int (*setxattr) (const char *, const char *, const char *, size_t, int);
 
         int (*getxattr) (const char *, const char *, char *, size_t);
 
         int (*listxattr) (const char *, char *, size_t);
 
         int (*removexattr) (const char *, const char *);
 
         int (*opendir) (const char *, struct fuse_file_info *);
 
         int (*readdir) (const char *, void *, fuse_fill_dir_t, off_t,
                         struct fuse_file_info *);
 
         int (*releasedir) (const char *, struct fuse_file_info *);
 
         int (*fsyncdir) (const char *, int, struct fuse_file_info *);
 
         void *(*init) (struct fuse_conn_info *conn,
                        struct fuse_config *cfg);
 
         void (*destroy) (void *private_data);
 
         int (*access) (const char *, int);
 
         int (*create) (const char *, mode_t, struct fuse_file_info *);
 
         int (*lock) (const char *, struct fuse_file_info *, int cmd,
                      struct flock *);
 
          int (*utimens) (const char *, const struct timespec tv[2],
                          struct fuse_file_info *fi);
 
         int (*bmap) (const char *, size_t blocksize, uint64_t *idx);
 
         int (*ioctl) (const char *, int cmd, void *arg,
                       struct fuse_file_info *, unsigned int flags, void *data);
 
         int (*poll) (const char *, struct fuse_file_info *,
                      struct fuse_pollhandle *ph, unsigned *reventsp);
 
         int (*write_buf) (const char *, struct fuse_bufvec *buf, off_t off,
                           struct fuse_file_info *);
 
         int (*read_buf) (const char *, struct fuse_bufvec **bufp,
                          size_t size, off_t off, struct fuse_file_info *);
         int (*flock) (const char *, struct fuse_file_info *, int op);
 
         int (*fallocate) (const char *, int, off_t, off_t,
                           struct fuse_file_info *);
 
         ssize_t (*copy_file_range) (const char *path_in,
                                     struct fuse_file_info *fi_in,
                                     off_t offset_in, const char *path_out,
                                     struct fuse_file_info *fi_out,
                                     off_t offset_out, size_t size, int flags);
};

struct fuse_config {
         int set_gid;
         unsigned int gid;
 
         int set_uid;
         unsigned int uid;
 
         int set_mode;
         unsigned int umask;
 
         double entry_timeout;
 
         double negative_timeout;
 
         double attr_timeout;
 
         int intr;
 
         int intr_signal;
 
         int remember;
 
         int hard_remove;
 
         int use_ino;
 
         int readdir_ino;
 
         int direct_io;
 
         int kernel_cache;
 
         int auto_cache;
 
         int ac_attr_timeout_set;
         double ac_attr_timeout;
 
         int nullpath_ok;
 
         int show_help;
         char *modules;
         int debug;
 };



struct fuse_context *fuse_get_context();
int fuse_main(int argc, char** argv, struct fuse_operations*, void*);



int fu_open(const char* path, int flags);
int fu_close(int fd);
ssize_t fu_write(int fd, const void* buf, size_t count);
ssize_t fu_read(int fd, void* buf, size_t count);
ssize_t fu_seek_set(int fd, off_t off);

}
#endif


#endif