dofile('rive_build_config.lua')

filter({ 'options:with_rive_tools' })
do
    defines({ 'WITH_RIVE_TOOLS' })
end
filter({ 'options:with_rive_text' })
do
    defines({ 'WITH_RIVE_TEXT' })
end
filter({})
filter({ 'options:with_rive_audio=system' })
do
    defines({ 'WITH_RIVE_AUDIO' })
end
filter({ 'options:with_rive_audio=external' })
do
    defines({ 'WITH_RIVE_AUDIO', 'EXTERNAL_RIVE_AUDIO_ENGINE', 'MA_NO_DEVICE_IO' })
end
filter({})

dofile(path.join(path.getabsolute('dependencies/'), 'premake5_harfbuzz_v2.lua'))
dofile(path.join(path.getabsolute('dependencies/'), 'premake5_sheenbidi_v2.lua'))
dofile(path.join(path.getabsolute('dependencies/'), 'premake5_miniaudio_v2.lua'))

project('rive')
do
    kind('StaticLib')
    cppdialect('C++11')
    includedirs({
        'include',
        harfbuzz .. '/src',
        sheenbidi .. '/Headers',
        miniaudio,
    })

    files({ 'src/**.cpp' })

    flags({ 'FatalCompileWarnings' })

    filter({ 'system:macosx' })
    do
        buildoptions({
            -- this triggers too much on linux, so just enable here for now
            '-Wimplicit-float-conversion',
        })
    end

    -- filter {'toolset:not msc', 'files:src/audio/audio_engine.cpp'}
    filter({ 'system:not windows', 'files:src/audio/audio_engine.cpp' })
    do
        buildoptions({ '-Wno-implicit-int-conversion' })
    end

    filter({ 'system:windows', 'files:src/audio/audio_engine.cpp' })
    do
        -- Too many warnings from miniaudio.h
        removeflags({ 'FatalCompileWarnings' })
    end

    -- filter 'files:src/audio/audio_engine.cpp'
    -- do
    --     buildoptions {
    --         '-Wno-implicit-int-conversion'
    --     }
    -- end

    filter({ 'system:macosx', 'options:variant=runtime' })
    do
        buildoptions({
            '-Wimplicit-float-conversion -fembed-bitcode -arch arm64 -arch x86_64 -isysroot '
                .. (os.getenv('MACOS_SYSROOT') or ''),
        })
    end

    filter({ 'system:ios' })
    do
        buildoptions({ '-flto=full', '-Wno-implicit-int-conversion' })
        files({ 'src/audio/audio_engine.m' })
    end

    filter('system:windows')
    do
        architecture('x64')
        defines({ '_USE_MATH_DEFINES' })
    end

    filter('system:emscripten')
    do
        buildoptions({ '-pthread' })
    end
end

newoption({
    trigger = 'variant',
    value = 'type',
    description = 'Choose a particular variant to build',
    allowed = {
        { 'system', 'Builds the static library for the provided system' },
        { 'emulator', 'Builds for an emulator/simulator for the provided system' },
        {
            'runtime',
            'Build the static library specifically targeting our runtimes',
        },
    },
    default = 'system',
})

newoption({
    trigger = 'with_rive_tools',
    description = 'Enables tools usually not necessary for runtime.',
})

newoption({
    trigger = 'with_rive_text',
    description = 'Compiles in text features.',
})

newoption({
    trigger = 'with_rive_audio',
    value = 'disabled',
    description = 'The audio mode to use.',
    allowed = { { 'disabled' }, { 'system' }, { 'external' } },
})