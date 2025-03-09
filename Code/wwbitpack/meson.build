wwbitpack = static_library(
    'wwbitpack',
    'BitPacker.cpp',
    'bitstream.cpp',
    'encoderlist.cpp',
    'encodertypeentry.cpp',
    include_directories : include_directories(
        '.',
        '../WWMath',
        '../wwutil',
    ),
    dependencies : [
        wwlib_dep,
        wwdebug_dep,
        #wwmath_dep,
        #wwutil_dep,
    ],
)
wwbitpack_dep = declare_dependency(
    link_with : wwbitpack,
    #dependencies : wwutil_dep,
    include_directories : include_directories(
        '.',
        '../wwutil',
    ),
)