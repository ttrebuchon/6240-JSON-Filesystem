#ifndef INCLUDED_RAMFS_RAM_FILESYSTEM_JSON_INFO_HH
#define INCLUDED_RAMFS_RAM_FILESYSTEM_JSON_INFO_HH

#include <RamFS/common.hh>
#include <nlohmann/json.hpp>

namespace RamFS
{
	class json_info
	{
		public:
		typedef nlohmann::json_pointer<nlohmann::json> jpointer_type;
		
		jpointer_type jpointer;
		nlohmann::json* root_doc;
		nlohmann::json json;


		json_info(nlohmann::json* root_json, jpointer_type ptr) :
			jpointer(ptr),
			root_doc(root_json),
			json()
		{
			json = (*root_doc)[jpointer];
		}


		void merge_json()
		{
			(*root_doc)[jpointer].merge_patch(json);
		}

		const nlohmann::json& get_source_json() const
		{
			return (*root_doc)[jpointer];
		}
	};
}

#endif