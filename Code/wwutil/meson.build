wwutil = static_library(
    'wwutil',
    ['mathutil.cpp', 'miscutil.cpp', 'stackdump.cpp'],
    dependencies: [wwlib_dep, wwdebug_dep, wwmath_dep],
)
wwutil_dep = declare_dependency(
    link_with: wwutil,
    include_directories: include_directories('.'),
)