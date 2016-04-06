#include <Player.h>
#include <MY_ResourceManager.h>

#define SPEED 0.5f
#define HEIGHT 10.f
#define HALF_HEIGHT HEIGHT * 0.5f
#define WIDTH 4.f
#define HALF_WIDTH WIDTH * 0.5f

Player::Player(PlayerNum _pnum, Shader * _shader) :
	Sprite(_shader),
	keyboard(&Keyboard::getInstance()),
	pnum(_pnum),
	bbox(sweet::Rectangle(0, 0, 3, 9)),
	hit(false),
	movX(0.f),
	movY(0.f),
	bullets(3),
	reloadingTimer(0.f)
{
	mesh->setScaleMode(GL_NEAREST);

	if(pnum == PLAYER_1) {
		setPrimaryTexture(MY_ResourceManager::globalAssets->getTexture("p1")->texture);
		up	   =  GLFW_KEY_W;
		down   =  GLFW_KEY_S;
		left   =  GLFW_KEY_A;
		right  =  GLFW_KEY_D;
		shootC =  GLFW_KEY_C;
		reloadC = GLFW_KEY_R;
		gorgeLimitUp = 26;
		gorgeLimitDown = -999;
	}else {
		setPrimaryTexture(MY_ResourceManager::globalAssets->getTexture("p2")->texture);
		up	    = GLFW_KEY_UP;
		down    = GLFW_KEY_DOWN;
		left    = GLFW_KEY_LEFT;
		right   = GLFW_KEY_RIGHT;
		shootC  = GLFW_KEY_SLASH;
		reloadC = GLFW_KEY_L;
		gorgeLimitUp = 999;
		gorgeLimitDown = 40;
	}
	meshTransform->scale(10.f);
	freezeTransformation();
}

void Player::update(Step* _step) {
	if(active){
		if(reloadingTimer < 0.005f){

			glm::vec3 worldPos = getWorldPos();

			if(keyboard->keyDown(up) && worldPos.y < 63 - HALF_HEIGHT && worldPos.y < gorgeLimitUp) {
				movY += SPEED;
				if(movY >= 1.f){
					firstParent()->translate(0.f, 1.f, 0.f);
					movY = 0.f;
				}
			}
			if(keyboard->keyDown(down) && worldPos.y > 1 + HALF_HEIGHT  && worldPos.y > gorgeLimitDown) {
				movY -= SPEED;
				if(movY <= -1.f){
					firstParent()->translate(0.f, -1.f, 0.f);
					movY = 0.f;
				}
			}
			if(keyboard->keyDown(left) && worldPos.x > 1 + HALF_WIDTH) {
				movX -= SPEED;
				if(movX <= -1.f){
					firstParent()->translate(-1.f, 0.f, 0.f);
					movX = 0.f;
				}
			}
			if(keyboard->keyDown(right) && worldPos.x < 63 - HALF_WIDTH) {
				movX += SPEED;
				if(movX >= 1.f){
					firstParent()->translate(1.f, 0.f, 0.f);
					movX = 0.f;
				}
			}
			if(bullets > 0 && keyboard->keyJustDown(shootC)) {
				shoot();
			}
			if(keyboard->keyJustDown(reloadC)) {
				reload();
			}
		}else {
			reloadingTimer -= _step->deltaTime;
			if(reloadingTimer <= 0.f) {
				addBullet();
			}
		}
		worldPos = getWorldPos();

		bbox.x = worldPos.x - bbox.width/2;
		bbox.y = worldPos.y - bbox.height/2;
	}
	Sprite::update(_step);
}

void Player::shoot() {
	--bullets;
	MY_ResourceManager::globalAssets->getAudio("shoot")->sound->play();
}

void Player::addBullet() {
	bullets++;
	reload();
	MY_ResourceManager::globalAssets->getAudio("reload")->sound->play();
}

void Player::reload() {
	if(bullets < 3){
		reloadingTimer = 30.f;
	}
}

void Player::die(){
	if(!hit){
		hit = true;
		active = false;
		if(pnum == PLAYER_1) {
			setPrimaryTexture(MY_ResourceManager::globalAssets->getTexture("p1_dead")->texture);
		}else {
			setPrimaryTexture(MY_ResourceManager::globalAssets->getTexture("p2_dead")->texture);
		}
		MY_ResourceManager::globalAssets->getAudio("die")->sound->play();
		meshTransform->scale(10.f);
		freezeTransformation();
	}
}
