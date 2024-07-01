/*
** EPITECH PROJECT, 2023
** B-CPP-500-MPL-5-1-rtype-nils.colire
** File description:
** Game
*/

#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#ifndef GAME_HPP_
#define GAME_HPP_

#include "../../../Engine/src/Core/Engine.hpp"

#ifdef _WIN32
#if defined(_WIN32)
#define NOGDI
#define NOUSER
#endif
#endif

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>

#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <random>
#include <sstream>

#define SERVER_IPADDRESS "127.0.0.1"
#define SERVER_UDP_PORT 13251

using boost::asio::ip::address;
using boost::asio::ip::udp;

std::vector<std::string> split(const std::string &s, char delimiter);

struct Players
{
    int uid;
    int health;
    std::string ip;
    int x;
    int y;
    bool isAlive;

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & uid;
        ar & ip;
        ar & x;
        ar & y;
        ar & isAlive;
        ar & health;
    }
} typedef player_t;

struct Mobs {
    int uid;
    int health;
    int mob_type;
    int x;
    int y;
    bool isAlive;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & uid;
        ar & mob_type;
        ar & x;
        ar & y;
        ar & isAlive;
        ar & health;
    }
} typedef mob_t;

struct Bullets {
    int uid;
    int uid_owner;
    int bullet_type;
    int x;
    int y;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & uid;
        ar & uid_owner;
        ar & bullet_type;
        ar & x;
        ar & y;
    }
} typedef bullet_t;

struct Games
{
    int gid;
    int stage;
    std::vector<player_t> players;
    std::vector<mob_t> mobs;
    std::vector<bullet_t> bullets;

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & gid;
        ar & stage;
        ar & players;
        ar & mobs;
        ar & bullets;
    }
} typedef game_t;

class Game : public ECS::DefaultScene
{
public:
    Game(ECS::Engine &engine, int port);
    ~Game();

    void InitBackgrounds(void);
    void UpdateBackground();

    void InitShip(void);
    void InitHud(void);
    void InitAudio(void);
    void InitEndGame(void);

    // maybe move in a Player class
    void ShootBullet(void);
    void EnemyShootBullet(ECS::Entity bullet_id, ECS::Entity enemy);
    void createAllyBullet(ECS::Entity bullet_id, ECS::Entity ally);
    bool bulletExists(int uid);

    void HitEnemies(ECS::Entity bullet);
    void HitShip(ECS::Entity bullet);
    void HitAnimation(ECS::Renderer2D &renderer);
    void fillMagazine();
    void MoveShip(void);
    void UpdateHud();

    void SpawnExplosion(Vector2 pos);
    void LoseHealth(int damage);
    void PlayerDeath(void);
    void PlayerWin(void);

    void Start();
    void Update();

    void SendData(void);
    void UpdateEntityPosition(ECS::Entity entity, Vector2 n_pos);
    void ReceiveData(game_t received);
    void updatePlayerMap(ECS::Entity id, std::string ip);

    ECS::Entity CreateAlly(int x, int y, int uid);
    void CreateEnnemy(int x, int y, int uid);
    void CreateBoss(int x, int y, int uid);

    // SERVER NETWORK
    void server_handle_receive(const boost::system::error_code &error, size_t bytes_transferred);
    void server_wait();
    void server_receiver(int port);
    void send_game_struct(game_t game);

private: //! A mettre en private non ?
    ECS::Engine &_engine;

    ECS::Entity _background;

    ECS::Entity _enemy;
    std::vector<ECS::Entity> _enemies;
    std::vector<Color> _mobColors = {
        WHITE, BLACK, BLUE, GREEN, YELLOW,
        PURPLE, ORANGE, PINK, SKYBLUE};

    ECS::Entity _ship;
    ECS::Entity _boss;
    bool _bossSpawned = false;
    bool _bossKilled = false;
    std::vector<ECS::Entity> _magazine;
    std::unordered_map<ECS::Entity, std::string> _playerMap;

    ECS::Entity _sound;
    ECS::Entity _music;
    ECS::Entity _healthBar;
    ECS::Audio _musicAudio;
    ECS::Entity _takeItSound;
    ECS::Entity _bossHitSound;
    ECS::Entity _deathScreen;
    ECS::Entity _winScreen;
    ECS::Entity _scoreUpSound;

    game_t _game;
    player_t _player;

    int _port;
    boost::asio::io_service io_service;
    udp::socket socket{io_service};
    boost::array<char, 1024> recv_buffer;
    udp::endpoint remote_endpoint;

    int _score;
    ECS::Entity _scoreText;

    std::vector<ECS::Entity> _enemyBullets;
    std::vector<ECS::Entity> _allyBullets;

    std::vector<ECS::Entity> _destroyedEntities;
};

#endif /* !GAME_HPP_ */
