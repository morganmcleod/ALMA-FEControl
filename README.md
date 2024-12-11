# ALMA-FEControl
C++ software for monitor and control of the ALMA Front End and related hardware

## This library must be built using 32-bits tools because the NI-CAN driver does not support 64-bits

I recommend using the tools available in MinGW W32. In this case simply run the `install_dep.sh` script.

## Third party libraries

Uses 3rd party libraries not included in this repository:
- MySQL Connector/C: https://dev.mysql.com/downloads/connector/c/
- NI-CAN driver: http://sine.ni.com/nips/cds/view/p/lang/en/nid/206955

3rdparty folder structure
```
3rdparty
├── MySQL41
│   ├── include
│   │   ├── config-netware.h
│   │   ├── config-os2.h
│   │   ├── config-win.h
│   │   ├── errmsg.h
│   │   ├── libmysql.def
│   │   ├── libmysqld.def
│   │   ├── m_ctype.h
│   │   ├── m_string.h
│   │   ├── my_alloc.h
│   │   ├── my_dbug.h
│   │   ├── my_getopt.h
│   │   ├── my_global.h
│   │   ├── my_list.h
│   │   ├── my_pthread.h
│   │   ├── my_sys.h
│   │   ├── mysql.h
│   │   ├── mysql_com.h
│   │   ├── mysql_embed.h
│   │   ├── mysql_time.h
│   │   ├── mysql_version.h
│   │   ├── mysqld_error.h
│   │   ├── raid.h
│   │   └── typelib.h
│   └── lib
│       ├── debug
│       │   ├── liblibmysql.a
│       │   ├── LIBMYSQL.def
│       │   ├── libmysql.dll
│       │   ├── libmysql.lib
│       │   ├── mysqlclient.lib
│       │   ├── mysys.lib
│       │   ├── regex.lib
│       │   ├── strings.lib
│       │   └── zlib.lib
│       └── opt
│           ├── libmysql.dll
│           ├── libmysql.lib
│           ├── mysqlclient.lib
│           ├── mysys.lib
│           ├── mysys_tls.lib
│           ├── mysys-max.lib
│           ├── regex.lib
│           ├── strings.lib
│           └── zlib.lib
└── Nican
    ├── include
    │   └── Nican.h
    └── lib
        ├── nicanmsc.lib
        └── nicanmsclib.a
```