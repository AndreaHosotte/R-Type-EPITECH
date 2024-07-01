/*
** EPITECH PROJECT, 2023
** B-CPP-500-MPL-5-1-rtype-nils.colire
** File description:
** sender
*/

#pragma once

#ifndef SENDER_HPP_
    #define SENDER_HPP_

   #include "server.hpp"

    class SenderServer {
        private:
            udp::socket _socket;

        public:
            SenderServer(boost::asio::io_service& io_service) : _socket(io_service, udp::v4()) {};

            void SendGamesStruct(const game_t& games, const std::string& ip, int port);
    };



#endif /* !SENDER_HPP_ */
