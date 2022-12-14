set_project("ucxxrt")
set_xmakever("2.7.3")

set_languages("c99", "cxx17")

add_rules("mode.debug", "mode.release")

target("ucxxrt")
    add_rules("wdk.env.wdm", "wdk.static")

    if is_mode("debug") then
        add_defines("_DEBUG", "DBG")
    else
        add_defines("NODEBUG")
    end
    
    add_defines(
        "POOL_ZERO_DOWN_LEVEL_SUPPORT=1",
        "POOL_NX_OPTIN=1",
        "_NO_CRT_STDIO_INLINE",
        "_HAS_EXCEPTIONS=1",
        "_ITERATOR_DEBUG_LEVEL=0",
        "_KERNEL_MODE=1",
        "__KERNEL_MODE=1",
        "NOMINMAX=1",
        "DEPRECATE_DDK_FUNCTIONS=1",
        "MSC_NOOPT"
    )

    set_policy("check.auto_ignore_flags", false)
    on_config(function (target)
        -- remove kernel cxflags
        print("remove flag: -kernel")
        local cxflags = target:get("cxflags")
        if "table" == type(cxflags) then
            for i, v in ipairs(cxflags) do
                cxflags[i] = string.gsub(v, "-kernel", "-no-kernel")    
            end
        else
            cxflags = string.gsub(cxflags, "-kernel", "-no-kernel")    
        end
        target:set("cxflags", cxflags, {force=false})
    end)
    
    set_pcxxheader("src/ucxxrt.inl")

    add_includedirs("./")
    add_includedirs("src")
    add_includedirs("src/ucrt/inc")
    add_includedirs("src/crt/vcruntime")
    
    -- add_files("src/crt/stl/**.cpp|syserror_import_lib.cpp")
    add_files("src/crt/stl/syserror.cpp", "src/crt/stl/xthrow.cpp")
    
    add_files("src/crt/vcruntime/**.cpp")
    add_files("src/ucrt/**.cpp")
    add_files("src/*.cpp")

    if is_arch("x86") then
        add_files("src/crt/x86/**.cpp")
        add_files("src/crt/x86/**.asm")
    elseif is_arch("x64") then
        add_files("src/crt/x64/**.cpp")
        add_files("src/crt/x64/**.asm")
    elseif is_arch("arm64") then
        add_files("src/crt/arm64/**.cpp")
        add_files("src/crt/arm64/**.asm")
    elseif is_arch("arm") then
        add_files("src/crt/arm/**.cpp")
        add_files("src/crt/arm/**.asm")
    end

    -- 开启try catch支持seh
    add_cxflags("/EHa")

target("ucxxrt.test")
    add_rules("wdk.env.wdm", "wdk.driver")

    if is_mode("debug") then
        add_defines("_DEBUG", "DBG")
    else
        add_defines("NODEBUG")
    end
    
    add_defines(
        "POOL_ZERO_DOWN_LEVEL_SUPPORT=1",
        "POOL_NX_OPTIN=1",
        "_NO_CRT_STDIO_INLINE",
        "_HAS_EXCEPTIONS=1",
        "_ITERATOR_DEBUG_LEVEL=0",
        "_KERNEL_MODE=1",
        "__KERNEL_MODE=1",
        "NOMINMAX=1",
        "DEPRECATE_DDK_FUNCTIONS=1",
        "MSC_NOOPT"
    )

    set_policy("check.auto_ignore_flags", false)
    on_config(function (target)
        print("remove flag: -kernel")
        local cxflags = target:get("cxflags")
        if "table" == type(cxflags) then
            for i, v in ipairs(cxflags) do
                cxflags[i] = string.gsub(v, "-kernel", "-no-kernel")
            end
        else
            cxflags = string.gsub(cxflags, "-kernel", "-no-kernel")
        end
        target:set("cxflags", cxflags, {force=false})

        -- add msvc stl includedirs
        import("detect.sdks.find_vstudio")
        local vs = find_vstudio({})
        local max_version = 0
        local sdk_includedirs = ""
        for k, v in pairs(vs) do
            local nk = tonumber(k)
            if max_version < nk then
                max_version = nk
                sdk_includedirs = vs[k]["vcvarsall"][os.arch()]["INCLUDE"]
            end
        end
        
        local incTable = {}
        sdk_includedirs = sdk_includedirs:gsub("\\\\", "\\")
        for token in string.gmatch(sdk_includedirs, "[^%;]+") do
            table.insert(incTable, token)
        end
        
        local includedirs = target:get("includedirs")
        if "table" == type(includedirs) then
            for _, v in ipairs(includedirs) do
                table.insert(incTable, v)
            end
        else
            table.insert(incTable, includedirs)
        end
        target:set("includedirs", incTable)
    end)

    add_includedirs("./")
    add_includedirs("src")

    -- 开启try catch支持seh
    add_cxflags("/EHa")

    add_deps("ucxxrt")
    add_files("test/*.cpp", "test/*.inf")
