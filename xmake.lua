set_runtimes(is_mode("debug") and "MDd" or "MD")

add_requires("catch2")

set_languages("c++20")
set_policy("build.ccache", false)
add_rules("mode.debug", "mode.release", "mode.coverage")

target("tss", function()
    set_kind("static")
    add_files("src/**.cpp")
    add_headerfiles("src/(tss/**.h)", { prefixdir = "tss" })
    add_includedirs("src/", { public = true })
    set_targetdir("./build/$(plat)/$(arch)/$(mode)/tss")
end)

target("tss-test", function()
    set_kind("binary")
    add_deps("tss")
    add_packages("catch2")
    add_includedirs("src")
    add_files("test/**.cpp")
    set_targetdir("./build/$(plat)/$(arch)/$(mode)/tss-test")
end)
