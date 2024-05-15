/*

2022-12-30 / im dong ye

*/

#ifndef __asset_lib_h_
#define __asset_lib_h_

#include <glad/glad.h>

namespace lim
{
	class AppBase;

	class AssetLib
	{
	private:
		AssetLib(const AssetLib&)=delete;
		AssetLib& operator=(const AssetLib&)=delete;
		AssetLib(AssetLib&&)=delete;
		AssetLib& operator=(AssetLib&&)=delete;
		inline static AssetLib* instance = nullptr;
		AssetLib();
		~AssetLib();

	public:
		//****** property ******//
		AppBase* app;

	public:
		static void create(AppBase* app);
		static void destroy();
		static AssetLib& get();
	};
}

#endif
