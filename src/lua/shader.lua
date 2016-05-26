
-- Copyright (c) 2016 Luke San Antonio
-- All rights reserved.

local ffi = require("ffi")

local rc = require("redcrane")

local shader = {}

function make_shader_dsl_env()
    local env = {}

    -- We don't support geometry shader anything quiet yet, but the C interface
    -- should, it's just this DSL implementation that is holding things back.
    -- Create a new shader
    function env.start_shader(name)
        assert(name and type(name) == "string")
        env.builder_ptr_ =
            ffi.gc(ffi.C.redc_make_shader_builder(rc.engine, name),
                   ffi.C.redc_unmake_shader_builder)
    end

    -- Compile and link shader
    function env.complete_shader()
        ffi.C.redc_shader_compile_vertex_part(env.builder_ptr_)
        ffi.C.redc_shader_compile_fragment_part(env.builder_ptr_)

        -- This next function is specifically going to deallocate the builder
        -- pointer properly, so don't worry about it ourselves.
        ffi.gc(env.builder_ptr_, nil)

        return ffi.gc(ffi.C.redc_shader_link(env.builder_ptr_),
                      ffi.C.redc_shader_destroy)
    end

    -- Destroy the builder without making a new shader
    function env.cancel_shader()
        -- We are actually relying on the Lua gc to do this for us but whatever
        -- it will eventually work fine, it's also safer anyway because we don't
        -- have to rely on the user calling this function unconditionally.
        env.builder_ptr_ = nil
    end

    function env.version(v)
        env.vs_code{"#version "..v[1]}
        env.fs_code{"#version "..v[1]}
    end

    -- Generate inputs of the vertex shader, no need to tag because they should
    -- be in a well specified order in the code anyway, etc.
    function env.vs_input(tbl)
        local location = tbl.location
        local in_type = tbl[1]
        local in_var = tbl[2]
        local str = "layout(location = "..location..") in "..in_type.." "
            ..in_var..";\n"
        ffi.C.redc_shader_append_vertex_part(env.builder_ptr_, str)
    end

    -- Define a fragment shader output, not so important but it's nice to have
    -- it done for us.
    function env.fs_output(tbl)
        local out_type = tbl[1]
        local out_var = tbl[2]
        local str = "out "..out_type.." "..out_var..";\n"
        ffi.C.redc_shader_append_fragment_part(env.builder_ptr_, str)
    end

    -- Generate outputs of the vertex shader that corrospond to inputs of the
    -- fragment shader, these aren't used outside the glsl code so no need to
    -- tag them, etc.
    function env.vs_fs_interface(tbl)
        local var_type = tbl[1]
        local var_name = tbl[2]

        local vert_str = "out "..var_type.." "..var_name..";\n"

        ffi.C.redc_shader_append_vertex_part(env.builder_ptr_, vert_str)

        local frag_str = "in "..var_type.." "..var_name..";\n"
        ffi.C.redc_shader_append_fragment_part(env.builder_ptr_, frag_str)
    end

    -- Generate uniform code for any kind of shader and tag it
    function tag_gen_uniform(tbl, klass)
        local uniform_type = tbl[1]
        local uniform_tag = tbl[2]
        local uniform_name = tbl[3] or uniform_tag

        -- Form the glsl code
        local str = "uniform "..uniform_type.." "..uniform_name..";\n"

        -- Append this code to the respective shader "part"
        ffi.C["redc_shader_append_"..klass.."_part"](env.builder_ptr_, str)

        -- Tag the uniform, this can happen again for the same uniform but a tag
        -- should corrospond 1-to-1 with a variable name.
        ffi.C.redc_shader_tag_uniform(env.builder_ptr_, uniform_tag, uniform_name)
    end

    -- Specializations so the user doesn't mess up the tag
    function env.vs_uniform(tbl)
        tag_gen_uniform(tbl, "vertex")
    end
    function env.fs_uniform(tbl)
        tag_gen_uniform(tbl, "fragment")
    end

    -- Appends code to any kind of shader type, easier than writing the entire
    -- thing twice.
    function append_code(code, klass)
        ffi.C["redc_shader_append_"..klass.."_part"](env.builder_ptr_, code)
    end

    function env.vs_code(code)
        append_code(code[1], "vertex")
    end
    function env.fs_code(code)
        append_code(code[1], "fragment")
    end

    return env
end

loaded_shaders = {}

function load_shader_source(filename)
    -- Open the file
    local file = io.open(filename)
    local source = ""

    -- If it's a good file load it all into a string and return that
    if file then
        source = file:read("*all")
        file:close()
        rc.internal:log_i("Loaded shader file '"..filename.."'")
    else
        -- Otherwise log this
        rc.internal:log_e("Failed to load shader file "..filename.."'")
    end
    -- Return loaded code (or an empty string).
    return source
end

function shader.load_shader(name)

    -- This is where all shaders live.
    local base_dir = "../assets/shader";

    -- Load the shader dsl file.
    local shader_dsl = loadfile(base_dir.."/"..name..".lua")

    -- If that Lua file doesn't exist

    if not shader_dsl then
        -- Try to load vs.glsl and fs.glsl from a directory (which should be
        -- the shader name).
        local shader_dir = base_dir.."/"..name;

        -- Load the two shaders
        local vs_source = load_shader_source(shader_dir.."/vs.glsl")
        local fs_source = load_shader_source(shader_dir.."/fs.glsl")

        -- We can't compile the shader source in right away because we don't
        -- have a a shader builder / environment set up yet.
        shader_dsl = function()
            -- So use a function that does this right away.
            vs_code({vs_source})
            fs_code({fs_source})
        end
    else
        -- Log the fact that we used the script.
        rc.internal:log_i("Found shader DSL for '"..name.."'")
    end

    -- Make a *new* shader builder / dsl environment
    local env = make_shader_dsl_env()

    -- Actually make use of the sandbox, this is a pretty limited one and
    -- doesn't include any redcrane functions or even basic Lua functions that
    -- are safe. We may decide they are useful, but so far it looks like the
    -- shader files are just going to be fairly declarative.
    setfenv(shader_dsl, env)

    -- Start a shader, run the script, then compile and link the shader
    env.start_shader(name)
    shader_dsl()
end

return shader
