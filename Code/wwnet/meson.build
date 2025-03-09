wwnet = static_library(
    'wwnet',
    'BWBalance.cpp',
    'connect.cpp',
    'lan.cpp',
    'msgstat.cpp',
    'msgstatlist.cpp',
    'msgstatlistgroup.cpp',
    'netstats.cpp',
    'netutil.cpp',
    'networkobject.cpp',
    'networkobjectfactory.cpp',
    'networkobjectfactorymgr.cpp',
    'networkobjectmgr.cpp',
    'packetmgr.cpp',
    'rhost.cpp',
    'singlepl.cpp',
    'wwpacket.cpp',
    include_directories : include_directories('..'),
    dependencies : [
        wwlib_dep,
        wwbitpack_dep,
        wwmath_dep,
        wwutil_dep,
        wwsaveload_dep,
    ],
)
wwnet_dep = declare_dependency(
    link_with : wwnet,
    include_directories : include_directories('.'),
    dependencies : [
        wwbitpack_dep,
    ],
)