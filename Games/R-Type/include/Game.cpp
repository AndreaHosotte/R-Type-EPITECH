/*
** EPITECH PROJECT, 2023
** B-CPP-500-MPL-5-1-rtype-nils.colire
** File description:
** Game
*/

#include "Game.hpp"
#include "boost/thread.hpp"

Game::Game(ECS::Engine &engine, int port) : _engine(engine), _port(port)
{
    std::string ip = "127.0.0.1:" + std::to_string(_port);

    _game = {
        .stage = 1,
        .players = {},
        .mobs = {},
        .bullets = {},
    };

    _player.ip = ip;
    _player.health = 100;
    _player.x = 150;
    _player.y = 500;
    _score = 0;
    std::thread s([&]
                  { server_receiver(_port); });
    s.detach();
}

Game::~Game()
{
}

void Game::InitBackgrounds()
{
    _background = _engine.CreateEntity();
    std::cout << "[BACKGROUND] Creating background with id " << _background << std::endl;

    _engine.AddComponent(
        _background,
        ECS::Renderer2D{
            .textureName = "background.png",
            .tint = WHITE,
            .zIndex = 1});

    _engine.AddComponent(
        _background,
        ECS::Transform2D{
            .position = {0, 0},
            .rotation = 0,
            .scale = {1, 1}});
}

// custom position & return id of entity
ECS::Entity Game::CreateAlly(int x, int y, int uid)
{
    ECS::Entity ship = _engine.CreateCustomEntity(uid);

    _engine.AddComponent(
        ship,
        ECS::EntityData{
            .HP = 100,
            .LVL = 1});
    _engine.AddComponent(
        ship,
        ECS::Collision2D{
            .tag = "player"
        });
    _engine.AddComponent(
        ship,
        ECS::Renderer2D{
            .textureName = "base_ship.png",
            .tint = GREEN,
            .zIndex = -1});
    _engine.AddComponent(
        ship,
        ECS::Transform2D{
            .position = {150, 50},
            .rotation = 0,
            .scale = {3, 3}});
    return uid;
}

void Game::CreateBoss(int x, int y, int uid)
{
    _boss = _engine.CreateCustomEntity(uid);

    _engine.AddComponent(
        _boss,
        ECS::EntityData{
            .HP = 1000,
            .LVL = 1});

    _engine.AddComponent(
        _boss,
        ECS::Renderer2D {
            .textureName = "realboss.png",
            .zIndex = -1,
            .isAnimated = true,
            .nbFrames = 5,
            .framesSpeed = 10,
            .loopAnimation = false,
            .animateOnStart = false
        });

    _engine.AddComponent(
        _boss,
        ECS::Transform2D{
            .position = {
                static_cast<float>(x),
                static_cast<float>(y)},
            .rotation = 0,
            .scale = {5, 5}});

    _engine.AddComponent(
        _boss,
        ECS::Collision2D{
            .tag = "enemy"});
}

void Game::CreateEnnemy(int x, int y, int uid)
{
    _enemy = _engine.CreateCustomEntity(uid);

    // get random color
    std::vector<Color> colors = {RED, BLUE, GREEN, YELLOW, PURPLE, ORANGE, PINK};
    int random = rand() % colors.size();
    Color color = colors[random];

    _engine.AddComponent(
        _enemy,
        ECS::EntityData{
            .HP = 100,
            .LVL = 1});
    _engine.AddComponent(
        _enemy,
        ECS::Renderer2D{
            .textureName = "enemy.png",
            .tint = WHITE,
            .zIndex = -1});

    _engine.AddComponent(
        _enemy,
        ECS::Transform2D{
            .position = {
                static_cast<float>(x),
                static_cast<float>(y)},
            .rotation = 0,
            .scale = {5, 5}});

    _engine.AddComponent(
        _enemy,
        ECS::Collision2D{
            .debug = false,
            .tag = "enemy"});
}

void Game::InitHud()
{
    _scoreText = _engine.CreateEntity();
    auto bar = _engine.CreateEntity();
    _healthBar = _engine.CreateEntity();
    _score = 0;

    _engine.AddComponent(
        _scoreText,
        ECS::Text{
            .text = "Score 0",
            .fontName = "r-type_font.ttf",
            .position = {30, 90},
            .fontSize = 50,
            .color = WHITE,
        });

    _engine.AddComponent(
        bar,
        ECS::Renderer2D{
            .textureName = "health_bar1.png",
            .tint = WHITE,
            .zIndex = -100});

    _engine.AddComponent(
        bar,
        ECS::Transform2D{
            .position = {30, 30},
            .rotation = 0,
            .scale = {1, 1}});

    _engine.AddComponent(
        _healthBar,
        ECS::Renderer2D{
            .textureName = "health_bar2.png",
            .tint = WHITE,
            .zIndex = -99});

    _engine.AddComponent(
        _healthBar,
        ECS::Transform2D{
            .position = {30, 30},
            .rotation = 0,
            .scale = {1, 1}});
}

void Game::InitShip()
{
    _ship = _engine.CreateEntity();

    _engine.AddComponent(
        _ship,
        ECS::Collision2D{
            .tag = "player"
        });
    _engine.AddComponent(
        _ship,
        ECS::Renderer2D{
            .textureName = "base_ship.png",
            .tint = WHITE,
            .zIndex = -1});
    _engine.AddComponent(
        _ship,
        ECS::Transform2D{
            .position = {150, 500},
            .rotation = 0,
            .scale = {3, 3}});
    _engine.AddComponent(
        _ship,
        ECS::EntityData{
            .HP = 100,
            .LVL = 1});

    _player.uid = (int)_ship;
    _player.isAlive = true;
    _player.health = 100;
    updatePlayerMap(_ship, _player.ip);
    _game.players.push_back(_player);
    send_game_struct(_game);
}

void Game::InitAudio()
{
    _sound = _engine.CreateEntity();
    _music = _engine.CreateEntity();
    _takeItSound = _engine.CreateEntity();
    _scoreUpSound = _engine.CreateEntity();
    _bossHitSound = _engine.CreateEntity();

    _engine.AddComponent(
        _sound,
        ECS::Audio{
            .audioName = "shoot.mp3",
            .type = ECS::AudioType::SOUND,
        });

    _engine.AddComponent(
        _music,
        ECS::Audio{
            .audioName = "music.mp3",
            .type = ECS::AudioType::MUSIC,
            .loopMusic = true,
            .playOnStart = true});

    _engine.AddComponent(
        _takeItSound,
        ECS::Audio{
            .audioName = "takeit.ogg",
            .type = ECS::AudioType::SOUND});

    _engine.AddComponent(
        _scoreUpSound,
        ECS::Audio{
            .audioName = "scoreup.ogg",
            .type = ECS::AudioType::SOUND});

    _engine.GetComponent<ECS::Audio>(_scoreUpSound).SetVolume(0.5f);

    _engine.AddComponent(
        _bossHitSound,
        ECS::Audio{
            .audioName = "bosshit.ogg",
            .type = ECS::AudioType::SOUND});
}

void Game::Start()
{
    // std::cout << _port << std::endl;
    InitBackgrounds();
    InitShip();
    InitHud();
    std::cout << "\n\nPlayer: " << _player.uid << " ip: " << _player.ip << " x: " << _player.x << " y: " << _player.y + "\n\n"
              << std::endl;
    fillMagazine();

    InitAudio();
    _musicAudio = _engine.GetComponent<ECS::Audio>(_music);
    _musicAudio.SetVolume(0.25f);
}

// ---------------------------------------------------------------------------

// --------------------------- Background parallax ---------------------------

void Game::UpdateBackground()
{
    auto &transform = _engine.GetComponent<ECS::Transform2D>(_background);
    auto &renderer = _engine.GetComponent<ECS::Renderer2D>(_background);

    transform.position.x -= 300 * GetFrameTime();
    if (transform.position.x <= -1920)
    {
        transform.position.x = 0;
    }
}

// ---------------------------------------------------------------------------

// --------------------------- Game mechanics ---------------------------

void Game::MoveShip()
{
    auto &transform = _engine.GetComponent<ECS::Transform2D>(_ship);
    auto &render2D = _engine.GetComponent<ECS::Renderer2D>(_ship);

    transform.position.x += ECS::InputSystem::GetAxis("Horizontal") * 800 * GetFrameTime();
    transform.position.y += ECS::InputSystem::GetAxis("Vertical") * 800 * GetFrameTime();

    if (transform.position.x < 0)
        transform.position.x = 0;
    if (transform.position.x > _engine.GetScreenSize().x - 100)
        transform.position.x = _engine.GetScreenSize().x - 100;

    if (transform.position.y < 0)
        transform.position.y = 0;
    if (transform.position.y > _engine.GetScreenSize().y - 50)
        transform.position.y = _engine.GetScreenSize().y - 50;

    if (transform.position.x != _player.x || transform.position.y != _player.y)
    {
        _player.x = transform.position.x;
        _player.y = transform.position.y;

        for (auto &player : _game.players)
        {
            if (player.uid == _player.uid)
            {
                player.x = _player.x;
                player.y = _player.y;
                send_game_struct(_game);
            }
        }
    }
}

void Game::HitEnemies(ECS::Entity bullet)
{
    auto &bulletHitbox = _engine.GetComponent<ECS::Collision2D>(bullet);
    auto &bulletData = _engine.GetComponent<ECS::EntityData>(bullet);
    auto &bulletTransform = _engine.GetComponent<ECS::Transform2D>(bullet);

    for (auto &mob : _game.mobs)
    {
        auto &entityHitbox = _engine.GetComponent<ECS::Collision2D>(mob.uid);
        auto &entityData = _engine.GetComponent<ECS::EntityData>(mob.uid);
        auto &entityRenderer = _engine.GetComponent<ECS::Renderer2D>(mob.uid);

        if (bulletHitbox.IsColliding(entityHitbox, 0.5f) && entityHitbox.tag == "enemy") {
            std::cout << "Before hit enemy " << mob.uid << " health: " << mob.health << std::endl;
            mob.health -= bulletData.DMG;
            std::cout << "After hit enemy " << mob.uid << " health: " << mob.health << std::endl;
            std::cout << "Bullet hit for " << bulletData.DMG << std::endl;
            HitAnimation(entityRenderer);
            send_game_struct(_game);
        }
    }
}

void Game::HitShip(ECS::Entity bullet)
{
    auto &bulletHitbox = _engine.GetComponent<ECS::Collision2D>(bullet);
    auto &bulletData = _engine.GetComponent<ECS::EntityData>(bullet);
    auto &bulletTransform = _engine.GetComponent<ECS::Transform2D>(bullet);

    for (auto &player : _game.players)
    {
        auto &entityHitbox = _engine.GetComponent<ECS::Collision2D>(player.uid);
        auto &entityData = _engine.GetComponent<ECS::EntityData>(player.uid);

        if (bulletHitbox.IsColliding(entityHitbox, 0.2f) && entityHitbox.tag == "player")
        {
            std::cout << "Before hit player " << player.uid << " health: " << player.health << std::endl;
            player.health -= bulletData.DMG;
            entityData.HP -= bulletData.DMG;
            _engine.GetComponent<ECS::Audio>(_takeItSound).PlayAudio();
            std::cout << "After hit player " << player.uid << " health: " << player.health << std::endl;
            std::cout << "Bullet hit for " << bulletData.DMG << std::endl;
            LoseHealth(bulletData.DMG);

            if (player.health <= 0)
            {
                player.isAlive = false;
                _player.isAlive = false;
            }
            send_game_struct(_game);
}
    }
}

void Game::HitAnimation(ECS::Renderer2D &renderer)
{
    if (!_bossSpawned) renderer.tint = RED;
    else if (_bossSpawned) _engine.GetComponent<ECS::Audio>(_bossHitSound).PlayAudio();
}

void Game::createAllyBullet(ECS::Entity bullet_id, ECS::Entity ally) {
    if (ally == _ship) {
        return;
    }

    ECS::Entity bullet = _engine.CreateCustomEntity(bullet_id);
    // get ally position
    auto &ally_transform = _engine.GetComponent<ECS::Transform2D>(ally);


    _engine.AddComponent(
            bullet,
            ECS::EntityData{
                .DMG = 25,
            });
        _engine.AddComponent(
            bullet,
            ECS::Collision2D{
                .enabled = false,
                .debug = false,
            });
        _engine.AddComponent(
            bullet,
            ECS::Renderer2D{
                .textureName = "bullet.png",
                .tint = WHITE,
                .draw = false,
                .zIndex = -10});
        _engine.AddComponent(
            bullet,
            ECS::Transform2D{
                .position = {ally_transform.position.x + 32, ally_transform.position.y},
                .rotation = 0,
                .scale = {0.3, 0.3}});
        _engine.AddComponent(
            bullet,
            ECS::RigidBody2D{
                .velocity = {0, 0},
                .force = {0, 0},
                .mass = 0.01,
            });
        _engine.AddComponent(
            bullet,
            ECS::Gravity{
                .force = 0,
                .enabled = false});


    auto &bullet_rb = _engine.GetComponent<ECS::RigidBody2D>(bullet);
    auto &bullet_renderer = _engine.GetComponent<ECS::Renderer2D>(bullet);
    auto &bullet_collision = _engine.GetComponent<ECS::Collision2D>(bullet);
    auto &bullet_transform = _engine.GetComponent<ECS::Transform2D>(bullet);
    auto &ship_transform = _engine.GetComponent<ECS::Transform2D>(ally);

    bullet_transform.position = {ship_transform.position.x + 32, ship_transform.position.y};
    bullet_renderer.draw = true;
    bullet_collision.enabled = true;
    bullet_rb.velocity = {0, 0};
    bullet_rb.force.x = 100;
    _allyBullets.push_back(bullet);
}

void Game::ShootBullet()
{
    static int i = 0;
    static int timeout = 0;
    auto bullet = _magazine[i];
    if (ECS::InputSystem::GetKey("SPACE") && _player.isAlive)
    {

        auto &audio = _engine.GetComponent<ECS::Audio>(_sound);
        audio.PlayAudio();

        auto &bullet_rb = _engine.GetComponent<ECS::RigidBody2D>(bullet);
        auto &bullet_renderer = _engine.GetComponent<ECS::Renderer2D>(bullet);
        auto &bullet_collision = _engine.GetComponent<ECS::Collision2D>(bullet);
        auto &bullet_transform = _engine.GetComponent<ECS::Transform2D>(bullet);
        auto &ship_transform = _engine.GetComponent<ECS::Transform2D>(_ship);

        bullet_transform.position = {ship_transform.position.x + 32, ship_transform.position.y};
        bullet_renderer.draw = true;
        bullet_collision.enabled = true;
        bullet_rb.velocity = {0, 0};
        bullet_rb.force.x = 100;
        i += 1;
        if (i == _magazine.size())
            i = 0;

        // create random number & random seed
        srand(time(NULL));
        int random = rand() % 1000;

        _game.bullets.push_back({
            .uid = random,
            .uid_owner = _player.uid,
            .bullet_type = 1,
            .x = static_cast<int>(bullet_transform.position.x),
            .y = static_cast<int>(bullet_transform.position.y),
        });
        send_game_struct(_game);
    }
}

void Game::EnemyShootBullet(ECS::Entity bullet_id, ECS::Entity enemy) {
    ECS::Entity bullet = _engine.CreateCustomEntity(bullet_id);
    auto &ennemy_transform = _engine.GetComponent<ECS::Transform2D>(enemy);
    Vector2 bulletScale = _bossSpawned ? Vector2{0.5, 1} : Vector2{0.3, 0.6};
    unsigned int bulletDamage = _bossSpawned ? 30 : 10;
    _engine.AddComponent(
            bullet,
            ECS::EntityData{
                .DMG = bulletDamage,
            });
        _engine.AddComponent(
            bullet,
            ECS::Collision2D{
                .debug = false,
            });
        _engine.AddComponent(
            bullet,
            ECS::Renderer2D{
                .textureName = "inversed_bullet.png",
                .tint = RED,
                .draw = false,
                .zIndex = -10});
        _engine.AddComponent(
            bullet,
            ECS::Transform2D{
                .position = {ennemy_transform.position.x, ennemy_transform.position.y + 52},
                .rotation = 0,
                .scale = bulletScale});
        _engine.AddComponent(
            bullet,
            ECS::RigidBody2D{
                .velocity = {0, 0},
                .force = {0, 0},
                .mass = 0.01,
            });
        _engine.AddComponent(
            bullet,
            ECS::Gravity{
                .force = 0,
                .enabled = false});

        _engine.AddComponent(
            bullet,
            ECS::Audio{
                .audioName = "enemy_shoot.ogg",
                .type = ECS::AudioType::SOUND,
                .playOnStart = true
            });

    if (_bossSpawned)
        _engine.GetComponent<ECS::Renderer2D>(_boss).animate = true;

    auto &bullet_rb = _engine.GetComponent<ECS::RigidBody2D>(bullet);
    auto &bullet_renderer = _engine.GetComponent<ECS::Renderer2D>(bullet);
    auto &bullet_collision = _engine.GetComponent<ECS::Collision2D>(bullet);
    auto &bullet_transform = _engine.GetComponent<ECS::Transform2D>(bullet);

    bullet_transform.position = {ennemy_transform.position.x + 16, ennemy_transform.position.y + 52};
    bullet_renderer.draw = true;
    bullet_collision.enabled = true;
    bullet_rb.velocity = {0, 0};
    bullet_rb.force.x = -100;
    _enemyBullets.push_back(bullet);
}

void Game::fillMagazine()
{
    auto &transform_ship = _engine.GetComponent<ECS::Transform2D>(_ship);

    for (int i = 0; i < 20; i++)
    {
        auto bullet = _engine.CreateEntity();

        _engine.AddComponent(
            bullet,
            ECS::EntityData{
                .DMG = 50,
            });
        _engine.AddComponent(
            bullet,
            ECS::Collision2D{
                .enabled = false,
                .debug = false,
            });
        _engine.AddComponent(
            bullet,
            ECS::Renderer2D{
                .textureName = "bullet.png",
                .tint = WHITE,
                .draw = false,
                .zIndex = -10});
        _engine.AddComponent(
            bullet,
            ECS::Transform2D{
                .position = {transform_ship.position.x, transform_ship.position.y},
                .rotation = 0,
                .scale = {0.3, 0.3}});
        _engine.AddComponent(
            bullet,
            ECS::RigidBody2D{
                .velocity = {0, 0},
                .force = {0, 0},
                .mass = 0.01,
            });
        _engine.AddComponent(
            bullet,
            ECS::Gravity{
                .force = 0,
                .enabled = false});

        _magazine.push_back(bullet);
    }
}

void Game::UpdateHud()
{
    static auto &text = _engine.GetComponent<ECS::Text>(_scoreText);

    text.text = "Score " + std::to_string(_score);
}

// ----------------------------------------------------------------------

// --------------------------- Game update ---------------------------

void Game::Update()
{
    UpdateHud();
    UpdateBackground();

    static bool loseOnce = true;
    static bool winOnce = true;

    if (!_player.isAlive)
    {
        if (loseOnce) {
            loseOnce = false;
            PlayerDeath();
            _engine.GetComponent<ECS::Renderer2D>(_ship).draw = false;
            _engine.GetComponent<ECS::Collision2D>(_ship).enabled = false;
            _engine.GetComponent<ECS::Audio>(_music).StopAudio();
        } else if (!loseOnce) {
            auto &quitButton = _engine.GetComponent<ECS::Button>(_deathScreen);
            quitButton.OnClick([&]() {
                _engine.Stop();
            });
            quitButton.OnOver();
        }
    }

    if (_player.isAlive) {
        MoveShip();
        ShootBullet();
    }

    for (auto const &bullet : _magazine)
    {
        HitEnemies(bullet);
    }

    for (auto const &bullet : _enemyBullets)
    {
        HitShip(bullet);
    }

    for (auto &mob : _game.mobs)
    {
        // isEntityExists
        if (_engine.EntityExists(mob.uid) == false)
        {
            continue;
        }
        auto &entityHitBox = _engine.GetComponent<ECS::Collision2D>(mob.uid);
        auto &entityData = _engine.GetComponent<ECS::EntityData>(mob.uid);
        auto &entityRenderer = _engine.GetComponent<ECS::Renderer2D>(mob.uid);

        if (mob.health <= 0)
        {
            std::cout << "Enemy dead" << std::endl;
            SpawnExplosion({entityHitBox.rect.x, entityHitBox.rect.y});
            _engine.DestroyEntity(mob.uid);
            mob.isAlive = false;
            _destroyedEntities.push_back(mob.uid);
            _score += 10;
            _engine.GetComponent<ECS::Audio>(_scoreUpSound).PlayAudio();
            if (_bossSpawned) _bossKilled = true;

            // print game struct
            // for (auto const &player : _game.players)
            // {
            //     std::cout << "Player [" << player.uid << "] is connected with ip [" << player.ip << "]" << std::endl;
            // }
            // for (auto const &mob : _game.mobs)
            // {
            //     std::cout << "Mob [" << mob.uid << "] is alive [" << mob.isAlive << "]" << std::endl;
            // }
            // for (auto const &bullet : _game.bullets)
            // {
            //     std::cout << "Bullet [" << bullet.uid << "] is alive [" << bullet.uid_owner << "]" << std::endl;
            // }

            send_game_struct(_game);
        }
    }

     if (_game.stage == 4)
    {
        send_game_struct(_game);
        if (winOnce) {
            winOnce = false;
            PlayerWin();
            _engine.GetComponent<ECS::Audio>(_music).StopAudio();
        } else if (!winOnce) {
            auto &quitButton = _engine.GetComponent<ECS::Button>(_winScreen);
            quitButton.OnClick([&]() {
                _engine.Stop();
            });
            quitButton.OnOver();
        }
    }
}

void Game::LoseHealth(int damage)
{
    auto &barRenderer = _engine.GetComponent<ECS::Renderer2D>(_healthBar);

    static float baseWidth = barRenderer.texture.width;

    barRenderer.texture.width -= (baseWidth / _player.health) * damage;
    if (barRenderer.texture.width <= 0)
        barRenderer.texture.width = 0;
    std::cout << "Health bar renderer width: " << barRenderer.texture.width << std::endl;
}
// --------------------------------------------------------------------

// --------------------------- Network functions ---------------------------

void Game::UpdateEntityPosition(ECS::Entity entity, Vector2 n_pos)
{
    auto &transform = _engine.GetComponent<ECS::Transform2D>(entity);

    transform.position = n_pos;
}

void Game::ReceiveData(game_t received)
{

    _game.stage = received.stage;
    for (auto const &player : received.players)
    {
        auto it = _playerMap.find(player.uid);

        if (it == _playerMap.end() && player.ip != _player.ip && player.uid != _player.uid)
        {
            std::cout << "New player [" << player.uid << "] connected with ip [" << player.ip << "]" << std::endl;
            std::cout << "Creating new player [" << player.uid << "] at position [" << player.x << ", " << player.y << "]" << std::endl;
            ECS::Entity new_player = CreateAlly(player.x, player.y, player.uid);
            updatePlayerMap(new_player, player.ip);
            _game.players.push_back(player);
        }
    }

    // update all _allies position and not the player
    for (auto const &player : received.players)
    {
        // std::cout << "Updating player [" << player.uid << "] position to [" << player.x << ", " << player.y << "]" << std::endl;
        UpdateEntityPosition(player.uid, {static_cast<float>(player.x), static_cast<float>(player.y)});
        // update also the player position in the game struct
        for (auto &p : _game.players)
        {
            if (p.uid == player.uid)
            {
                p.x = player.x;
                p.y = player.y;
            }
        }
    }

    // update all _enemies position
    for (auto const &enemy : received.mobs)
    {
        if (std::find(_destroyedEntities.begin(), _destroyedEntities.end(), enemy.uid) != _destroyedEntities.end())
        {
            continue;
        }
        if (_engine.EntityExists(enemy.uid) == false)
        {
            std::cout << "Creating new enemy [" << enemy.uid << "] at position [" << enemy.x << ", " << enemy.y << "]" << std::endl;
            if (enemy.mob_type == 3) {
                CreateBoss(enemy.x, enemy.y, enemy.uid);
                _bossSpawned = true;
            } else {
                CreateEnnemy(enemy.x, enemy.y, enemy.uid);
            }
            _game.mobs.push_back(enemy);
        }
        else
        {
            UpdateEntityPosition(enemy.uid, {static_cast<float>(enemy.x), static_cast<float>(enemy.y)});
            for (auto &mob : _game.mobs)
            {
                if (mob.uid == enemy.uid)
                {
                    mob.health = enemy.health;
                }
            }
        }
    }

    // create bullets stored in the game struct
    for (auto &bullet : received.bullets)
    {
        if (bulletExists(bullet.uid) == false && bullet.uid_owner != _player.uid)
        {
            if (bullet.bullet_type == 1) {
                std::cout << "Creating new bullet [" << bullet.uid << "] at position [" << bullet.x << ", " << bullet.y << "] by player [" << bullet.uid_owner << "]" << std::endl;
                createAllyBullet(bullet.uid, bullet.uid_owner);
                // delete bullet from game struct
                for (auto it = _game.bullets.begin(); it != _game.bullets.end(); ++it)
                {
                    if (it->uid == bullet.uid)
                    {
                        _game.bullets.erase(it);
                        break;
                    }
                }
            } else {
                std::cout << "Creating new bullet [" << bullet.uid << "] at position [" << bullet.x << ", " << bullet.y << "] by enemy [" << bullet.uid_owner << "]" << std::endl;
                EnemyShootBullet(bullet.uid, bullet.uid_owner);
                // delete bullet from game struct
                for (auto it = _game.bullets.begin(); it != _game.bullets.end(); ++it)
                {
                    if (it->uid == bullet.uid)
                    {
                        _game.bullets.erase(it);
                        break;
                    }
                }
            }
        }
    }
    _game.bullets.clear();
}

bool Game::bulletExists(int uid)
{
    for (auto const &bullet : _allyBullets)
    {
        if (bullet == uid)
        {
            return true;
        }
    }
    for (auto const &bullet : _enemyBullets)
    {
        if (bullet == uid)
        {
            return true;
        }
    }
    return false;
}

void Game::updatePlayerMap(ECS::Entity id, std::string ip)
{
    _playerMap.emplace(id, ip);
    auto it = _playerMap.find(id);
    if (it != _playerMap.end())
    {
        // std::cout << "Player [" << it->first << "] is connected with ip [" << it->second << "]" << std::endl;
    }
}

void Game::server_handle_receive(const boost::system::error_code &error, size_t bytes_transferred)
{
    game_t tmp_game;

    if (error)
    {
        // std::cout << "Receive failed: " << error.message() << "\n";
        return;
    }
    std::string message(recv_buffer.begin(), recv_buffer.begin() + bytes_transferred);
    // deserialize message
    std::istringstream archive_stream(message);
    boost::archive::binary_iarchive archive(archive_stream);
    archive >> tmp_game;

    ReceiveData(tmp_game);
    server_wait();
}

void Game::server_wait()
{
    socket.async_receive_from(boost::asio::buffer(recv_buffer),
                              remote_endpoint,
                              boost::bind(&Game::server_handle_receive, this,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
}

void Game::server_receiver(int port)
{
    // std::cout << "HERE" << std::endl;
    socket.open(udp::v4());
    socket.bind(udp::endpoint(address::from_string("127.0.0.1"), port));

    server_wait();

    std::cout << "\n\nReceiving at " << socket.local_endpoint().address().to_string() << ":" << socket.local_endpoint().port() << "\n\n";
    io_service.run();
    // std::cout << "Receiver exit\n";
}

void Game::send_game_struct(game_t game)
{
    boost::asio::io_service io_service;
    udp::socket socket(io_service);
    udp::endpoint remote_endpoint = udp::endpoint(address::from_string(SERVER_IPADDRESS), SERVER_UDP_PORT);
    socket.open(udp::v4());

    std::ostringstream archive_stream;
    boost::archive::binary_oarchive archive(archive_stream);
    archive << game;
    std::string message = archive_stream.str();
    socket.send_to(boost::asio::buffer(message), remote_endpoint);
    socket.close();
}

void Game::SpawnExplosion(Vector2 pos)
{
    ECS::Entity explosion = _engine.CreateEntity();

    _engine.AddComponent(
        explosion,
        ECS::Renderer2D{
            .textureName = "explosion.png",
            .zIndex = -19,
            .isAnimated = true,
            .nbFrames = 5,
            .framesSpeed = 8,
            .loopAnimation = false,
            .animateOnStart = true,
            .destroyOnAnimationEnd = true
        });

    _engine.AddComponent(
        explosion,
        ECS::Transform2D{
            .position = {pos.x, pos.y},
            .rotation = 0,
            .scale = {2, 2}
        });

    _engine.AddComponent(
        explosion,
        ECS::Audio{
            .audioName = "boom.ogg",
            .type = ECS::AudioType::SOUND,
            .playOnStart = true
        });

}

void Game::PlayerDeath()
{
    ECS::Entity explosion = _engine.CreateEntity();
    _deathScreen = _engine.CreateEntity();
    Vector2 pos = _engine.GetComponent<ECS::Transform2D>(_ship).position;

    _engine.AddComponent(
        explosion,
        ECS::Renderer2D{
            .textureName = "player_death.png",
            .zIndex = -39,
            .isAnimated = true,
            .nbFrames = 6,
            .framesSpeed = 8,
            .loopAnimation = false,
            .animateOnStart = true,
            .destroyOnAnimationEnd = true
        });

    _engine.AddComponent(
        explosion,
        ECS::Transform2D{
            .position = {pos.x, pos.y},
            .rotation = 0,
            .scale = {3, 3}
        });

    _engine.AddComponent(
        explosion,
        ECS::Audio{
            .audioName = "player_death.ogg",
            .type = ECS::AudioType::SOUND,
            .playOnStart = true
        });

    _engine.AddComponent(
        _deathScreen,
        ECS::Renderer2D{
            .textureName = "lol.png",
            .tint = {0, 0, 0, 100},
            .zIndex = -100,
        });

    _engine.AddComponent(
        _deathScreen,
        ECS::Transform2D{
            .position = {0, 0},
            .rotation = 0,
            .scale = {1, 1}
        });

    _engine.AddComponent(
        _deathScreen,
        ECS::Text{
            .text = "GAME OVER",
            .fontName = "r-type_font.ttf",
            .position = {(float)GetScreenWidth() / 2 - 400, 200},
            .fontSize = 150,
            .color = RED,
        });

    _engine.AddComponent(
        _deathScreen,
        ECS::Button{
            .position = {(float)GetScreenWidth() / 2 - 200, 600},
            .width = 300,
            .height = 100,
            .buttonText = {
                .text = "QUIT GAME",
                .fontName = "r-type_font.ttf",
                .fontSize = 50,
                .color = BLACK
            },
            .baseColor = WHITE,
            .overColor = GRAY,
            .clickColor = DARKGRAY,
            .rounded = true,
            .roundness = 0.3f,
        });

    _engine.AddComponent(
        _deathScreen,
        ECS::Audio{
            .audioName = "lose.ogg",
            .type = ECS::AudioType::SOUND,
            .playOnStart = true
        });
}

void Game::PlayerWin()
{
    _winScreen = _engine.CreateEntity();

    _engine.AddComponent(
        _winScreen,
        ECS::Renderer2D{
            .textureName = "lol.png",
            .tint = {0, 0, 0, 100},
            .zIndex = -100,
        });

    _engine.AddComponent(
        _winScreen,
        ECS::Transform2D{
            .position = {0, 0},
            .rotation = 0,
            .scale = {1, 1}
        });

    _engine.AddComponent(
        _winScreen,
        ECS::Text{
            .text = "YOU WIN!",
            .fontName = "r-type_font.ttf",
            .position = {(float)GetScreenWidth() / 2 - 300, 200},
            .fontSize = 150,
            .color = YELLOW,
        });

    _engine.AddComponent(
        _winScreen,
        ECS::Button{
            .position = {(float)GetScreenWidth() / 2 - 200, 600},
            .width = 300,
            .height = 100,
            .buttonText = {
                .text = "QUIT GAME",
                .fontName = "r-type_font.ttf",
                .fontSize = 50,
                .color = BLACK
            },
            .baseColor = WHITE,
            .overColor = GRAY,
            .clickColor = DARKGRAY,
            .rounded = true,
            .roundness = 0.3f,
        });

    _engine.AddComponent(
        _winScreen,
        ECS::Audio{
            .audioName = "win.ogg",
            .type = ECS::AudioType::SOUND,
            .playOnStart = true
        });
}
