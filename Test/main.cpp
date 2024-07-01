/*
** EPITECH PROJECT, 2023
** B-CPP-500-MPL-5-1-rtype-nils.colire
** File description:
** main
*/

#include "../Engine/src/Core/Engine.hpp"

namespace ECS {
    class Menu : public DefaultScene {
        public:
            Menu(Engine &engine) : _engine(engine) {};
            ~Menu() {};

            void Start()
            {
                _background = _engine.CreateEntity();
                _ship = _engine.CreateEntity();
                auto title = _engine.CreateEntity();
                _playButton = _engine.CreateEntity();
                _quitButton = _engine.CreateEntity();
                _sound = _engine.CreateEntity();
                _deadScreen = _engine.CreateEntity();

                _engine.AddComponent(
                    _background,
                    Renderer2D{
                        .textureName = "background.png",
                        .tint = WHITE,
                    });

                _engine.AddComponent(
                    _background,
                    Transform2D{
                        .position = {0, 0},
                        .rotation = 0,
                        .scale = {1, 1}
                    });

                _engine.AddComponent(
                    _ship,
                    Renderer2D{
                        .textureName = "base_ship.png",
                        .tint = WHITE,
                        .zIndex = -1
                    });

                _engine.AddComponent(
                    _ship,
                    Transform2D{
                        .position = {400, 400},
                        .rotation = 0,
                        .scale = {1, 1}
                    });

                _engine.AddComponent(
                    title,
                    Text {
                        .text = "R-Type",
                        .fontName = "test_font.ttf",
                        .position = {600, 0},
                        .fontSize = 100,
                        .color = WHITE
                    });

                _engine.AddComponent(
                    _playButton,
                    Button {
                        .position = {600, 400},
                        .width = 300,
                        .height = 100,
                        .buttonText = {
                            .text = "Play",
                            .fontName = "test_font.ttf",
                            .fontSize = 50,
                            .color = BLACK
                        },
                        .baseColor = WHITE,
                        .overColor = GRAY,
                        .clickColor = DARKGRAY,
                        .rounded = true,
                        .roundness = 0.5f
                    });

                _engine.AddComponent(
                    _quitButton,
                    Button {
                        .position = {600, 600},
                        .width = 300,
                        .height = 100,
                        .buttonText = {
                            .text = "Quit",
                            .fontName = "test_font.ttf",
                            .fontSize = 50,
                            .color = BLACK
                        },
                        .baseColor = WHITE,
                        .overColor = GRAY,
                        .clickColor = DARKGRAY,
                        .rounded = true,
                        .roundness = 0.5f
                    });


                _engine.AddComponent(
                    _sound,
                    Audio{
                        .audioName = "fart.ogg",
                        .type = AudioType::SOUND,
                    });

                _engine.AddComponent(
                    _deadScreen,
                    Image{
                        .fileName = "lol.png",
                        .position = {0, 0},
                        .scale = {1, 1},
                        .tint = {0, 255, 0, 120},
                    });
            };

            void Update()
            {
                auto &play = _engine.GetComponent<Button>(_playButton);
                auto &quit = _engine.GetComponent<Button>(_quitButton);
                play.OnOver();
                quit.OnOver();
                play.OnClick([&]() {
                    StartGame();
                });
                quit.OnClick([&]() {
                    _engine.Stop();
                });

                if (InputSystem::GetKey("Space")) {
                    _engine.GetComponent<Audio>(_sound).PlayAudio();
                }
            };

            void StartGame()
            {
                ClearBackground(BLACK);
                _engine.SwitchScene(1);
            };

        private:
            Engine &_engine;
            Entity _background;
            Entity _ship;
            Entity _playButton;
            Entity _quitButton;
            Entity _sound;
            Entity _deadScreen;
            int _caca;
    };

    class Game : public DefaultScene {
        public:
            Game(Engine &engine) : _engine(engine) {};
            ~Game() {};

            void Start()
            {
                _background = _engine.CreateEntity();
                _enemy = _engine.CreateEntity();
                _ship = _engine.CreateEntity();
                auto enemy2 = _engine.CreateEntity();

                _engine.AddComponent(
                    _background,
                    Renderer2D{
                        .textureName = "background.png",
                        .tint = WHITE,
                        .zIndex = 0
                    });
                
                _engine.AddComponent(
                    _background,
                    Transform2D{
                        .position = {0, 0},
                        .rotation = 0,
                        .scale = {1, 1}
                    });

                _engine.AddComponent(
                    _ship,
                    Renderer2D{
                        .textureName = "base_ship.png",
                        .tint = WHITE,
                        .zIndex = -20
                    });

                _engine.AddComponent(
                    _ship,
                    Transform2D{
                        .position = {150, 50},
                        .rotation = 0,
                        .scale = {3, 3}
                    });

                _engine.AddComponent(
                    _ship,
                    Collision2D{
                        .debug = true,
                        .tag = "player"
                    });

                _engine.AddComponent(
                    _enemy,
                    Renderer2D{
                        .textureName = "realboss.png",
                        .zIndex = -1,
                        .isAnimated = true,
                        .nbFrames = 5,
                        .framesSpeed = 8,
                        .loopAnimation = false,
                        .animateOnStart = true
                    });

                _engine.AddComponent(
                    _enemy,
                    Transform2D{
                        .position = {100, 100},
                        .rotation = 0,
                        .scale = {3, 3}
                    });

                _engine.AddComponent(
                    _enemy,
                    Collision2D{
                        .debug = true,
                        .tag = "enemy"
                    });

                _engine.AddComponent(
                    enemy2,
                    Renderer2D{
                        .textureName = "gragas_sheet.png",
                        .zIndex = -1,
                        .isAnimated = true,
                        .nbFrames = 41,
                        .framesSpeed = 8,
                        .loopAnimation = true,
                        .animateOnStart = true
                    });

                _engine.AddComponent(
                    enemy2,
                    Transform2D{
                        .position = {500, 400},
                        .rotation = 0,
                        .scale = {3, 3}
                    });

                _engine.AddComponent(
                    enemy2,
                    Collision2D{
                        .debug = true,
                        .tag = "enemy"
                    });

                InitAudio();
            };

            void CreateEnemy()
            {
                Entity mob = _engine.CreateEntity();

                _engine.AddComponent(
                    mob,
                    Renderer2D{
                        .textureName = "enemy.png",
                        .zIndex = -12,
                    });

                _engine.AddComponent(
                    mob,
                    Transform2D{
                        .position = {1000, 400},
                        .rotation = 0,
                        .scale = {5, 5}
                    });

                _engine.AddComponent(
                    mob,
                    Collision2D{
                        .debug = true,
                        .tag = "enemy"
                    });
            }

            void Update()
            {
                MoveShip();

                if (InputSystem::GetKey("P"))
                    GoBackMenu();

                if (InputSystem::GetKey("SPACE")) {
                    _engine.GetComponent<Audio>(_sound).PlayAudio();
                    CreateEntity();
                }

                if (InputSystem::GetKey("M")) {
                    _engine.GetComponent<Audio>(_music).StopAudio();
                    _engine.GetComponent<Audio>(_music).PlayAudio();
                }
            };

            void InitAudio()
            {
                _sound = _engine.CreateEntity();
                _music = _engine.CreateEntity();

                _engine.AddComponent(
                    _sound,
                    Audio{
                        .audioName = "player_death.ogg",
                        .type = AudioType::SOUND,
                    });
                _engine.AddComponent(
                    _music,
                    Audio{
                        .audioName = "bomba.mp3",
                        .type = AudioType::MUSIC,
                        .loopMusic = true,
                        .playOnStart = true
                    });
                _engine.GetComponent<Audio>(_music).SetVolume(0.1f);
            }


            void CreateEntity()
            {
                explosion = _engine.CreateEntity();
                auto &ship_transform = _engine.GetComponent<Transform2D>(_ship);

                _engine.AddComponent(
                    explosion,
                    Renderer2D{
                        .textureName = "player_death.png",
                        .zIndex = -30,
                        .isAnimated = true,
                        .nbFrames = 6,
                        .framesSpeed = 8,
                        .loopAnimation = false,
                        .animateOnStart = true,
                        .destroyOnAnimationEnd = true
                    });

                _engine.AddComponent(
                    explosion,
                    Transform2D{
                        .position = {ship_transform.position.x, ship_transform.position.y},
                        .rotation = 0,
                        .scale = {3, 3}
                    });
            }

            void MoveShip()
            {
                auto &transform = _engine.GetComponent<Transform2D>(_ship);
                auto &collision = _engine.GetComponent<Collision2D>(_ship);
                auto &enemyCollision = _engine.GetComponent<Collision2D>(_enemy);

                transform.position.x += InputSystem::GetAxis("Horizontal") * 400 * GetFrameTime();
                transform.position.y += InputSystem::GetAxis("Vertical") * 400 * GetFrameTime();

                if (transform.position.x < 0)
                    transform.position.x = 0;
                if (transform.position.x > _engine.GetScreenSize().x - 100)
                    transform.position.x = _engine.GetScreenSize().x - 100;

                if (transform.position.y < 0)
                    transform.position.y = 0;
                if (transform.position.y > _engine.GetScreenSize().y - 50)
                    transform.position.y = _engine.GetScreenSize().y - 50;
            };

            void GoBackMenu()
            {
                _engine.SwitchScene(0);
            };

        private:
            Engine &_engine;
            Entity _background;
            Entity _ship;
            Entity _enemy;
            Entity _sound;
            Entity _music;
            Entity _ground;
            Entity explosion;

            Audio _musicAudio;
    };
}

int main(void)
{
    ECS::Engine engine("R-type", 1600, 900, ECS::Mode2D, "../assets/");

    engine.Init();

    engine.SetAudioVolume(0.5f);

    // engine.SetCamera(ECS::Camera(ECS::CameraType::CAMERA_2D));

    std::shared_ptr<ECS::Scene> menu = std::make_shared<ECS::Menu>(engine);
    std::shared_ptr<ECS::Scene> game = std::make_shared<ECS::Game>(engine);

    unsigned int menuID = engine.AddScene(menu);
    unsigned int gameID = engine.AddScene(game);

    engine.SwitchScene(menuID);

    engine.Run();
}
