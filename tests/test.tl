require "ifj21"

global foo : function (string, integer) : string, integer

function foo (a : string, b : integer) : string, integer
    b = b*(b + b)
    return "hell",5
end

function concat (x : string, y : string) : string, integer
    return x
end
function main()
    local x : string
    local ret: integer
end
main()