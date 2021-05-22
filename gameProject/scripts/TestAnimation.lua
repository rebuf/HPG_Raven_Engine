
dofile("scripts/libs/Class.lua")

-- declare BaseClass
local ScriptBehavior = require("ScriptBehavior")

-- extends from ScriptBehavior
local TestAnimation = class("TestAnimation",ScriptBehavior)


--constructor
function TestAnimation:ctor()
	LOGV("TestAnimation:ctor")

end

function TestAnimation:OnInit()
-- self.entity is the Entity that current component bind to 
-- c++ class is Entity
end


function TestAnimation:OnUpdate(dt)

	if Input.IsKeyPressed(KeyCode.A) then
		LOGV("A is pressed");
		
		--self:GetAnimator() is equal to self.entity:GetAnimator()
		self:GetAnimator():SetValue("isSetting",true);
		self:GetAnimator():SetValue("isWalking",false);
		
	end
	
	
	if Input.IsKeyPressed(KeyCode.W) then
		LOGV("W is pressed");
		
		self:GetAnimator():SetValue("isSetting",false);
		self:GetAnimator():SetValue("isWalking",true);

	end
	
	if Input.IsKeyPressed(KeyCode.S) then
		self:GetAnimator():SetValue("isSetting",false);
		self:GetAnimator():SetValue("isWalking",false);
	end
	
	if Input.IsKeyPressed(KeyCode.D) then
		LOGV("D is pressed");
	end

end

return TestAnimation;