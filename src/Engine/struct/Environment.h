#pragma once

namespace MGE {

struct Environment
{
    Bitmap* AssetAddress[10]; // Maximum 10 assets
    byte AssetMemory[10][10 * MEGABYTES];
};

}