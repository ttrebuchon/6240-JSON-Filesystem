#ifndef INCLUDED_RAMFS_OPERATIONS_HPP
#define INCLUDED_RAMFS_OPERATIONS_HPP

#include "Operations.hh"
#include <cassert>



namespace RamFS
{
	
	template <class FS>
	FUSEOperations<FS>* FUSEOperations<FS>::get_ops_object()
	{
		auto ctx = fuse_get_context();
		if (ctx)
		{
			return (FUSEOperations*)ctx->private_data;
		}
		else
		{
			NOT_IMPLEMENTED
			return nullptr;
		}
	}
	
	
	
	
	
	template <class FS>
	int FUSEOperations<FS>::filesystem_main(int argc, char** argv, FS& imp)
	{
		auto ops = new FUSEOperations();
		auto fuse_ops = ops->fuse_ops = new fuse_operations();
		memset(fuse_ops, 0, sizeof(fuse_operations));
		
		ops->fs = &imp;
		ops->owns_fs = false;
		
		FUSEOperations::set_fuse_operations(fuse_ops);
		
		int result = fuse_main(argc, argv, fuse_ops, ops);
		
		delete fuse_ops;
		ops->fuse_ops = nullptr;
		delete ops;
		
		return result;
		
	}
	
	template <class FS>
	int FUSEOperations<FS>::main(int argc, char** argv)
	{
		if (!fuse_ops)
		{
			fuse_ops = new fuse_operations();
			memset(fuse_ops, 0, sizeof(fuse_operations));
			
			FUSEOperations::set_fuse_operations(fuse_ops);
		}
		
		int result = fuse_main(argc, argv, fuse_ops, this);
		
		return result;
	}
	
	
	
	
	
	
	
	template <class FS>
	void FUSEOperations<FS>::set_fuse_operations(fuse_operations* ops)
	{
		
		#define SET_OP(NAME) \
		ops->NAME = &FUSEOperations::NAME;
		
		SET_OP(getattr);
		SET_OP(readlink);
		SET_OP(mknod);
		SET_OP(mkdir);
		SET_OP(unlink);
		SET_OP(rmdir);
		SET_OP(symlink);
		SET_OP(rename);
		SET_OP(link);
		SET_OP(chmod);
		SET_OP(chown);
		SET_OP(truncate);
		SET_OP(utimens);
		SET_OP(open);
		SET_OP(read);
		SET_OP(write);
		SET_OP(statfs);
		SET_OP(flush);
		SET_OP(release);
		SET_OP(fsync);
		SET_OP(opendir);
		SET_OP(readdir);
		SET_OP(releasedir);
		SET_OP(fsyncdir);
		SET_OP(init);
		SET_OP(destroy);

		// SET_OP(access);
		// SET_OP(ftruncate);
		// SET_OP(fgetattr);
		// SET_OP(lock);
		// SET_OP(ioctl);
		// SET_OP(poll);
		// SET_OP(write_buf);
		// SET_OP(read_buf);
		// SET_OP(flock);
		// SET_OP(fallocate);
		
		#undef SET_OP
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	template <class FS>
	int FUSEOperations<FS>::getattr(const char* path, struct stat* statbuf)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->getattr(path, statbuf);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	template <class FS>
	int FUSEOperations<FS>::readlink(const char* path, char* link, size_t size)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->readlink(path, link, size);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	template <class FS>
	int FUSEOperations<FS>::mknod(const char* path, mode_t mode, dev_t dev)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->mknod(path, mode, dev);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	template <class FS>
	int FUSEOperations<FS>::mkdir(const char* path, mode_t mode)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->mkdir(path, mode);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	template <class FS>
	int FUSEOperations<FS>::unlink(const char* path)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->unlink(path);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	template <class FS>
	int FUSEOperations<FS>::rmdir(const char* path)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->rmdir(path);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	template <class FS>
	int FUSEOperations<FS>::symlink(const char* path, const char* link)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->symlink(path, link);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	template <class FS>
	int FUSEOperations<FS>::rename(const char* path, const char* new_path)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->rename(path, new_path);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	template <class FS>
	int FUSEOperations<FS>::link(const char* path, const char* new_path)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->link(path, new_path);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	template <class FS>
	int FUSEOperations<FS>::chmod(const char* path, mode_t mode)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->chmod(path, mode);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	template <class FS>
	int FUSEOperations<FS>::chown(const char* path, uid_t uid, gid_t gid)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->chown(path, uid, gid);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	template <class FS>
	int FUSEOperations<FS>::truncate(const char* path, off_t new_size)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->truncate(path, new_size);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	template <class FS>
	int FUSEOperations<FS>::utimens(const char* path, const struct timespec tv[2])
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->utimens(path, tv);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	template <class FS>
	int FUSEOperations<FS>::open(const char* path, fuse_file_info* info)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->open(path, info);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	template <class FS>
	int FUSEOperations<FS>::read(const char* path, char* buf, size_t size, off_t offset, fuse_file_info* info)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->read(path, buf, size, offset, info);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	template <class FS>
	int FUSEOperations<FS>::write(const char* path, const char* buf, size_t size, off_t offset, fuse_file_info* fi)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->write(path, buf, size, offset, fi);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	template <class FS>
	int FUSEOperations<FS>::statfs(const char* path, struct statvfs* vfs)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->statfs(path, vfs);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	template <class FS>
	int FUSEOperations<FS>::flush(const char* path, fuse_file_info* fi)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->flush(path, fi);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	template <class FS>
	int FUSEOperations<FS>::release(const char* path, fuse_file_info* fi)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->release(path, fi);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	template <class FS>
	int FUSEOperations<FS>::fsync(const char* path, int data_sync, fuse_file_info* fi)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->fsync(path, data_sync, fi);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
	
	
	template <class FS>
	int FUSEOperations<FS>::opendir(const char* path, fuse_file_info* fi)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->opendir(path, fi);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	template <class FS>
	int FUSEOperations<FS>::readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, fuse_file_info* fi)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->readdir(path, buf, filler, offset, fi);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	template <class FS>
	int FUSEOperations<FS>::releasedir(const char* path, fuse_file_info* fi)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->releasedir(path, fi);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	template <class FS>
	int FUSEOperations<FS>::fsyncdir(const char* path, int data_sync, fuse_file_info* fi)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->fsyncdir(path, data_sync, fi);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
	
	template <class FS>
	void* FUSEOperations<FS>::init(fuse_conn_info*)
	{
		FUSEOperations* ops = nullptr;
		auto ctx = fuse_get_context();
		assert(ctx);
		if (ctx)
		{
			ops = (FUSEOperations*)ctx->private_data;
		}
		
		if (!ops)
		{
			ops = new FUSEOperations();
		}
		
		if (!ops->fs)
		{
			ops->fs = detail::try_default_construct<FS>::try_construct();
			if (!ops->fs)
			{
				// Total Failure
				fprintf(stderr, "No filesystem implementation instance provided.\n");
				exit(EXIT_FAILURE);
			}
			else
			{
				ops->owns_fs = true;
			}
		}
		
		return ops;
	}
		
	template <class FS>
	void FUSEOperations<FS>::destroy(void* user_data)
	{
		/*FUSEOperations* ops = (FUSEOperations*)user_data;
		if (ops)
		{
			if (ops->fs)
			{
				delete ops->fs;
				ops->fs = nullptr;
			}
		}*/
	}
		
	
	template <class FS>
	int FUSEOperations<FS>::access(const char* path, int mask)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->access(path, mask);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	template <class FS>
	int FUSEOperations<FS>::ftruncate(const char* path, off_t offset, fuse_file_info* fi)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->ftruncate(path, offset, fi);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}
		
	template <class FS>
	int FUSEOperations<FS>::fgetattr(const char* path, struct stat* statbuf, fuse_file_info* fi)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->fgetattr(path, statbuf, fi);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}

	template <class FS>
	int FUSEOperations<FS>::lock(const char* path, fuse_file_info* fi, int cmd, struct flock* l)
	{
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->lock(path, fi, cmd, l);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
	}

	template <class FS>
	int FUSEOperations<FS>::ioctl(const char* path, int cmd, void* arg, struct fuse_file_info* fi, unsigned int flags, void* data)
	{
		NOT_IMPLEMENTED
		assert(false);
		return -1;
		/*
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->ioctl(path, cmd, arg, fi, flags, data);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
		*/
	}

	template <class FS>
	int FUSEOperations<FS>::poll(const char* path, fuse_file_info* fi, struct fuse_pollhandle* hndl, unsigned* us)
	{
		NOT_IMPLEMENTED
		assert(false);
		return -1;
		/*
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->poll(path, fi, hndl, us);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
		*/
	}

	template <class FS>
	int FUSEOperations<FS>::write_buf(const char* path, struct fuse_bufvec* buf, off_t off, fuse_file_info* fi)
	{
		NOT_IMPLEMENTED
		assert(false);
		return -1;
		/*
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->write_buf(path, buf, off, fi);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
		*/
	}

	template <class FS>
	int FUSEOperations<FS>::read_buf(const char* path, struct fuse_bufvec** bufp, size_t size, off_t off, fuse_file_info* fi)
	{
		NOT_IMPLEMENTED
		assert(false);
		return -1;
		/*
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->read_buf(path, bufp, size, off, fi);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
		*/
	}
	
	template <class FS>
	int FUSEOperations<FS>::flock(const char* path, fuse_file_info* fi, int op)
	{
		NOT_IMPLEMENTED
		assert(false);
		return -1;
		/*
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->flock(path, fi, op);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
		*/
	}

	template <class FS>
	int FUSEOperations<FS>::fallocate(const char* path, int mode, off_t offset, off_t len, fuse_file_info* fi)
	{
		NOT_IMPLEMENTED
		assert(false);
		return -1;
		/*
		FUSEOperations* ops;
		if ((ops = get_ops_object()))
		{
			return ops->fs->fallocate(path, mode, offset, len, fi);
		}
		else
		{
			// TODO?
			NOT_IMPLEMENTED
			assert(false);
			return -1;
		}
		*/
	}
}

#endif