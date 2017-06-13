glof = function()
    print("ddd")
end


on_s = function(plr, mt)
    print("this is args", plr, mt)
    local mt = getmetatable(plr)
    for k, v in pairs(mt) do
        print(k, v)
    end

    print("________________")
    print(plr:Sex(), plr:not_exist_method())
end

