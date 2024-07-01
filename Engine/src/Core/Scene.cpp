/*
** EPITECH PROJECT, 2023
** B-CPP-500-MPL-5-1-rtype-nils.colire
** File description:
** Scene
*/

#include "Scene.hpp"
#include <iostream>

namespace ECS {
    SceneManager::SceneManager() : _scenes(0), _currentScene(0), _insertedSceneId(0)
    {
    }

    SceneManager::~SceneManager()
    {
    }

    void SceneManager::Start() {
        if (_currentScene) {
            _currentScene->Start();
        }
    }

    void SceneManager::Update()
    {
        if (_currentScene) {
            _currentScene->Update();
        }
    }

    void SceneManager::LateUpdate()
    {
        if (_currentScene)
            _currentScene->LateUpdate();
    }

    unsigned int SceneManager::AddScene(std::shared_ptr<Scene> scene)
    {
        auto inserted = _scenes.insert(std::make_pair(_insertedSceneId, scene));
        _insertedSceneId++;
        inserted.first->second->OnCreate();
        return _insertedSceneId - 1;
    }

    void SceneManager::RemoveScene(unsigned int id)
    {
        auto it = _scenes.find(id);
        if (it != _scenes.end()) {
            if (_currentScene == it->second)
                _currentScene = nullptr;
            it->second->OnDestroy();
            _scenes.erase(it);
        }
    }

    void SceneManager::SwitchScene(unsigned int id)
    {
        auto it = _scenes.find(id);
        if (it != _scenes.end()) {
            if (_currentScene)
                _currentScene->OnDeactivate();
            _currentScene = it->second;
            _currentScene->OnActivate();
        }
    }
}
