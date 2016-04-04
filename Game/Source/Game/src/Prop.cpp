#include <Prop.h>
#include <MY_ResourceManager.h>
#include <MeshFactory.h>

Prop::Prop(Shader * _shader, std::string _name) :
	Sprite(_shader),
	bbox(sweet::Rectangle(0, 0, 0, 0))
{
	auto tex = MY_ResourceManager::globalAssets->getTexture(_name)->texture;
	setPrimaryTexture(tex);
	bbox.width = tex->width;
	bbox.height = tex->height;
	meshTransform->scale(std::max(tex->width, tex->height));
	mesh->setScaleMode(GL_NEAREST);
	bbox.width = tex->width;
	bbox.height = tex->height;
}

void Prop::update(Step* _step) {
	glm::vec3 wp = getWorldPos();
	bbox.x = wp.x - bbox.width/2;
	bbox.y = wp.y - bbox.height/2 - 3;
	Sprite::update(_step);
}