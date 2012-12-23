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

std::unique_ptr<GameplayObject> CommandGeneratePowerup::factory(CommandGeneratePowerup::PowerUps type, SDL_Rect *rect, SDL_Rect *pos, int param)
{
	switch (type) {
		case CommandGeneratePowerup::PowerUps::TypeHealth:
			return std::unique_ptr<GameplayObject>(new HealthPowerUp(Main::graphics->powerups, rect, pos, param));
			break;
		case CommandGeneratePowerup::PowerUps::TypeAmmo:
			return std::unique_ptr<GameplayObject>(new AmmoPowerUp(Main::graphics->powerups, rect, pos, param));
			break;
		case CommandGeneratePowerup::PowerUps::TypeDoubleDamage:
			return std::unique_ptr<GameplayObject>(new DoubleDamagePowerUp(Main::graphics->powerups, rect, pos, param));
			break;
		case CommandGeneratePowerup::PowerUps::TypeInstantKillBullet:
			return std::unique_ptr<GameplayObject>(new InstantKillBulletPowerUp(Main::graphics->powerups, rect, pos, param));
			break;
		case CommandGeneratePowerup::PowerUps::TypeBomb:
			return std::unique_ptr<GameplayObject>(new BombPowerUp(Main::graphics->powerups, rect, pos, param));
			break;
		case CommandGeneratePowerup::PowerUps::TypeMine:
			return std::unique_ptr<GameplayObject>(new MinePowerUp(Main::graphics->powerups, rect, pos, param));
			break;
		case CommandGeneratePowerup::PowerUps::TypeAirstrike:
			return std::unique_ptr<GameplayObject>(new AirstrikePowerUp(Main::graphics->powerups, rect, pos));
			break;
		case CommandGeneratePowerup::PowerUps::TypeLaserBeam:
			return std::unique_ptr<GameplayObject>(new LaserBeamPowerUp(Main::graphics->powerups, rect, pos));
			break;
		case CommandGeneratePowerup::PowerUps::TypeShield:
			return std::unique_ptr<GameplayObject>(new ShieldPowerUp(Main::graphics->powerups, rect, pos));
			break;
		case CommandGeneratePowerup::PowerUps::TypeRandom:
		default:
			return std::unique_ptr<GameplayObject>(new RandomPowerUp(Main::graphics->powerups, pos));
			break;

	}
}