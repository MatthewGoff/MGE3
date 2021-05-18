#include "Engine.h"

static Bitmap* asset_location[10]; // Maximum 10 assets

void Engine::LoadAsset(int id)
{
    //Check that we haven't already loaded
    if (asset_location[id] != nullptr) {return;}

    char path[128];
    Util::MoveString(path, "Assets\\x.bmp");
    path[7] = id + 48;
    
    Bitmap* bitmap = (Bitmap*)malloc(10 * MEGABYTES);
    
    byte* mem = (byte*)malloc(10 * MEGABYTES);
    bool success = Engine::OpenBitmap(mem, 10 * MEGABYTES, bitmap, 10 * MEGABYTES, path);
    free(mem);
    
    if (!success)
    {
        return;
    }
    
    asset_location[id] = bitmap;
}

Bitmap* Engine::GetAsset(int id)
{
    return asset_location[id];
}