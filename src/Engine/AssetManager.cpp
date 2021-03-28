#include "Engine.h"

static Bitmap* asset_location[10]; // Maximum 10 assets

void Engine::LoadAsset(int id)
{
    char path[6];
    path[0] = id + 48;
    path[1] = '.';
    path[2] = 'b';
    path[3] = 'm';
    path[4] = 'p';
    path[5] = 0;
    
    Bitmap* bitmap = (Bitmap*)malloc(10 * MEGABYTES);
    
    byte* mem = (byte*)malloc(10 * MEGABYTES);
    bool success = Engine::OpenBitmap(mem, 10 * MEGABYTES, bitmap, 10 * MEGABYTES, path);
    free(mem);
    
    if (success == 0)
    {
        return;
    }
    
    asset_location[id] = bitmap;
}

Bitmap* Engine::GetAsset(int id)
{
    return asset_location[id];
}