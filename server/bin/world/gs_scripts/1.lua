glof = function()
    print("ddd")
end




f1 = function(s)
    local b = "BBB BBB"
    return s .. " AA " .. b
end



f2 = function(plr, a)
    print("begin or f2")
    local t = 3
    local b = a * a
    local ret = f1("sss")
    local method = plr:not_exist_method()
    print("end or f2")
end



f3 = function(plr, str)
    local sex = plr:Sex()
    print("plr:", sex, str)
    f2(plr, sex)
end



f4 = function(plr)
    local s = "local var in f4"
    print("f4 BEGIN")
    f3(plr, "this is from f4")        
    print("f4 END")
end





on_s = function(plr)
    print("this is args", plr)
    local mt = getmetatable(plr)
    for k, v in pairs(mt) do
        print(k, v)
    end
    
    f4(plr)
    print("________________")
end

