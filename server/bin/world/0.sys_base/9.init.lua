
-- make gc more aggresive
collectgarbage("setstepmul", 1000)


-- init random
math.randomseed(os.time())
for i = 1, 100 do math.random() end



--[[
    由于脚本加载先后顺序而引起的问题，
    如:在1.lua文件定义变量指向2.lua文件中才定义的变量
    肯定定义不成功，故提出此解决办法
    你可以在自己的lua文件中提供一个函数，并注册
    当系统内所有的脚本加载完成的时候，系统会把统一调用所有注册的函数
--]]
local _file_init_funs = {}


-- 在各个脚本文件中提供一个local化的函数，负责初始化脚本内部变量
RegisterFileInit = function(func)
    table.insert(_file_init_funs, func)
end


RunFileInit = function()
    for _, func in ipairs(_file_init_funs) do
        func()
    end
    _file_init_funs = {}
end
