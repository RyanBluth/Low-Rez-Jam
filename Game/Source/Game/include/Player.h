#pragma once

#include <Sprite.h>
#include <Keyboard.h>
#include <Rectangle.h>

typedef int KeyCode;

enum PlayerNum {
	PLAYER_1,
	PLAYER_2
};

class Player : public Sprite{
public:

	KeyCode up;
	KeyCode down;
	KeyCode left;
	KeyCode right;
	KeyCode shootC;
	KeyCode reloadC;

	Keyboard * const keyboard;

	PlayerNum pnum;

	sweet::Rectangle bbox;

	bool hit;

	float movX;
	float movY;

	int bullets;
	int gorgeLimitUp;
	int gorgeLimitDown;

	int reloadingTimer;

	explicit Player(PlayerNum _pnum, Shader * _shader);

	virtual void update(Step * _step) override;

	void shoot();
	void addBullet();
	void reload();
};