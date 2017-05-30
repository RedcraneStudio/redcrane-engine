local client = ...

redcrane.server:start(32)

while redcrane.server:running() do
    redcrane.server:step()
end

if client == nil then
    return 0
end

local err, ret = coroutine.resume(client)
return ret
