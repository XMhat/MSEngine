Console.Write("Hello world from my new app!");
local fFont<const> = Font.Console();
Core.OnTick(function()
  fFont:Print(10, 10, "Hello world!");
end);
