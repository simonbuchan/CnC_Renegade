wwaudio = static_library(
    'wwaudio',
    'AudibleSound.cpp',
    'AudioEvents.cpp',
    'AudioSaveLoad.cpp',
    'FilteredSound.cpp',
    'Listener.cpp',
    'listenerhandle.cpp',
    'LogicalListener.cpp',
    'LogicalSound.cpp',
    'sound2dhandle.cpp',
    'Sound3D.cpp',
    'sound3dhandle.cpp',
    'SoundBuffer.cpp',
    'soundhandle.cpp',
    'SoundPseudo3D.cpp',
    'SoundScene.cpp',
    'SoundSceneObj.cpp',
    'soundstreamhandle.cpp',
    'Utils.cpp',
    'WWAudio.cpp',
    dependencies : [
        sdk_miles_dep,

        wwlib_dep,
        wwsaveload_dep,
        wwmath_dep,
        ww3d2_dep,
    ],
)
wwaudio_dep = declare_dependency(
    link_with : wwaudio,
    include_directories : include_directories('.'),
    dependencies : [sdk_miles_dep],
)