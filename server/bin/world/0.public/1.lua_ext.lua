-- respect to Mr.Long

math.round = function (v)
    if (v >= 0) then
        return math.floor(v + 0.5)
    else
        return math.ceil(v - 0.5)
    end
end

-------------------------------------------------------------------------------

table.shuffle = function (t)
    if t == nil then return nil end

    local n
    for i = 2, #t do
        n = math.random(i)
        t[i], t[n] = t[n], t[i]
    end

    return t
end


table.dump = function (t)

    local function tab_indent(lv)
        for i = 1, lv do
            io.write("    ")
        end
    end

    local function print_key(k, kt, lv)
        tab_indent(lv)
        if kt == "number" then
            io.write("["..k.."]")
        else
            io.write(k)
        end
    end

    local function print_val(v, vt, lv)
        if vt == "number" then
            io.write(v)
        elseif vt == "string" then
            io.write("\""..v.."\"")
        elseif vt == "boolean" then
            io.write(v and "true" or "false")
        elseif vt == "table" then
            if lv < 16 then
                print()
                tab_indent(lv)
                print("{")

                local keyt, valt
                for key, val in pairs(v) do
                    keyt = type(key)
                    valt = type(val)

                    print_key(key, keyt, lv + 1)
                    io.write(" = ")
                    print_val(val, valt, lv + 1)
                    print()
                end

                tab_indent(lv)
                io.write("}")
            else
                io.write("{}")
            end
        else
            io.write("<<< "..tostring(v).." >>>")
        end
        if lv > 0 then io.write(",") end
    end

    io.write("Root = ")
    print_val(t, type(t), 0)
end


string.split = function (str, sep)
    sep = sep or " "

    local t = {}
    for v in string.gmatch(str, "[^"..sep.."]+") do
        table.insert(t, v)
    end
    return t
end


table.print = function(tab)
    for k, v in pairs(tab) do
        print(k, v)
    end
end


table.size = function(tab)
    local count = 0
    for k, v in pairs(tab) do
        count  = count + 1
    end
    return count
end


table.default_value = function(tab, val)
    if type(tab) ~= "table" then
        print("[luaext ERROR]: Not a table!")
        return
    end
    local __index_func = function(t, k)
        rawset(t, k, val)
        return val
    end
    local mt = getmetatable(tab)
    if not mt then
        setmetatable(tab, { __index = __index_func })
    else
        if mt.__index then
            print("[luaext ERROR]: Alreay has a __index meta")
            return
        else
            mt.__index = __index_func
        end
    end
end
