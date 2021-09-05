#include "Engine.h"

namespace MGE {

static Bitmap* asset_location[10]; // Maximum 10 assets

void Engine::LoadAsset(Memory vol, int id, Memory destination)
{
    //Check that we haven't already loaded
    if (asset_location[id] != nullptr) {return;}

    char path[128];
    Util::MoveString(path, "Assets\\x.bmp");
    path[7] = id + 48;
    
    bool success = Engine::OpenBitmap(vol.Addr, vol.Size, (Bitmap*)destination.Addr, destination.Size, path);
    if (!success) {return;}
    
    asset_location[id] = (Bitmap*)destination.Addr;
}

Bitmap* Engine::GetAsset(int id)
{
    return asset_location[id];
}

}