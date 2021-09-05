#include "Engine.h"
#include "struct/Environment.h"

namespace MGE {

void Engine::LoadAsset(Memory vol, Environment* env, int id, Memory destination)
{
    //Check that we haven't already loaded
    if (env->AssetAddress[id] != nullptr) {return;}

    char path[128];
    Util::MoveString(path, "Assets\\x.bmp");
    path[7] = id + 48;
    
    bool success = Engine::OpenBitmap(vol.Addr, vol.Size, (Bitmap*)destination.Addr, destination.Size, path);
    if (!success) {return;}
    
    env->AssetAddress[id] = (Bitmap*)destination.Addr;
}

Bitmap* Engine::GetAsset(Environment* env, int id)
{
    return env->AssetAddress[id];
}

}