#include <limbrary/asset_lib.h>


lim::AssetLib::AssetLib()
{
}
lim::AssetLib::~AssetLib()
{
}
void lim::AssetLib::create(AppBase* _app)
{
	if( !instance )
		instance = new AssetLib();
	instance->app = _app;
}
lim::AssetLib& lim::AssetLib::get()
{
	return *instance;
}
void lim::AssetLib::destroy()
{
	if( instance )
		delete instance;
	instance = nullptr;
}