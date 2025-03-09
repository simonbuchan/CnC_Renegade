wwdebug = static_library(
    'wwdebug',
    'wwdebug.cpp',
    'wwmemlog.cpp',
    'wwprofile.cpp',
#    dependencies : [wwlib_dep], mutual dependency
    include_directories : [include_directories('../wwlib')],
)
wwdebug_dep = declare_dependency(
    link_with : wwdebug,
    include_directories : [include_directories('.')],
)