#pragma once
#include "Graphics.h"
#include"Mouse.h"
#include "Keyboard.h"

class Scene {
public:
	virtual void Update(Keyboard& kbd, Mouse& mouse, float dt) = 0;
	virtual void Draw(Graphics& gfx) const = 0;
	virtual ~Scene() = default;
};