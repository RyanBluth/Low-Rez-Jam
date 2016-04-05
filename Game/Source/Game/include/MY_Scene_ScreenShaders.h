#pragma once

#include <MY_Scene_Base.h>
#include "Player.h"
#include "Bullet.h"
#include "Prop.h"

class RenderSurface;
class StandardFrameBuffer;

class MY_Scene_ScreenShaders : public MY_Scene_Base{
public:

	MeshEntity * drawText;
	MeshEntity * redText;
	MeshEntity * blueText;

	MeshEntity * menu;

	Shader * screenSurfaceShader;
	RenderSurface * screenSurface;
	StandardFrameBuffer * screenFBO;

	MeshEntity * renderPlane;
	FBOTexture * renderTexture;

	OrthographicCamera * orthoCam;

	Player * player1;
	Player * player2;

	Transform * p1Bullets;
	Transform * p2Bullets;

	std::vector<Bullet *> bullets;
	std::vector<Prop *> props;

	virtual void update(Step * _step) override;
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;

	virtual void unload() override;
	virtual void load() override;

	virtual void emitBullet(Transform * _from, int _yDir);
	virtual void updateBulletDisplay();

	MY_Scene_ScreenShaders(Game * _game, bool _showMenu);
	~MY_Scene_ScreenShaders();
};