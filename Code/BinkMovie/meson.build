binkmovie = static_library(
    'binkmovie',
    'BINKMovie.cpp',
    'subtitle.cpp',
    'subtitlemanager.cpp',
    'subtitleparser.cpp',
    include_directories : [
        include_directories('..', is_system : true),
    ],
    dependencies : [
        sdk_bink_dep,
        wwlib_dep,
        wwdebug_dep,
        wwmath_dep,
        ww3d2_dep,
    ]
)
binkmovie_dep = declare_dependency(
    link_with : binkmovie,
    include_directories : include_directories('.'),
)
