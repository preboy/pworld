
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


--------------------- dump table ----------------------
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


-------------------- another class --------------------

zcg.Class =
{
    __call = function (p, ...)
        local o = {}
        setmetatable(o, p)
        local c = {}
        local ctors = nil
        repeat
            if p.ctor ~= ctors then
                ctors = p.ctor
                table.insert(c, p.ctor)
            end
            p = getmetatable(p)
        until not p
        for i = #c, 1, -1 do
            c[i](o, ...)
        end
        return o
    end,

    extend = function(p, b)
        b.__index = b
        b.__call  = Class.__call
        if p then
            setmetatable(b, p)
        else
            setmetatable(b, Class)
        end
        return b
    end,

    parent = function(cls)
        return getmetatable(getmetatable(cls))
    end,
}
