# master SConstruct for libdelorean
#
# author: Philippe Proulx <eepp.ca>


import os.path


root_env = Environment()
root_env.Append(CPPPATH=['#/include'])

subdirs = [
    'src',
]

for basedir in subdirs:
    sconscript = os.path.join(basedir, 'SConscript')
    SConscript(sconscript, exports=['root_env'])
