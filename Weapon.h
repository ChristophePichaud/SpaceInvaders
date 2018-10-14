#pragma once
class Weapon
{
public:
	Weapon();
	~Weapon();

public:
	sf::Texture	_texture;
	sf::Sprite	_sprite;
	bool		_enabled = false;
};

