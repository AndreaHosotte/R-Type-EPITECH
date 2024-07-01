/*
** EPITECH PROJECT, 2023
** B-CPP-500-MPL-5-1-rtype-nils.colire
** File description:
** sender
*/

#include "../includes/sender.hpp"

void SenderServer::SendGamesStruct(const game_t& games, const std::string& ip, int port)
{
    std::ostringstream archive_stream;
    boost::archive::binary_oarchive archive(archive_stream);
    archive << games;
    std::string message = archive_stream.str();
    _socket.send_to(boost::asio::buffer(message), udp::endpoint(address::from_string(ip), port));
}
