from building import *
import rtconfig

cwd     = GetCurrentDir()
src     = Glob('*.c')
CPPPATH = [cwd]
LOCAL_CCFLAGS = ''

src += Glob('port/of_mtd_rtt.c')
if GetDepend(['OF_USING_SFUD_PORT']):
    src += Glob('port/of_flash_sfud_port.c')

group = DefineGroup('OF-mtd', src, depend = [''], CPPPATH = CPPPATH, LOCAL_CCFLAGS = LOCAL_CCFLAGS)

Return('group')