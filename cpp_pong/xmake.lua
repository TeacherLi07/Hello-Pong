add_rules("mode.debug", "mode.release")

target("pong_cli")
    set_kind("binary")
    add_files("src/*.cpp")
    if is_plat("windows") then
        add_defines("PLATFORM_WINDOWS")
        add_syslinks("user32")
    else
        add_defines("PLATFORM_POSIX")
    end
