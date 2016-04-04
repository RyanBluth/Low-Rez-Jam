#pragma once

#include <string>
#include <Sprite.h>

class Prop : public Sprite {
public:
	sweet::Rectangle bbox;

	Prop(Shader * _shader, std::string _name);

	virtual void update(Step * _step) override;
};