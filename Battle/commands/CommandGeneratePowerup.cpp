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

std::unique_ptr<GameplayObject> CommandGeneratePowerup::factory(CommandGeneratePowerup::PowerUps type, short powerupid, SDL_Rect *rect, SDL_Rect *pos, int param)
{
	std::unique_ptr<GameplayObject> powerup;

	switch (type) {
		case CommandGeneratePowerup::PowerUps::TypeHealth:
			powerup = std::unique_ptr<GameplayObject>(new HealthPowerUp(Main::graphics->powerups, rect, pos, param));
			break;
		case CommandGeneratePowerup::PowerUps::TypeAmmo:
			powerup = std::unique_ptr<GameplayObject>(new AmmoPowerUp(Main::graphics->powerups, rect, pos, param));
			break;
		case CommandGeneratePowerup::PowerUps::TypeDoubleDamage:
			powerup = std::unique_ptr<GameplayObject>(new DoubleDamagePowerUp(Main::graphics->powerups, rect, pos, param));
			break;
		case CommandGeneratePowerup::PowerUps::TypeInstantKillBullet:
			powerup = std::unique_ptr<GameplayObject>(new InstantKillBulletPowerUp(Main::graphics->powerups, rect, pos, param));
			break;
		case CommandGeneratePowerup::PowerUps::TypeBomb:
			powerup = std::unique_ptr<GameplayObject>(new BombPowerUp(Main::graphics->powerups, rect, pos, param));
			break;
		case CommandGeneratePowerup::PowerUps::TypeMine:
			powerup = std::unique_ptr<GameplayObject>(new MinePowerUp(Main::graphics->powerups, rect, pos, param));
			break;
		case CommandGeneratePowerup::PowerUps::TypeAirstrike:
			powerup = std::unique_ptr<GameplayObject>(new AirstrikePowerUp(Main::graphics->powerups, rect, pos));
			break;
		case CommandGeneratePowerup::PowerUps::TypeLaserBeam:
			powerup = std::unique_ptr<GameplayObject>(new LaserBeamPowerUp(Main::graphics->powerups, rect, pos));
			break;
		case CommandGeneratePowerup::PowerUps::TypeShield:
			powerup = std::unique_ptr<GameplayObject>(new ShieldPowerUp(Main::graphics->powerups, rect, pos));
			break;
		case CommandGeneratePowerup::PowerUps::TypeRandom:
		default:
			powerup = std::unique_ptr<GameplayObject>(new RandomPowerUp(Main::graphics->powerups, pos));
			break;
	}

	powerup->set_id(powerupid);
	return powerup;
}