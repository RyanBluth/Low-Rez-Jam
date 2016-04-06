#pragma once

#include <MY_Scene_Main.h>
#include <RenderSurface.h>
#include <StandardFrameBuffer.h>
#include <RenderOptions.h>
#include <MeshFactory.h>
#include <FBOTexture.h>
#include <Player.h>
#include <Bullet.h>
#include <Prop.h>

MY_Scene_Main::MY_Scene_Main(Game * _game, bool _showMenu) :
	MY_Scene_Base(_game),
	screenSurfaceShader(new Shader("assets/RenderSurface_1", false, true)),
	screenSurface(new RenderSurface(screenSurfaceShader, true)),
	screenFBO(new StandardFrameBuffer(true)),
	renderPlane(new MeshEntity(MeshFactory::getPlaneMesh())),
	orthoCam(new OrthographicCamera(0, 64, 0, 64, -10, 10))
{
	screenSurface->setScaleMode(GL_NEAREST);
	screenSurface->uvEdgeMode = GL_CLAMP_TO_BORDER;

	activeCamera = orthoCam;
	childTransform->addChild(orthoCam);
	cameras.push_back(orthoCam);

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
		->translate(30.f, 6.f, 0.f);

	player2 = new Player(PLAYER_2, baseShader);
	childTransform->addChild(player2)
		->translate(30.f, 58.f, 0.f);

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

	// Add menu screen last{
	menu = new MeshEntity(MeshFactory::getPlaneMesh(32.f), baseShader);
	childTransform->addChild(menu)->translate(32.f, 32.f, 0);
	menu->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("menu")->texture);
	menu->mesh->setScaleMode(GL_NEAREST);
	menu->setVisible(_showMenu);
}

MY_Scene_Main::~MY_Scene_Main(){
	// memory management
	screenSurface->decrementAndDelete();
	screenSurfaceShader->decrementAndDelete();
	screenFBO->decrementAndDelete();
}

void MY_Scene_Main::update(Step * _step){
	// Screen shader update
	// Screen shaders are typically loaded from a file instead of built using components, so to update their uniforms
	// we need to use the OpenGL API calls
	screenSurfaceShader->bindShader(); // remember that we have to bind the shader before it can be updated
	GLint test = glGetUniformLocation(screenSurfaceShader->getProgramId(), "time");
	checkForGlError(0);
	if(test != -1){
		glUniform1f(test, _step->time);
		checkForGlError(0);
	}

	if(keyboard->keyJustDown(GLFW_KEY_L)){
		screenSurfaceShader->unload();
		screenSurfaceShader->loadFromFile(screenSurfaceShader->vertSource, screenSurfaceShader->fragSource);
		screenSurfaceShader->load();
	}

	if(!menu->isVisible()){
		updateBulletDisplay();

		for(unsigned long int i = 0; i < bullets.size();){
			bool hitSomething = false;

			if(bullets.at(i)->dir > 0) {
				if(bullets.at(i)->bbox.intersects(player2->bbox)) {
					if(!drawText->isVisible()){
						player2->die();
						hitSomething = true;
					}
				}
			}else {
				if(bullets.at(i)->bbox.intersects(player1->bbox)) {
					if(!drawText->isVisible()){
						player1->die();
						hitSomething = true;
					}
				}
			}

			for(auto p : props) {
				if(p->bbox.intersects(bullets.at(i)->bbox)) {
					hitSomething = true;
				}
			}

			if(hitSomething || bullets.at(i)->getWorldPos().y > 70 || bullets.at(i)->getWorldPos().y < -10) {
				childTransform->removeChild(bullets.at(i)->firstParent());
				bullets.erase(bullets.begin() + i);
			}else {
				++i;
			}
		}

		if(blueText->isVisible() || redText->isVisible() || drawText->isVisible()) {
			if(keyboard->keyJustUp(GLFW_KEY_SPACE)) {
				MY_Scene_Main * ns = new MY_Scene_Main(game, false);
				std::string key = std::to_string(sweet::step.time);
				game->scenes[key] = ns;
				game->switchScene(key, true);
			}
		}

		if(player2->hit && player1->hit) {
			drawText->setVisible(true);
			redText->setVisible(false);
			blueText->setVisible(false);
		}

		if(player2->hit && !player1->hit && bullets.size() == 0) {
			drawText->setVisible(false);
			redText->setVisible(false);
			blueText->setVisible(true);
		}

		if(player1->hit && !player2->hit && bullets.size() == 0) {
			drawText->setVisible(false);
			redText->setVisible(true);
			blueText->setVisible(false);
		}
	}

	if(keyboard->keyJustUp(GLFW_KEY_SPACE)) {
		menu->setVisible(false);
	}

	// Scene update
	MY_Scene_Base::update(_step);
}

void MY_Scene_Main::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	_renderOptions->clear();
	_renderOptions->setViewPort(0, 0, 64, 64);
	_renderOptions->depthEnabled = false;
	// keep our screen framebuffer up-to-date with the current viewport
	screenFBO->resize(64, 64);

	// bind our screen framebuffer
	FrameBufferInterface::pushFbo(screenFBO);
	// render the scene
	uiLayer->setVisible(false);
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
	uiLayer->setVisible(true);
	uiLayer->render(_matrixStack, _renderOptions);
}

void MY_Scene_Main::load(){
	MY_Scene_Base::load();

	screenSurface->load();
	screenFBO->load();
}

void MY_Scene_Main::emitBullet(Transform * _from, int _yDir) {
	auto b = new Bullet(_yDir, baseShader);
	childTransform->addChild(b)
		->translate(_from->getTranslationVector(), false);
	bullets.push_back(b);
}

void MY_Scene_Main::updateBulletDisplay() {
	while(player1->bullets < p1Bullets->children.size()) {
		p1Bullets->removeChild(p1Bullets->children.back());
		emitBullet(player1->firstParent(), 1);
	}

	while(player2->bullets < p2Bullets->children.size()) {
		p2Bullets->removeChild(p2Bullets->children.back());
		emitBullet(player2->firstParent(), -1);
	}

	int i = player1->bullets - 1;
	while(player1->bullets > p1Bullets->children.size()) {
		auto b = new MeshEntity(MeshFactory::getPlaneMesh(), baseShader);
		p1Bullets->addChild(b)
			->translate(4 + i * 2, 3, 0);
		++i;
	}

	i = player2->bullets - 1;
	while(player2->bullets > p2Bullets->children.size()) {
		auto b = new MeshEntity(MeshFactory::getPlaneMesh(), baseShader);
		p2Bullets->addChild(b)
			->translate(57 + i * 2, 61, 0);
		++i;
	}
}

void MY_Scene_Main::unload(){
	screenFBO->unload();
	screenSurface->unload();

	MY_Scene_Base::unload();
}