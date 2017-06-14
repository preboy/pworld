
-------------------- class model --------------------

Class =
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



RingQueue = Class.extend(nil, {

    -------------- ctor --------------

    ctor = function (self, n)
        self._head = -1
        self._tail = -1

        if n then
            self._QMAX = n
        else
            self._QMAX = 8
        end
    end,

    -------------- public --------------

    empty = function(self)
        return self._head == -1
    end,

    full = function (self)
        return self:ptr_inc(self._tail) == self._head
    end,

    push = function (self, e)
        if self:full() then return false end

        if self:empty() then
            self._head = 1
            self._tail = 1
        else
            self._tail = self:ptr_inc(self._tail)
        end

        self[self._tail] = e
        return true
    end,

    pop = function (self)
        if self:empty() then return end

        if self._head == self._tail then
            self._head = -1
            self._tail = -1
        else
            self._head = self:ptr_inc(self._head)
        end
    end,

    head = function (self)
        if self:empty() then return nil end

        return self[self._head]
    end,

    size = function (self)
        if self:empty() then return 0 end

        local n = self._tail - self._head
        if n < 0 then
            n = n + self._QMAX
        end

        return n + 1
    end,

    -------------- private --------------

    ptr_inc = function (self, ptr)
        ptr = ptr + 1
        if ptr > self._QMAX then
            ptr = 1
        end
        return ptr
    end,
})
