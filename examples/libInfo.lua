--
-- see LICENSE file in doc folder
--
-- libInfo.lua

print("\n------ virtualGUser test start --------\n")
print("\n-------Interfaces------\n")
print("\n------ MPWindow -------\n")
for n,v in pairs(MPWindow) do 
	print(n,v) 
end
print("\n------ MPProcess -------\n")
for n,v in pairs(MPProcess) do 
	print(n,v) 
end
print("\n------ MPRuntime -------\n")
for n,v in pairs(MPRuntime) do 
	print(n,v) 
end
print("\n------ virtualGUser test end ----------\n")
