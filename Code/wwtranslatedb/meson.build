wwtranslatedb = static_library(
    'wwtranslatedb',
    'stringtwiddler.cpp',
    'tdbcategory.cpp',
    'translatedb.cpp',
    'translateobj.cpp',
    dependencies : [
        wwlib_dep,
        wwsaveload_dep,
    ],
)
wwtranslatedb_dep = declare_dependency(
    link_with : wwtranslatedb,
    include_directories : include_directories('.'),
)
