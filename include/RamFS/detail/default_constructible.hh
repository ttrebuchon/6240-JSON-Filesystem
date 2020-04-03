#ifndef INCLUDED_RAMFS_DETAIL_DEFAULT_CONSTRUCTIBLE_HH
#define INCLUDED_RAMFS_DETAIL_DEFAULT_CONSTRUCTIBLE_HH

#include <RamFS/common.hh>
#include <type_traits>



namespace RamFS
{
namespace detail
{
	template <class T, bool = std::is_default_constructible<T>::value && !std::is_abstract<T>::value>
	struct try_default_construct;
	
	template <class T>
	struct try_default_construct<T, false> : std::false_type
	{
		static T* try_construct()
		{
			return nullptr;
		}
	};
	
	template <class T>
	struct try_default_construct<T, true> : std::true_type
	{
		static T* try_construct()
		{
			return new T();
		}
	};
}
}

#endif