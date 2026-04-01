#include <GopherEngine/Platform/Window.hpp>

#include <iostream>
using namespace std;

namespace GopherEngine {
    Window::Window(std::string title, unsigned int width, unsigned int height) {
        window_ = nullptr;
        title_ = title;
        width_ = width;
        height_ = height;
        dirty_ = false;
        vertical_sync_ = false;
        framerate_limit_ = 0;

        style_ = sf::Style::Default;
        state_ = sf::State::Windowed;
        viewport_mode_ = ViewportMode::Fit;

        last_mouse_x_ = 0;
        last_mouse_y_ = 0;
        key_states_ = {}; 

        // Default callbacks are no-ops — avoids null checks in handle_events()
        on_mouse_move_ = ([](const MouseMoveEvent&) {});
        on_mouse_down_ = ([](const MouseButtonEvent&) {});
        on_mouse_up_ = ([](const MouseButtonEvent&) {});
        on_mouse_scroll_ = ([](const MouseScrollEvent&) {});
        on_key_down_ = ([](const KeyEvent&) {});
        on_key_up_ = ([](const KeyEvent&) {});
        on_key_press_ = ([](const KeyEvent&) {});
    }

    Window::~Window() {
        if(window_ != nullptr)
            delete window_;
    }

    void Window::set_size(unsigned int width, unsigned int height) {
        width_ = width;
        height_ = height;   
    }

    void Window::set_style(int style) {
        style_ = style;
    }
    
    void Window::set_state(sf::State state) {
        state_ = state;
    }

    void Window::set_title(std::string title) {
        title_ = title;
    }

    void Window::set_vertical_sync(bool vertical_sync) {
        vertical_sync_ = vertical_sync;
    }

    void Window::set_framerate_limit(unsigned int limit) {
        framerate_limit_ = limit;
    }

    bool Window::set_active(bool active) {
        if (window_ == nullptr) {
            cerr << "Error: window has not been created yet" << endl;
            return false;
        }
        return window_->setActive(active);
    }

    int Window::get_width() {
        return width_;
    }
    
    int Window::get_height() {
        return height_;
    }

    bool Window::is_open() {
        if(window_ != nullptr)
            return window_->isOpen();
        else
            return false;
    }

    void Window::set_dirty(bool dirty) {
        dirty_ = dirty;
    }

    bool Window::get_dirty() {
        return dirty_;
    }

    void Window::set_viewport_mode(ViewportMode mode) {
        viewport_mode_ = mode;
        dirty_ = true;
    }

    ViewportMode Window::get_viewport_mode() const {
        return viewport_mode_;
    }

    void Window::create_window() {
        if(window_ != nullptr)
            delete window_;
        
        sf::ContextSettings contextSettings;
        contextSettings.depthBits   = 24;
        contextSettings.sRgbCapable = true;

        window_ = new sf::Window(sf::VideoMode({width_, height_}), 
                        static_cast<sf::String>(title_),
                        style_,
                        state_,
                        contextSettings);
                        
        window_->setVerticalSyncEnabled(vertical_sync_);

        if(framerate_limit_ > 0)
            window_->setFramerateLimit(framerate_limit_);
    }

    void Window::handle_events() {
        if(window_ == nullptr) {
            cerr << "Error: window has not been created yet" << endl;
            return;
        }

        // Handle events
        while (const std::optional event = window_->pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window_->close();
            }
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
            {
                width_ = resized->size.x;
                height_ = resized->size.y;
                dirty_ = true;
            }
            else if (const auto* moved = event->getIf<sf::Event::MouseMoved>())
            {
                MouseMoveEvent e;
                e.x       = moved->position.x;
                e.y       = moved->position.y;
                e.delta_x = e.x - last_mouse_x_;
                e.delta_y = e.y - last_mouse_y_;
                last_mouse_x_ = e.x;
                last_mouse_y_ = e.y;
                on_mouse_move_(e);
            }
            else if (const auto* pressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                MouseButtonEvent e;
                e.button = static_cast<MouseButton>(pressed->button); // see note below
                e.x = pressed->position.x;
                e.y = pressed->position.y;
                on_mouse_down_(e);
            }
            else if (const auto* released = event->getIf<sf::Event::MouseButtonReleased>())
            {
                MouseButtonEvent e;
                e.button = static_cast<MouseButton>(released->button);
                e.x = released->position.x;
                e.y = released->position.y;
                on_mouse_up_(e);
            }
            else if (const auto* scrolled = event->getIf<sf::Event::MouseWheelScrolled>())
            {
                MouseScrollEvent e;
                e.delta = scrolled->delta;
                e.x = scrolled->position.x;
                e.y = scrolled->position.y;
                on_mouse_scroll_(e);
            }
            else if (const auto* kp = event->getIf<sf::Event::KeyPressed>())
            {
                KeyEvent e;
                e.key = static_cast<Key>(kp->code); 
                e.shift = kp->shift;
                e.control = kp->control;
                e.alt = kp->alt;

                if(!key_states_[static_cast<size_t>(e.key)]) {
                    key_states_[static_cast<size_t>(e.key)] = true;
                    on_key_down_(e);
                }

                on_key_press_(e);
            }
            else if (const auto* kr = event->getIf<sf::Event::KeyReleased>())
            {
                KeyEvent e;
                e.key = static_cast<Key>(kr->code); 
                e.shift = kr->shift;
                e.control = kr->control;
                e.alt = kr->alt;

                key_states_[static_cast<size_t>(e.key)] = false;
                
                on_key_up_(e);
            }
        }
    }

    void Window::display() {
        if(window_ == nullptr) {
            std::cerr << "Error: window has not been created yet" << std::endl;
        }
        else {
            window_->display();
        }
    }

    // -----------------------------------------------------------------------
    // Callback registration
    // -----------------------------------------------------------------------

    void Window::set_on_mouse_move(std::function<void(const MouseMoveEvent&)> cb) { 
        on_mouse_move_ = cb; 
    }

    void Window::set_on_mouse_down(std::function<void(const MouseButtonEvent&)> cb) {
        on_mouse_down_ = cb;
    }

    void Window::set_on_mouse_up(std::function<void(const MouseButtonEvent&)> cb) {
        on_mouse_up_ = cb;
    }

    void Window::set_on_mouse_scroll(std::function<void(const MouseScrollEvent&)> cb) {
        on_mouse_scroll_ = cb;
    }

    void Window::set_on_key_down(std::function<void(const KeyEvent&)> cb) {
        on_key_down_ = cb;
    }

    void Window::set_on_key_up(std::function<void(const KeyEvent&)> cb) {
        on_key_up_ = cb;
    }

    void Window::set_on_key_press(std::function<void(const KeyEvent&)> cb) {
        on_key_press_ = cb;
    }

}