import sys
import gdb

# Update module path.
dir_ = '/mnt/hgfs/SrcRepos/gt6-RAMSES_8_5/xio/drivers/udt/source/glib2/share/glib-2.0/gdb'
if not dir_ in sys.path:
    sys.path.insert(0, dir_)

from gobject import register
register (gdb.current_objfile ())
