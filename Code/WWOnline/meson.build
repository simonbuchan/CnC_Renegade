wwonline = static_library(
    'wwonline',
    'GameResField.cpp',
    'GameResPacket.cpp',
    'PingProfile.cpp',
    'WaitCondition.cpp',
    'WOLChannel.cpp',
    'WOLChatMsg.cpp',
    'WOLChatObserver.cpp',
    'WOLConnect.cpp',
    'WOLDownload.cpp',
    'WOLErrorUtil.cpp',
    'WOLGame.cpp',
    'WOLGameOptions.cpp',
    'WOLLadder.cpp',
    'WOLLoginInfo.cpp',
    'WOLNetUtilObserver.cpp',
    'WOLPageMsg.cpp',
    'WOLProduct.cpp',
    'WOLServer.cpp',
    'WOLSession.cpp',
    'WOLSquad.cpp',
    'WOLString.cpp',
    'WOLUser.cpp',
    include_directories : [
        include_directories('..', is_system : true),
    ],
    dependencies : [
        wwlib_dep,

        wolapi_dep,

        ww3d2_dep,
        wwaudio_dep,
        wwphys_dep,
        wwnet_dep,

        combat_dep,
    ]
)
wwonline_dep = declare_dependency(
    link_with : wwonline,
    include_directories : include_directories('.'),
    dependencies : [
        wolapi_dep,
    ]
)
