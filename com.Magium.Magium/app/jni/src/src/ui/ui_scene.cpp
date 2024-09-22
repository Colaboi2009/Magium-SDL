#include "ui_scene.hpp"

namespace MagiumSDL{
    UIScene::UIScene() {
        m_rect = {0, 0, 0, 0};
    }

    void UIScene::remove(std::shared_ptr<UIElement> e)
    {
        for (int i = 0; i < m_elements.size(); i++) {
            if (m_elements[i] == e)
                m_elements.erase(m_elements.begin() + i);
        }
    }

    void UIScene::clear()
    {
        m_elements.clear();
    }

    void UIScene::enable()
    {
        UIElement::enable();
        for (auto e : m_elements){
            e->enable();
        }
    }

    void UIScene::disable() 
    {
        UIElement::disable();
        for (auto e : m_elements){
            e->disable();
        }
    }
    
    void UIScene::events(SDL_Event &event)
    {
        // std::vector<int> toRemove;
        for (int i = 0; i < m_elements.size(); i++){
            auto e = m_elements[i];
            // if (e->destroyed())
            //     toRemove.push_back(i);
            if (e->enabled()){
                e->events(event);
            }
        }

        // for (int i : toRemove){
        //     m_elements.erase(m_elements.begin() + i);
        // }
    }
    
    void UIScene::render()
    {
        for (int i = 0; i < m_elements.size(); i++){
            auto e = m_elements[i];
            SDL_FRect originalPos = e->rect();
            e->rect().x += m_rect.x;
            e->rect().y += m_rect.y;
            if (e->enabled()){
                e->render();
            }
            e->rect() = originalPos;
        }
    }
}