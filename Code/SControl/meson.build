scontrol = static_library(
    'scontrol',
    'servercontrol.cpp',
    'servercontrolsocket.cpp',
    dependencies : [
        wwmath_dep,
    ]
)
scontrol_dep = declare_dependency(
    link_with : scontrol,
    include_directories : include_directories('.'),
)
