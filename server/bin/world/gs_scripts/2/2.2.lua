print("2.2")


e = function()

    print("in e")
end



ga1 = 
{
   { a = 1, b = 2, c = 3, d = 4},
   a = 199,
   x = 
   {
        100, 200, 300,  
        {
            xxx = 1, yyy = "b", zzz = true,
        }
   }

}

local l = #ga1

print ( next(ga1, l) )

