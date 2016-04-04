#include <Bullet.h>
#include <MeshFactory.h>
#include <MeshInterface.h>

Bullet::Bullet(int _dir, Shader * _shader) :
	MeshEntity(MeshFactory::getPlaneMesh(), _shader),
	dir(_dir),
	bbox(sweet::Rectangle(0, 0, 1, 1))
{
}

void Bullet::update(Step* _step) {
	firstParent()->translate(0.f, dir * 2.f, 0.f);
	glm::vec3 worldPos = getWorldPos();
	bbox.x = worldPos.x;
	bbox.y = worldPos.y;
	MeshEntity::update(_step);
}