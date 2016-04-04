#pragma once

#include <MY_Scene_ScreenShaders.h>
#include <RenderSurface.h>
#include <StandardFrameBuffer.h>
#include <RenderOptions.h>
#include <MeshFactory.h>
#include <FBOTexture.h>
#include <Player.h>
#include <Bullet.h>
#include <Prop.h>

MY_Scene_ScreenShaders::MY_Scene_ScreenShaders(Game * _game) :
	MY_Scene_Base(_game),
	renderPlane(new MeshEntity(MeshFactory::getPlaneMesh())),
	screenSurfaceShader(new Shader("assets/engine basics/DefaultRenderSurface", false, true)),
	screenSurface(new RenderSurface(screenSurfaceShader, true)),
	screenFBO(new StandardFrameBuffer(true)),
	orthoCam(new OrthographicCamera(0, 64, 0, 64, -10, 10))
{
	screenSurface->setScaleMode(GL_NEAREST);

	activeCamera = orthoCam;
	childTransform->addChild(orthoCam);
	cameras.push_back(orthoCam);

	// set-up some UI to toggle between results
	uiLayer->addMouseIndicator();

	// memory management
	screenSurface->incrementReferenceCount();
	screenSurfaceShader->incrementReferenceCount();
	screenFBO->incrementReferenceCount();

	auto bg = new MeshEntity(MeshFactory::getPlaneMesh(32), baseShader);
	bg->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("background")->texture);
	childTransform->addChild(bg)
		->translate(32, 32, 0);

	auto c1 = new Prop(baseShader, "cactus");
	childTransform->addChild(c1)
		->translate(21.5, 27, 0);
	props.push_back(c1);

	auto b1 = new Prop(baseShader, "barrel");
	childTransform->addChild(b1)
		->translate(51.5, 27, 0);
	props.push_back(b1);

	player1 = new Player(PLAYER_1, baseShader);
	childTransform->addChild(player1)
		->translate(32.f, 5.f, 0.f)
		->scale(9.f);

	player2 = new Player(PLAYER_2, baseShader);
	childTransform->addChild(player2)
		->translate(32.f, 50.f, 0.f)
		->scale(9.f);

	auto c2 = new Prop(baseShader, "cactus");
	childTransform->addChild(c2)
		->translate(11.5, 38, 0);
	props.push_back(c2);

	auto b2 = new Prop(baseShader, "barrel");
	childTransform->addChild(b2)
		->translate(41.5, 38, 0);
	props.push_back(b2);

	p1Bullets = new Transform();
	for(unsigned long int i = 0; i < player1->bullets; ++i) {
		auto b = new MeshEntity(MeshFactory::getPlaneMesh(), baseShader);
		p1Bullets->addChild(b)
			->translate(4 + i * 2, 3, 0);
	}
	childTransform->addChild(p1Bullets);

	p2Bullets = new Transform();
	for(unsigned long int i = 0; i < player2->bullets; ++i) {
		auto b = new MeshEntity(MeshFactory::getPlaneMesh(), baseShader);
		p2Bullets->addChild(b)
			->translate(57 + i * 2, 61, 0);
	}

	childTransform->addChild(p2Bullets);

	drawText = new MeshEntity(MeshFactory::getPlaneMesh(32), baseShader);
	childTransform->addChild(drawText)->translate(32, 32, 0);
	drawText->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("drawText")->texture);
	drawText->setVisible(false);

	redText = new MeshEntity(MeshFactory::getPlaneMesh(32), baseShader);
	childTransform->addChild(redText)->translate(32, 32, 0);
	redText->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("redText")->texture);
	redText->setVisible(false);

	blueText = new MeshEntity(MeshFactory::getPlaneMesh(32), baseShader);
	childTransform->addChild(blueText)->translate(32, 32, 0);
	blueText->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("blueText")->texture);
	blueText->setVisible(false);
}

MY_Scene_ScreenShaders::~MY_Scene_ScreenShaders(){
	// memory management
	screenSurface->decrementAndDelete();
	screenSurfaceShader->decrementAndDelete();
	screenFBO->decrementAndDelete();
}

void MY_Scene_ScreenShaders::update(Step * _step){
	while(player1->bullets < p1Bullets->children.size()) {
		p1Bullets->removeChild(p1Bullets->children.back());
		emitBullet(player1->firstParent(), 1);
	}
	while(player2->bullets < p2Bullets->children.size()) {
		p2Bullets->removeChild(p2Bullets->children.back());
		emitBullet(player2->firstParent(), -1);
	}
	for(unsigned long int i = 0; i < bullets.size();){
		if(bullets.at(i)->dir > 0) {
			if(bullets.at(i)->bbox.intersects(player2->bbox)) {
				blueText->setVisible(true);
			}
		}else {
			if(bullets.at(i)->bbox.intersects(player1->bbox)) {
				redText->setVisible(true);
			}
		}

		bool hitProp = false;

		for(auto p : props) {
			if(p->bbox.intersects(bullets.at(i)->bbox)) {
				hitProp = true;
			}
		}

		if(hitProp || bullets.at(i)->getWorldPos().y > 70 || bullets.at(i)->getWorldPos().y < -10) {
			childTransform->removeChild(bullets.at(i)->firstParent());
			bullets.erase(bullets.begin() + i);
		}else {
			++i;
		}
	}

	if(blueText->isVisible() || redText->isVisible() || drawText->isVisible()) {
		player1->active = false;
		player2->active = false;

		if(keyboard->keyJustUp(GLFW_KEY_SPACE)) {
			MY_Scene_ScreenShaders * ns = new MY_Scene_ScreenShaders(game);
			std::string key = std::to_string(sweet::step.time);
			game->scenes[key] = ns;
			game->switchScene(key, true);
		}
	}

	if(!redText->isVisible() && !blueText->isVisible() && bullets.size() == 0 &&  player1->bullets <= 0 && player2->bullets <= 0) {
		drawText->setVisible(true);
	}

	// Scene update
	MY_Scene_Base::update(_step);
}

void MY_Scene_ScreenShaders::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	_renderOptions->clear();
	_renderOptions->setViewPort(0, 0, 64, 64);
	_renderOptions->depthEnabled = false;
	// keep our screen framebuffer up-to-date with the current viewport
	screenFBO->resize(64, 64);

	// bind our screen framebuffer
	FrameBufferInterface::pushFbo(screenFBO);
	// render the scene
	MY_Scene_Base::render(_matrixStack, _renderOptions);
	// unbind our screen framebuffer, rebinding the previously bound framebuffer
	// since we didn't have one bound before, this will be the default framebuffer (i.e. the one visible to the player)
	FrameBufferInterface::popFbo();

	_renderOptions->clear();
	auto sd = sweet::getWindowDimensions();
	int min = std::min(sd.y, sd.x);
	int max = std::max(sd.y, sd.x);
	bool xBigger = sd.x > sd.y;
	//_renderOptions->setViewPort(0, 0, min, min);
	_renderOptions->setViewPort(xBigger ? (max - min) * 0.5f : 0, xBigger ? 0 : (max - min) * 0.5f, min, min);

	// render our screen framebuffer using the standard render surface

	screenSurface->render(screenFBO->getTextureId());

	// render the uiLayer after the screen surface in order to avoid hiding it through shader code
	uiLayer->render(_matrixStack, _renderOptions);
}

void MY_Scene_ScreenShaders::load(){
	MY_Scene_Base::load();

	screenSurface->load();
	screenFBO->load();
}

void MY_Scene_ScreenShaders::emitBullet(Transform * _from, int _yDir) {
	auto b = new Bullet(_yDir, baseShader);
	childTransform->addChild(b)
		->translate(_from->getTranslationVector(), false);
	bullets.push_back(b);
}

void MY_Scene_ScreenShaders::unload(){
	screenFBO->unload();
	screenSurface->unload();

	MY_Scene_Base::unload();
}