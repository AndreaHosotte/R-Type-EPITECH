/*
** EPITECH PROJECT, 2023
** B-CPP-500-MPL-5-1-rtype-nils.colire
** File description:
** server
*/

#include "../includes/server.hpp"
#include "../includes/sender.hpp"

static int stage = 0;

struct Client {
    boost::asio::io_service io_service;
    udp::socket socket{io_service};
    boost::array<char, 1024> recv_buffer;
    udp::endpoint remote_endpoint;
    Games games = {1, 1, {}, {}, {}};


    void server_management(game_t& tmp_game) {
        for (auto& player : tmp_game.players) {
            int found = false;
            for (auto& game_player : games.players) {
                if (game_player.uid == player.uid) {
                    game_player.x = player.x;
                    game_player.y = player.y;
                    game_player.isAlive = player.isAlive;
                    found = true;
                }
            }
            if (!found) {
                games.players.push_back(player);
            }
        }

        int allDead = true;
        for (auto& mob : games.mobs) {
            if (mob.health > 0) {
                allDead = false;
                break;
            }
        }

        for (auto& mob : tmp_game.mobs) {
            for (auto& game_mob : games.mobs) {
                if (game_mob.uid == mob.uid) {
                    game_mob.isAlive = mob.isAlive;
                    game_mob.health = mob.health;
                }
            }
        }


        for (auto& bullet : tmp_game.bullets) {
            int found = false;
            for (auto& game_bullet : games.bullets) {
                if (game_bullet.uid == bullet.uid) {
                    found = true;
                }
            }
            if (!found) {
                games.bullets.push_back(bullet);
            }
        }

        if (allDead && stage < 4) {
            stage++;
            games.stage = stage;
            std::cout << "Stage " << stage << "\n";
            games.mobs.clear();
            games.bullets.clear();
        }
    }

    void handle_receive(const boost::system::error_code& error, size_t bytes_transferred) {
        game_t tmp_game;

        if (error) {
            std::cout << "Receive failed: " << error.message() << "\n";
            return;
        }

        try {
            std::string message(recv_buffer.begin(), recv_buffer.begin() + bytes_transferred);

            // Deserialize message to game_t struct
            std::stringstream ss(message);
            boost::archive::binary_iarchive ia(ss);
            ia >> tmp_game;
            server_management(tmp_game);
        } catch (std::exception& e) {
            std::cout << "Données invalide ! TRICHEUR !\n";
        }
        wait();
    }

    void wait() {
        socket.async_receive_from(boost::asio::buffer(recv_buffer),
            remote_endpoint,
            boost::bind(&Client::handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }

    void Receiver() {
        socket.open(udp::v4());
        socket.bind(udp::endpoint(address::from_string(SERVER_IPADDRESS), SERVER_UDP_PORT));

        wait();

        std::cout << "Receiving at " << socket.local_endpoint().address().to_string() << ":" << socket.local_endpoint().port() << "\n";
        io_service.run();
        std::cout << "Receiver exit\n";
    }
};

static void stage_3(Client &client)
{
    static int alreadyDone = false;

    if (alreadyDone) return;

    int x = 800;
    int y = 450;

    mob_t mob = {
        .uid = rand() % 10000,
        .health = 1000,
        .mob_type = 3,
        .x = x,
        .y = y,
        .isAlive = true,
    };

    client.games.mobs.push_back(mob);
    alreadyDone = true;

}

static void stage_2(Client &client)
{
    static int alreadyDone = false;

    if (alreadyDone) return;

    for (int i = 0; i < 10; i++) {
        int x = rand() % 800 + 1000;
        int y = rand() % 880 + 20;

        mob_t mob = {
            .uid = rand() % 10000,
            .health = 100,
            .mob_type = 1,
            .x = x,
            .y = y,
            .isAlive = true,
        };
        client.games.mobs.push_back(mob);
    }
    alreadyDone = true;

}

static void stage_1(Client &client) {
    static int alreadyDone = false;

    if (alreadyDone) return;
    for (int i = 0; i < 4; i++) {
        int x = rand() % 800 + 1000;
        int y = rand() % 880 + 20;

        mob_t mob = {
            .uid = rand() % 10000,
            .health = 100,
            .mob_type = 1,
            .x = x,
            .y = y,
            .isAlive = true,
        };
        client.games.mobs.push_back(mob);
    }
    alreadyDone = true;
}

static void ia_ennemies(Client &client)
{
    if (client.games.players.size() == 0)
        return;
    for (auto& mob : client.games.mobs) {
        if (mob.isAlive) {
            int x = rand() % 10 - 5;
            int y = rand() % 10 - 5;

            if (mob.x + x < 0 || mob.x + x > 1920)
                x = -x;
            if (mob.y + y < 0 || mob.y + y > 1080)
                y = -y;

            mob.x += x;
            mob.y += y;
        }
    }
    if (rand() % 250 == 0 && client.games.mobs.size() > 0) {
        int uid_owner = rand() % client.games.mobs.size();
        int x = client.games.mobs[uid_owner].x;
        int y = client.games.mobs[uid_owner].y;

        if (client.games.mobs[uid_owner].health <= 0)
            return;

        bullet_t bullet = {
            .uid = rand() % 10000,
            .uid_owner = client.games.mobs[uid_owner].uid,
            .bullet_type = 2,
            .x = x,
            .y = y,
        };
        client.games.bullets.push_back(bullet);
        // wait 5 seconds and delete bullet
        std::thread t([&] {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            // delete bullet
            client.games.bullets.erase(std::remove_if(client.games.bullets.begin(), client.games.bullets.end(), [&](const bullet_t& b) {
                return b.uid == bullet.uid;
            }), client.games.bullets.end());
        });
        t.detach();
    }

    if (stage == 1) {
       stage_1(client);
    } else if (stage == 2) {
        stage_2(client);
    } else {
        stage_3(client);
    }
}

int main(int argc, char *argv[])
{
    boost::asio::io_service io_service;
    SenderServer sender(io_service);
    Client client;

    std::thread r([&] { client.Receiver(); });

    std::thread s([&] {
        std::cout << "Waiting for activity...\n";

        // make random seed
        srand(time(NULL));
        while(1) {
            // Send to all clients (games.players.ip) the games struct
            ia_ennemies(client);
            for (auto& player : client.games.players) {
                auto ip = player.ip.substr(0, player.ip.find(":"));
                auto port = player.ip.substr(player.ip.find(":") + 1);

                sender.SendGamesStruct(client.games, ip, std::stoi(port));
            }
            client.games.mobs.erase(std::remove_if(client.games.mobs.begin(), client.games.mobs.end(), [&](const mob_t& m) {
                return m.isAlive == false;
            }), client.games.mobs.end());
            std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
        }
    });

    // Run all threads
    r.join();
    s.join();
    return 0;
}