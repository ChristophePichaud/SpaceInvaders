#include "pch.h"
#include "StringHelpers.h"
#include "Game.h"
#include "EntityManager.h"

const float Game::PlayerSpeed = 100.f;
const sf::Time Game::TimePerFrame = sf::seconds(1.f / 60.f);

Game::Game()
	: mWindow(sf::VideoMode(840, 600), "Space Invaders 1978", sf::Style::Close)
	, mTexture()
	, mPlayer()
	, mFont()
	, mStatisticsText()
	, mStatisticsUpdateTime()
	, mStatisticsNumFrames(0)
	, mIsMovingUp(false)
	, mIsMovingDown(false)
	, mIsMovingRight(false)
	, mIsMovingLeft(false)
{
	mWindow.setFramerateLimit(160);

	if (!mTexture.loadFromFile("Media/Textures/SI_Player.png"))
	{
		// Handle loading error
	}

	mPlayer.setTexture(mTexture);
	mPlayer.setPosition(100.f, 500.f);

	std::shared_ptr<Entity> player = std::make_shared<Entity>();
	player->m_sprite = mPlayer;
	player->m_type = EntityType::player;
	player->m_size = mTexture.getSize();
	player->m_position = mPlayer.getPosition();
	EntityManager::m_Entities.push_back(player);

	if (!_TextureEnemy.loadFromFile("Media/Textures/SI_Enemy.png"))
	{
		// Handle loading error
	}

	for (int i = 0; i < SPRITE_COUNT_X; i++)
	{
		for (int j = 0; j < SPRITE_COUNT_Y; j++)
		{
			_Enemy[i][j].setTexture(_TextureEnemy);
			_Enemy[i][j].setPosition(100.f + 50.f * (i + 1), 0.f + 50.f * (j + 1));
			
			std::shared_ptr<Entity> se = std::make_shared<Entity>();
			se->m_sprite = _Enemy[i][j];
			se->m_type = EntityType::enemy;
			se->m_size = _TextureEnemy.getSize();
			se->m_position = _Enemy[i][j].getPosition();
			EntityManager::m_Entities.push_back(se);
		}
	}

	if (!_TextureBlock.loadFromFile("Media/Textures/SI_Block.png"))
	{
		// Handle loading error
	}

	for (int i = 0; i < BLOCK_COUNT; i++)
	{
		_Block[i].setTexture(_TextureBlock);
		_Block[i].setPosition(0.f + 150.f * (i + 1), 350.f);

		std::shared_ptr<Entity> sb = std::make_shared<Entity>();
		sb->m_sprite = _Block[i];
		sb->m_type = EntityType::block;
		sb->m_size = _TextureBlock.getSize();
		sb->m_position = _Block[i].getPosition();
		EntityManager::m_Entities.push_back(sb);
	}

	mFont.loadFromFile("Media/Sansation.ttf");
	mStatisticsText.setFont(mFont);
	mStatisticsText.setPosition(5.f, 5.f);
	mStatisticsText.setCharacterSize(10);
}

void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (mWindow.isOpen())
	{
		sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;

			processEvents();
			update(TimePerFrame);
		}

		updateStatistics(elapsedTime);
		render();
	}
}

void Game::processEvents()
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			handlePlayerInput(event.key.code, true);
			break;

		case sf::Event::KeyReleased:
			handlePlayerInput(event.key.code, false);
			break;

		case sf::Event::Closed:
			mWindow.close();
			break;
		}
	}
}

void Game::update(sf::Time elapsedTime)
{
	sf::Vector2f movement(0.f, 0.f);
	if (mIsMovingUp)
		movement.y -= PlayerSpeed;
	if (mIsMovingDown)
		movement.y += PlayerSpeed;
	if (mIsMovingLeft)
		movement.x -= PlayerSpeed;
	if (mIsMovingRight)
		movement.x += PlayerSpeed;

	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::player)
		{
			continue;
		}

		entity->m_sprite.move(movement * elapsedTime.asSeconds());
	}
}

void Game::render()
{
	mWindow.clear();

	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		mWindow.draw(entity->m_sprite);
	}

	mWindow.draw(mStatisticsText);
	mWindow.draw(mText);
	mWindow.display();
}

void Game::updateStatistics(sf::Time elapsedTime)
{
	mStatisticsUpdateTime += elapsedTime;
	mStatisticsNumFrames += 1;

	if (mStatisticsUpdateTime >= sf::seconds(1.0f))
	{
		mStatisticsText.setString(
			"Frames / Second = " + toString(mStatisticsNumFrames) + "\n" +
			"Time / Update = " + toString(mStatisticsUpdateTime.asMicroseconds() / mStatisticsNumFrames) + "us");

		mStatisticsUpdateTime -= sf::seconds(1.0f);
		mStatisticsNumFrames = 0;
	}

	//
	// Handle collision
	//

	if (mStatisticsUpdateTime >= sf::seconds(0.050f))
	{
		// Game Over ?
		int count = std::count_if(EntityManager::m_Entities.begin(), EntityManager::m_Entities.end(), [](std::shared_ptr<Entity> element) {
			if (element->m_type == EntityType::enemy)
			{
				if (element->m_enabled == false)
				{
					return true;
				}
			}
			return false;
		});

		//if (count >= (5))
		if (count == (SPRITE_COUNT_X * SPRITE_COUNT_Y))
		{
			mText.setFont(mFont);
			mText.setPosition(300.f, 300.f);
			mText.setCharacterSize(40);
			mText.setString("GAME OVER");
		}

		// Handle collision weapon enemies

		for (std::shared_ptr<Entity> weapon : EntityManager::m_Entities)
		{
			if (weapon->m_enabled == false)
			{
				continue;
			}

			if (weapon->m_type != EntityType::weapon)
			{
				continue;
			}

			for (std::shared_ptr<Entity> enemy : EntityManager::m_Entities)
			{
				if (enemy->m_type != EntityType::enemy)
				{
					continue;
				}

				if (enemy->m_enabled == false)
				{
					continue;
				}

				sf::FloatRect boundWeapon;
				boundWeapon = weapon->m_sprite.getGlobalBounds();

				sf::FloatRect boundEnemy;
				boundEnemy = enemy->m_sprite.getGlobalBounds();

				if (boundWeapon.intersects(boundEnemy) == true)
				{
					enemy->m_enabled = false;
					weapon->m_enabled = false;
					//break;
					goto end;
				}
			}
		}

		end:

		// Handle collision weapon blocks

		for (std::shared_ptr<Entity> weapon : EntityManager::m_Entities)
		{
			if (weapon->m_enabled == false)
			{
				continue;
			}

			if (weapon->m_type != EntityType::weapon)
			{
				continue;
			}

			for (std::shared_ptr<Entity> block : EntityManager::m_Entities)
			{
				if (block->m_type != EntityType::block)
				{
					continue;
				}

				if (block->m_enabled == false)
				{
					continue;
				}

				sf::FloatRect boundWeapon;
				boundWeapon = weapon->m_sprite.getGlobalBounds();

				sf::FloatRect boundBlock;
				boundBlock = block->m_sprite.getGlobalBounds();

				if (boundWeapon.intersects(boundBlock) == true)
				{
					weapon->m_enabled = false;
					//break;
					goto end2;
				}
			}
		}

	end2:

		//
		// Handle Weapon moves
		//

		for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
		{
			if (entity->m_enabled == false)
			{
				continue;
			}

			if (entity->m_type != EntityType::weapon)
			{
				continue;
			}

			float x, y;
			x = entity->m_sprite.getPosition().x;
			y = entity->m_sprite.getPosition().y;
			y--;

			if (y <= 0)
			{
				entity->m_enabled = false;
			}
			else
			{
				entity->m_sprite.setPosition(x, y);
			}
		}

		//
		// Handle Enemy moves
		//

		for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
		{
			if (entity->m_enabled == false)
			{
				continue;
			}

			if (entity->m_type != EntityType::enemy)
			{
				continue;
			}

			float x, y;
			x = entity->m_sprite.getPosition().x;
			y = entity->m_sprite.getPosition().y;
		
			if (entity->m_bLeftToRight == true)
				x++;
			else
				x--;
			entity->m_times++;

			if (entity->m_times>=100)
			{
				if (entity->m_bLeftToRight == true)
				{
					entity->m_bLeftToRight = false;
					entity->m_times = 0;
				}
				else
				{
					entity->m_bLeftToRight = true;
					entity->m_times = 0;
				}
			}
			
			entity->m_sprite.setPosition(x, y);
		}

	}
}

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{
	if (key == sf::Keyboard::Up)
		mIsMovingUp = isPressed;
	else if (key == sf::Keyboard::Down)
		mIsMovingDown = isPressed;
	else if (key == sf::Keyboard::Left)
		mIsMovingLeft = isPressed;
	else if (key == sf::Keyboard::Right)
		mIsMovingRight = isPressed;

	if (key == sf::Keyboard::Space)
	{
		if (isPressed == false)
		{
			return;
		}

		std::shared_ptr<Entity> sw = std::make_shared<Entity>();
		sf::Texture texture;
		texture.loadFromFile("Media/Textures/SI_Weapon.png");	
		sw->m_sprite.setTexture(texture);
		sw->m_sprite.setPosition(
			EntityManager::GetPlayer()->m_sprite.getPosition().x + EntityManager::GetPlayer()->m_sprite.getTexture()->getSize().x / 2, 
			EntityManager::GetPlayer()->m_sprite.getPosition().y - 10);
		sw->m_type = EntityType::weapon;
		sw->m_size = sw->m_sprite.getTexture()->getSize();
		EntityManager::m_Entities.push_back(sw);
	}
}
