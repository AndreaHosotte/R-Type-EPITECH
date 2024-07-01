/*
** EPITECH PROJECT, 2023
** B-CPP-500-MPL-5-1-rtype-nils.colire
** File description:
** server
*/

#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#ifndef SERVER_HPP_
    #define SERVER_HPP_
    #include <iostream>
    #include <thread>
    #include <string>
    #include <vector>
    #include <sstream>

    #include <boost/asio.hpp>
    #include <boost/array.hpp>
    #include <boost/bind.hpp>
    #include <boost/archive/binary_iarchive.hpp>
    #include <boost/archive/binary_oarchive.hpp>
    #include <boost/serialization/vector.hpp>

    #define SERVER_IPADDRESS "127.0.0.1"
    #define SERVER_UDP_PORT 13251


using boost::asio::ip::udp;
using boost::asio::ip::address;

std::vector<std::string> split(const std::string& s, char delimiter);

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


#endif /* !SERVER_HPP_ */
