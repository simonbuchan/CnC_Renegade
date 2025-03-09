wwsaveload = static_library(
    'wwsaveload',
    'definition.cpp',
    'definitionfactory.cpp',
    'definitionfactorymgr.cpp',
    'definitionmgr.cpp',
    'parameter.cpp',
    'persistfactory.cpp',
    'pointerremap.cpp',
    'saveload.cpp',
    'saveloadstatus.cpp',
    'saveloadsubsystem.cpp',
    'twiddler.cpp',
    'wwsaveload.cpp',
    dependencies : [
        wwlib_dep,
        wwdebug_dep,
        wwmath_dep,
    ],
)
wwsaveload_dep = declare_dependency(
    link_with : wwsaveload,
    include_directories : include_directories('.'),
    dependencies : [wwmath_dep],
)