'''
config.py contains volatile build configuration and is ignored by git

config_template.py is included in the repo so you can create and rename a copy
'''

def vulkan_sdk(): return "C:\\VulkanSDK\\1.2.162.1"

def debug(): return True

'''
A list of directories and files which should be compiled. Recursive; Only considers (.cpp); Duplicates discarded.
'''
def source():

    source = []
    source += ["src"]
    #source += ["src\\WindowsOS\\Rendering"]
    
    return source

def ignore_warnings():
    warnings = ""
    #warnings += " -wd4533" # 4533 initialization of <var> is skipped by <goto>
    
    return warnings