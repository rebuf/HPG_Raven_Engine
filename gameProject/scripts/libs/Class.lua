
------------------------------------------------------------------------------
--              This file is part of the Raven Engine                       --
--            this file is used to declare a class in lua                   -- 
------------------------------------------------------------------------------ 



function typeof(var)  
    local _type = type(var);  
    if(_type ~= "table" and _type ~= "userdata") then  
        return _type;  
    end  
    local _meta = getmetatable(var);  
    if(_meta ~= nil and _meta.__cname ~= nil) then  
        return _meta.__cname;  
    else  
        return _type;  
    end  
end  

function class(classname,super)
  local newClass = {}
  newClass.__cname = classname
  if super ~= nil then
    newClass.super = super
    newClass.__index = super
  else
    newClass.__index = newClass
  end

  newClass.new = function(...)
      local obj = {}
      local currentClass = newClass
      local currentObj = obj
      local bases = {}

      while currentClass ~= nil do
          for k, v in pairs(currentClass) do
              if type(v) == 'function' then 
                  currentObj[k] = v
              end
              
              if obj[k] == nil and k ~= 'super' and k ~= 'new' then
                  if type(v) == 'table' then
                      obj[k] = {}
                      depthCopy(v, obj[k])
                  else
                      obj[k] = v
                  end
              end
          end
          
          bases[#bases + 1] = currentObj
          currentObj = currentObj.super or {}
          --currentObj.super = {}
          currentClass = currentClass.super
      end
      
      for i = 2, #bases do
          local base = bases[i]
          
          for k, v in pairs(base) do
              if type(v) == 'function' then
                  setVirtualFunction(base, k)
              end
          end
      end


      local callSuper
      callSuper = function(super,...)
        if(super.ctor ~= nil and super ~= nil) then
                super:ctor(...)
            elseif super.super ~= nil then
                callSuper(super.super)
            end
        end

        if obj.ctor then  
            obj.ctor(obj,...)
        elseif obj.super then
            callSuper(obj.super)
        end
   
        setmetatable(obj,{
			__index = newClass
		})
      --currentObj.super = nil
       
      return obj
  end
  return newClass
end

function depthCopy(from, to)
  for k, v in pairs(from) do
      if type(v) == 'table' and v~= from then
          to[k] = {}
          depthCopy(v, to[k])
      else
          to[k] = v
      end
  end
end

function setVirtualFunction(target, targetFunction)
  local oldFunction = target[targetFunction]
  
  local newFunction = function(self, ...)
      if self.super == nil then
          oldFunction(self, ...)
          return
      end
      if self.super.super == nil then
        oldFunction(self, ...)
        return
    end

      local supersuper = self.super.super
      local super = self.super
      
      self.super = supersuper
      oldFunction(self, ...)
      self.super = super
  end
  
  target[targetFunction] = newFunction
end

function instanceOf(cls, name)
    if(cls == nil) then 
        return false
    end


    if(type(cls) ~= "table") then 
        return false
    end

    if rawget(cls, "__cname") == name then return true end
    local __index = rawget(cls, "__index")
    if not __index then return false end
    return instanceOf(__index, name)
end

function printTable( tbl , level, filteDefault)
  local msg = ""
  filteDefault = filteDefault or true --（DeleteMe, _class_type）
  level = level or 1
  local indent_str = ""
  for i = 1, level do
    indent_str = indent_str.."  "
  end

  LOGV(indent_str .. "{")
  for k,v in pairs(tbl) do
    if filteDefault then
      --if k ~= "__index" or k == "__index" and v ~= tbl  then
        local item_str = string.format("%s%s = %s", indent_str .. " ",tostring(k), tostring(v))
        LOGV(item_str)
        if type(v) == "table" and v ~= tbl then
          printTable(v, level + 1)
        end
      --end
    else
      local item_str = string.format("%s%s = %s", indent_str .. " ",tostring(k), tostring(v))
      LOGV(item_str)
      if type(v) == "table" then
        printTable(v, level + 1)
      end
    end
  end
  LOGV(indent_str .. "}")
end



