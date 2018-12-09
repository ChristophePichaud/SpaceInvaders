#pragma once
#include "Weapon.h"

#define SPRITE_COUNT_X 11
#define SPRITE_COUNT_Y 5
#define BLOCK_COUNT 4

class Game
{
public:
	Game();
	~Game() { };
	void run();

private:
	void processEvents();
	void update(sf::Time elapsedTime);
	void render();

	void updateStatistics(sf::Time elapsedTime);
	void HandleCollisionEnemyMasterWeaponPlayer();
	void HanldeEnemyMasterWeaponMoves();
	void HandleEnemyMasterWeaponFiring();
	void HandleCollisionEnemyMasterWeaponBlock();
	void HandleEnemyMasterMove();
	void HandleCollisionEnemyWeaponBlock();
	void HandleCollisionWeaponPlayer();
	void HanldeEnemyWeaponMoves();
	void HandleEnemyWeaponFiring();
	void HandleCollisionBlockEnemy();
	void HandleEnemyMoves();
	void HanldeWeaponMoves();
	void HandleCollisionWeaponBlock();
	void HandleCollisionWeaponEnemy();
	void HandleGameOver();
	void GameOver();
	void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);

private:
	static const float		PlayerSpeed;
	static const sf::Time	TimePerFrame;

	sf::RenderWindow		mWindow;
	sf::Texture	mTexture;
	sf::Sprite	mPlayer;
	sf::Font	mFont;
	sf::Text	mStatisticsText;
	sf::Time	mStatisticsUpdateTime;
	sf::Text	mText;

	std::size_t	mStatisticsNumFrames;
	bool mIsMovingUp;
	bool mIsMovingDown;
	bool mIsMovingRight;
	bool mIsMovingLeft;

	bool _IsGameOver = false;
	bool _IsEnemyWeaponFired = false;
	bool _IsWeaponFired = false;
	bool _IsEnemyMasterWeaponFired = false;

	sf::Texture	_TextureEnemy;
	sf::Sprite	_Enemy[SPRITE_COUNT_X][SPRITE_COUNT_Y];
	sf::Texture	_TextureBlock;
	sf::Sprite	_Block[BLOCK_COUNT];
	sf::Texture	_TextureWeapon;
	sf::Texture	_TextureWeaponEnemy;
	sf::Texture	_TextureWeaponEnemyMaster;
	sf::Sprite	_Weapon;
	sf::Texture	_TextureEnemyMaster;
	sf::Sprite	_EnemyMaster;
};

