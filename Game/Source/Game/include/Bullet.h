#pragma once

#include <MeshEntity.h>
#include <Rectangle.h>

class Bullet : public MeshEntity {
public:

	int dir;

	sweet::Rectangle bbox;

	Bullet(int _dir, Shader * _shader);

	virtual void update(Step * _step) override;
};
