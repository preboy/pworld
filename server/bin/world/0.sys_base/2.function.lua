
zcg = {}

--------------------- dump table ----------------------
local max_depth = 8

local function _print_prefix(str, n)
    for i = 1, n do
        io.write("    ")
    end
    io.write(str)
end

local function _print_table(tab, depth)
    for k, v in pairs(tab) do
        local key
        if type(k) ~= "string" then
            key = string.format("[%s]", tostring(k))
        else
            key = string.format("'%s'", k)
        end
        _print_prefix(key .. " = ", depth + 1)
        local type_ = type(v)
        if type_ == "number" then
            io.write(v .. ",\n")
        elseif type_ == "string" then
            io.write("\"" .. v .. "\",\n")
        elseif type_ == "table" then
            if depth + 1 <= max_depth then
                io.write("{\t-- ", tostring(v), "\n")
                _print_table(v, depth + 1)
            else
                io.write("{...},\t-- ", tostring(v), " [[max-depth]]\n")
            end
        elseif type_ == "boolean" then
            io.write(tostring(v) .. ",\n")
        else
            io.write(string.format("[%s],\n", tostring(v)))
        end
    end
    _print_prefix("},", depth)
    io.write("\n")
end

-- print table to output or file
zcg.logTable = function(tab, filename)
    local f
    if filename then
        f = io.open(filename, "w+")
        io.output(f)
    end
    if not tab then
        io.write("tab = nil\n")
        return
    end
    local type_ = type(tab)
    if type_ ~= "table" then
        io.write(string.format("%s\n", tostring(tab)))
    else
        io.write("ROOT = {\n")
        local depth = 0
        _print_table(tab, depth)
    end
    if f then
        f:flush()
        f:close()
    end
end


--------------------- debug extends ----------------------

-- dump local variables in lua stack
zcg.dump_local = function(n)
    local idx = 1
    repeat
        local name, value = debug.getlocal(n, idx)
        if not name then
            break
        end
        print("var:", name, value)
        idx = idx + 1
    until false
end


-- 模拟 CLuaEngine::_on_lua_error
zcg.dump_locals = function(err)

    local stack = debug.traceback(nil, nil, 5)
    print("========================= START DUMP LOCAL VARIABLE =========================")
    for i = 3, 20 do
        local info = debug.getinfo(i, "n")
        if not info then
            break
        end
        local func = string.format("from `%s`", info.name)
        info = debug.getinfo(i-1, "n")
        if info then
            func = string.format("name `%s`", info.name)
        end
        print(string.format("==== The [%d  (%s)] stack level ====", i, func))
        zcg.dump_local(i)
    end

    return string.format("\nError report:\n\t%s\n%s", err, stack)
end


--------------------- utility tools ----------------------

-- 将整数(20151120)转换成秒数
zcg.date_to_os_time = function(val)
    val = tostring(val)
    local t1 = string.sub(val, 0, 4)
    local t2 = string.sub(val, 5, 6)
    local t3 = string.sub(val, 7, 8)
    local tt =
    {
        year    = tonumber(t1),
        month   = tonumber(t2),
        day     = tonumber(t3),
        hour    = 0,
        min     = 0,
        sec     = 0
    }
    return os.time(tt)
end
