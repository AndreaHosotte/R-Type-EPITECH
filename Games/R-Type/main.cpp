/*
** EPITECH PROJECT, 2023
** B-CPP-500-MPL-5-1-rtype-nils.colire
** File description:
** main
*/

#include "include/Game.hpp"

int main(void)
{
    srand(time(NULL));
    int port = rand() % 5000 + 5000;
    ECS::Engine engine("R-Type", 1920, 1080, ECS::Mode2D, "../assets/");
    engine.Init();

    std::shared_ptr<ECS::Scene> game = std::make_shared<Game>(engine, port);

    unsigned int gameID = engine.AddScene(game);

    engine.SwitchScene(gameID);
    engine.Run();
}
