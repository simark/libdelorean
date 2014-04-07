# master SConstruct for libdelorean
#
# author: Philippe Proulx <eepp.ca>


import os


# C++ flags
ccflags = [
    '-std=c++11',
    '-Wall',
    '-g',
]

# this is to allow colorgcc
custom_env = {
    'PATH': os.environ['PATH'],
    'TERM': os.environ['TERM'],
    'HOME': os.environ['HOME'],
}

root_env = Environment(CCFLAGS=ccflags, ENV=custom_env)
root_env.Append(CPPPATH=['#/include'])

subdirs = [
    'src',
    'tests',
]

for basedir in subdirs:
    sconscript = os.path.join(basedir, 'SConscript')
    SConscript(sconscript, exports=['root_env'])
