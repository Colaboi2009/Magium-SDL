#pragma once

#include <vector>
#include <memory>

#include "ui_element.hpp"

namespace MagiumSDL{
    using UISceneList = std::vector<std::shared_ptr<UIElement>>;
    
    class UIScene : public UIElement{
    private:
        UISceneList m_elements;

    public:
        UIScene();
    
        void add(std::shared_ptr<UIElement> e) { m_elements.push_back(e); e->enable(); }
        UIElement &get(int index) { return *m_elements[index]; }
        UISceneList &getAll() { return m_elements; }
        
        void remove(std::shared_ptr<UIElement> e);
        void clear();
        
        void enable() override;
        void disable() override;
        void events(SDL_Event& event) override;
        void render() override;
    };
}