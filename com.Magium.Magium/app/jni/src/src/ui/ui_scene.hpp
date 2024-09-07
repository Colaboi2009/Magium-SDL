#pragma once

#include <vector>

#include "ui_element.hpp"

namespace MagiumSDL{
    class UIScene : public UIElement{
    private:
        std::vector<UIElement*> m_elements;

    public:
        UIScene();
    
        void add(UIElement *element) { m_elements.push_back(element); element->enable(); }
        UIElement &get(int index) { return *m_elements[index]; }
        std::vector<UIElement*> &getAll() { return m_elements; }
        
        void destroyAll();
        
        void enable() override;
        void disable() override;
        void events(SDL_Event& event) override;
        void render() override;
    };
}