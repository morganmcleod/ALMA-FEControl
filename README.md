# ALMA-FEControl
C++ software for monitor and control of the ALMA Front End and related hardware

Uses 3rd party libraries not included in this repository:<br>
Pthreads-win32: https://www.sourceware.org/pthreads-win32/<br>
MySQL Connector/C: https://dev.mysql.com/downloads/connector/c/<br>
NI-CAN driver: http://sine.ni.com/nips/cds/view/p/lang/en/nid/206955<br>
CPPUnit: https://sourceforge.net/projects/cppunit/


$ ls -R 3rdparty/

3rdparty/:
MySQL41/
Nican/

3rdparty/MySQL41:
include/
lib/

3rdparty/MySQL41/include:
config-netware.h
config-os2.h
config-win.h
errmsg.h
libmysql.def
libmysqld.def
m_ctype.h
m_string.h
my_alloc.h
my_dbug.h
my_getopt.h
my_global.h
my_list.h
my_pthread.h
my_sys.h
mysql.h
mysql_com.h
mysql_embed.h
mysql_time.h
mysql_version.h
mysqld_error.h
raid.h
typelib.h

3rdparty/MySQL41/lib:
debug/
opt/

3rdparty/MySQL41/lib/debug:
liblibmysql.a
LIBMYSQL.def
libmysql.dll*
libmysql.lib
mysqlclient.lib
mysys.lib
regex.lib
strings.lib
zlib.lib

3rdparty/MySQL41/lib/opt:
libmysql.dll*
libmysql.lib
mysqlclient.lib
mysys.lib
mysys_tls.lib
mysys-max.lib
regex.lib
strings.lib
zlib.lib

3rdparty/Nican:
include/
lib/

3rdparty/Nican/include:
Nican.h

3rdparty/Nican/lib:
libnicanmsc.a
nicanmsc.lib
README.txt
