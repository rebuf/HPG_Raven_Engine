
dofile("scripts/libs/Class.lua")

-- declare BaseClass
local ScriptBehavior = require("ScriptBehavior")

-- extends from ScriptBehavior
local TestAudio = class("TestAudio",ScriptBehavior)

--constructor
function TestAudio:ctor()
	LOGV("TestAudio:ctor")
end

function TestAudio:OnInit()
-- self.entity is the Entity that current component bind to 
-- c++ class is Entity
-- don't worry about the pointer , lua will manage it;
	self.audio1 = AudioClip.new("audio/audio(1).ogg");
	self.audio2 = AudioClip.new("audio/audio(2).ogg");
	self.audio3 = AudioClip.new("audio/audio(3).ogg");
	self.audio4 = AudioClip.new("audio/audio(4).ogg");
	
	--self.audioSource = AudioSource.new();
	--self.audioSource:SetLooping(false)
end


function TestAudio:GetAudioSource()
	if self.source ~= nil then
		return self.source
	end

	if self.entity:HasSoundComponent() then
		self.source = self:GetSound():GetAudioSource()
	else
		self.source = AudioSource.new()
		self.source:SetLooping(false)
	end
	
	return self.source
end



function TestAudio:OnUpdate(dt)
	
	if Input.IsKeyHeld(KeyCode.A) then
	
		local source = self:GetAudioSource()
	
		source:Stop();--stop other audio
		source:SetAudioClip(self.audio1)
		source:Play();
		
	end
	
	if Input.IsKeyHeld(KeyCode.D) then
		local source =  self:GetAudioSource()
	
		source:Stop();--stop other audio
		source:SetAudioClip(self.audio2)
		source:Play();
	end
	
	
	if Input.IsKeyHeld(KeyCode.W) then

		local source = self:GetAudioSource()
		source:Stop();--stop other audio
		source:SetAudioClip(self.audio3)
		source:Play();

	end
	
	if Input.IsKeyHeld(KeyCode.S) then

		local source =  self:GetAudioSource()
		source:Stop();--stop other audio
		source:SetAudioClip(self.audio4)
		source:Play();

	end

end

return TestAudio;