import os

output = ""

## General ##
output += "-OUT:build\\main.exe" + "\n" # Location of executable (.exe)

## Debug ##
if (True):
    output += "-DEBUG" + "\n"
    output += "-PDB:build\\main.pdb" + "\n" # Location of .pdb

## Libraries ##
output += "user32.lib" + "\n" # Multiple OS services from <window.h>
output += "gdi32.lib" + "\n" # StretchDIBits() from <windows.h>
output += "winmm.lib" + "\n" # timeBeginPeriod() from <windows.h>
output += "C:\\VulkanSDK\\1.2.162.1\\Lib\\vulkan-1.lib" + "\n"

## Input files ##
for root, dirs, files in os.walk("build\\obj"):
    for file in files:
        if (file[-4:] != ".obj"): continue
        output += os.path.join(root, file) + "\n"

file = open("BuildTools\\link_command.txt", "w")
file.write(output)
file.close()