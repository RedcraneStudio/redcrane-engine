local client = ...

redcrane.server:start(32)

while redcrane.server:running() do
    redcrane.server:step()
    if client ~= nil then
        local err, ret = coroutine.resume(client)
        if ret == 0 then
            break
        end
    end
end

if client == nil then
    return 0
end

local err, ret = coroutine.resume(client)
return ret
