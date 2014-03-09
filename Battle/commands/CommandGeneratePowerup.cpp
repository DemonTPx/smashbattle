#include <SDL/SDL.h>

#include "CommandGeneratePowerup.h"

#include "HealthPowerUp.h"
#include "AmmoPowerUp.h"
#include "DoubleDamagePowerUp.h"
#include "InstantKillBulletPowerUp.h"
#include "BombPowerUp.h"
#include "MinePowerUp.h"
#include "AirstrikePowerUp.h"
#include "LaserBeamPowerUp.h"
#include "ShieldPowerUp.h"
#include "RandomPowerUp.h"

namespace network {

std::unique_ptr<GameplayObject> CommandGeneratePowerup::factory(
	Main &main,
	CommandGeneratePowerup::PowerUps type,
	short powerupid,
	SDL_Rect *rect,
	SDL_Rect *pos,
	int param
){
	std::unique_ptr<GameplayObject> powerup;

	switch (type) {
		case CommandGeneratePowerup::PowerUps::TypeHealth:
			powerup = std::unique_ptr<GameplayObject>(new HealthPowerUp(main.graphics->powerups, rect, pos, param, main));
			break;
		case CommandGeneratePowerup::PowerUps::TypeAmmo:
			powerup = std::unique_ptr<GameplayObject>(new AmmoPowerUp(main.graphics->powerups, rect, pos, param, main));
			break;
		case CommandGeneratePowerup::PowerUps::TypeDoubleDamage:
			powerup = std::unique_ptr<GameplayObject>(new DoubleDamagePowerUp(main.graphics->powerups, rect, pos, param, main));
			break;
		case CommandGeneratePowerup::PowerUps::TypeInstantKillBullet:
			powerup = std::unique_ptr<GameplayObject>(new InstantKillBulletPowerUp(main.graphics->powerups, rect, pos, param, main));
			break;
		case CommandGeneratePowerup::PowerUps::TypeBomb:
			powerup = std::unique_ptr<GameplayObject>(new BombPowerUp(main.graphics->powerups, rect, pos, param, main));
			break;
		case CommandGeneratePowerup::PowerUps::TypeMine:
			powerup = std::unique_ptr<GameplayObject>(new MinePowerUp(main.graphics->powerups, rect, pos, param, main));
			break;
		case CommandGeneratePowerup::PowerUps::TypeAirstrike:
			powerup = std::unique_ptr<GameplayObject>(new AirstrikePowerUp(main.graphics->powerups, rect, pos, main));
			break;
		case CommandGeneratePowerup::PowerUps::TypeLaserBeam:
			powerup = std::unique_ptr<GameplayObject>(new LaserBeamPowerUp(main.graphics->powerups, rect, pos, main));
			break;
		case CommandGeneratePowerup::PowerUps::TypeShield:
			powerup = std::unique_ptr<GameplayObject>(new ShieldPowerUp(main.graphics->powerups, rect, pos, main));
			break;
		case CommandGeneratePowerup::PowerUps::TypeRandom:
		default:
			powerup = std::unique_ptr<GameplayObject>(new RandomPowerUp(main.graphics->powerups, pos, main));
			break;
	}

	powerup->set_id(powerupid);
	return powerup;
}

}