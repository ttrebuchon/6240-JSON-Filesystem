
#include <RamFS/common.hh>
#include <RamFS/RamFilesystem.hh>
#include <vector>
#include <RamFS/Operations.hh>
#include <iostream>
#include <nlohmann/json.hpp>



static struct FSOptions
{
	char* json_path = nullptr;
} opts;


static int opt_proc(void* data, const char* arg, int key, struct fuse_args* outargs)
{
	return 1;
}



static struct fuse_opt eligible_opts[] = 
{
	{ "--json=%s", offsetof(FSOptions, json_path), 0 },
	FUSE_OPT_END
};

int main(int argc, char* argv[])
{
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    fuse_opt_parse(&args, &opts, eligible_opts, opt_proc);

	if (!opts.json_path)
	{
		std::cout << 
			"Usage: " << argv[0]
			<< " [FUSE Options | --json=JSON_PATH] [MOUNT_PATH]"
			<< std::endl;
		return 0;
	}


	std::ifstream json_in(opts.json_path);
	if (!json_in.good())
	{
		std::cerr << "Couldn't open JSON file " << opts.json_path << std::endl;
		return -1;
	}
	else
	{
		json_in.close();
	}

	int rc;


	RamFS::FUSEOperations<RamFS::RamFilesystem> fuse;
	fuse.fs = new RamFS::RamFilesystem(opts.json_path);
	rc = fuse.main(args.argc, args.argv);
	
	return rc;
}