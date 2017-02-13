# ALMA-FEControl
C++ software for monitor and control of the ALMA Front End and related hardware

Uses 3rd party libraries not included in this repository:<br>
Pthreads-win32: https://www.sourceware.org/pthreads-win32/<br>
MySQL Connector/C: https://dev.mysql.com/downloads/connector/c/<br>
NI-CAN driver: http://sine.ni.com/nips/cds/view/p/lang/en/nid/206955<br>
CPPUnit: https://sourceforge.net/projects/cppunit/


$ ls -R 3rdparty/

3rdparty/:
include/
lib/

3rdparty/include:
MySQL41/
Nican/
cppunit/
pthreads_win32/

3rdparty/include/MySQL41:
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

3rdparty/include/Nican:
Nican.h

3rdparty/include/cppunit/cppunit:
...

3rdparty/include/pthreads_win32:
md5.sum
pthread.h
sched.h
semaphore.h

3rdparty/lib:
MySQL41/
Nican/
cppunit/
pthreads_win32/

3rdparty/lib/MySQL41:
debug/
opt/

3rdparty/lib/MySQL41/debug:
LIBMYSQL.def
liblibmysql.a
libmysql.dll*
libmysql.lib
mysqlclient.lib
mysys.lib
regex.lib
strings.lib
zlib.lib

3rdparty/lib/MySQL41/opt:
libmysql.dll*
libmysql.lib
mysqlclient.lib
mysys-max.lib
mysys.lib
mysys_tls.lib
regex.lib
strings.lib
zlib.lib

3rdparty/lib/Nican:
README.txt
libnicanmsc.a
nicanmsc.lib

3rdparty/lib/cppunit:
Debug/
Release/

3rdparty/lib/cppunit/Debug:
...

3rdparty/lib/cppunit/Release:

3rdparty/lib/pthreads_win32:
libpthreadGC2.a
md5.sum
pthreadGC2.dll*
