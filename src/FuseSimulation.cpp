#ifdef FUSE_SIMULATION

#include <RamFS/common.hh>
#include <cstring>
#include <cassert>
#include <errno.h>
#include <sys/stat.h>
#include <map>
#include <fcntl.h>
#include <libgen.h>
#include <cstdlib>


static struct fuse_context context;
struct fuse_operations* current_ops = nullptr;
static bool context_exists = false;

struct fuse_context *fuse_get_context()
{
	if (context_exists)
	{
		return &context;
	}
	else
	{
		return nullptr;
	}
}

int fuse_main(int argc, char** argv, struct fuse_operations* ops, void* usr_data)
{
	assert(ops);
	current_ops = ops;
	context_exists = true;
	context.private_data = usr_data;
	
	struct fuse_conn_info conn;
	memset(&conn, 0, sizeof(conn));
	
	struct fuse_config cfg;
	memset(&cfg, 0, sizeof(cfg));
	
	assert(ops->init);
	//(*ops->init)(&conn, &cfg);
	context.private_data = ops->init(&conn, &cfg);
	
	
	
	// TODO: Run some tests
	int rc;
	
	
	rc = ops->mkdir("d", 0);
	assert(rc == 0);
	rc = ops->mkdir("d/a", 0);
	assert(rc == 0);
	rc = ops->mkdir("d/a/b", 0);
	assert(rc != ENOENT);
	assert(rc == 0);
	
	
	rc = ops->mknod("d/a/b/a.txt", S_IFREG, 0);
	assert(rc == 0);
	
	fuse_file_info fi1;
	memset(&fi1, 0, sizeof(fi1));
	
	rc = ops->open("d/a/b/a.txt", &fi1);
	assert(rc == 0);
	
	int fd = fu_open("d/a/b/a.txt", O_RDONLY);
	if (fd < 0)
	{
		perror("Couldn't open a.txt");
	}
	assert(fd >= 0);
	
	auto writ = fu_write(fd, "Hello.\n", 7);
	assert(writ == 7);
	writ = fu_write(fd, "World.\n", 7);
	assert(writ == 7);
	
	char buf[15] = {0};
	fu_seek_set(fd, 0);
	fu_read(fd, buf, 15);
	buf[14] = '\0';
	assert(strcmp(buf, "Hello.\nWorld.\n") == 0);
	
	fu_close(fd);
	
	
	
	
	
	ops->destroy(context.private_data);
	
	
	
	
	return 0;
}








struct FileTracker
{
	struct fuse_file_info fi;
	off_t offset;
	char path[PATH_MAX];
};



static std::map<int, FileTracker> fi_s;

static int _fu_open(const char* path, int flags)
{
	int fd = (int)fi_s.size() + 1;
	auto file = &fi_s[fd];
	memset(file, 0, sizeof(FileTracker));
	file->fi.flags = flags;
	strncpy(file->path, path, PATH_MAX);
	int rc = current_ops->open(path, &file->fi);
	if (rc < 0)
	{
		return rc;
	}
	else
	{
		return fd;
	}
}

int fu_open(const char* path, int flags)
{
	int rc = _fu_open(path, flags);
	if (rc >= 0)
	{
		return rc;
	}
	
	if ((rc == ENOENT) && ((flags & O_CREAT) != 0))
	{
		rc = current_ops->mknod(path, S_IFREG, 0);
		if (rc < 0)
		{
			errno = rc;
			return -1;
		}
		
		return fu_open(path, flags);
		
	}
	
	errno = rc;
	return -1;
}


int fu_close(int fd)
{
	auto file = &fi_s.at(fd);
	int rc = current_ops->release(file->path, &file->fi);
	return rc;
}

ssize_t fu_write(int fd, const void* buf, size_t count)
{
	auto file = &fi_s.at(fd);
	int rc = current_ops->write(file->path, reinterpret_cast<const char*>(buf), count, file->offset, &file->fi);
	if (rc > 0)
	{
		file->offset += rc;
		return rc;
	}
	else
	{
		errno = rc;
		return -1;
	}
}

ssize_t fu_read(int fd, void* buf, size_t count)
{
	auto file = &fi_s.at(fd);
	int rc = current_ops->read(file->path, reinterpret_cast<char*>(buf), count, file->offset, &file->fi);
	if (rc > 0)
	{
		file->offset += rc;
		return rc;
	}
	else
	{
		errno = rc;
		return -1;
	}
}

ssize_t fu_seek_set(int fd, off_t off)
{
	auto file = &fi_s.at(fd);
	file->offset = off;
	return off;
}



#endif