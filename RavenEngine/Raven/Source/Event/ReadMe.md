#### Event System

this is core system in a game engine. it recevie all event from os and dispatch them.

#### Desgin 

the Event System is consist with three components

1. Events

2. EventDispathcer

3. EventHandler


#### Event

Events have different types like (Mouse, Keybord, I/O, Window, Char and some defined events).
it will be packed and throw to EventDispatcher.


#### EventDispatcher

EventDispatcher is a thread-safe queue. receive and store all events.
events will be dispathed  to  eventHandler if the eventHandler register some kind of callback.


#### EventHandler


the final callbacks. a kind of event have a kind of callback in event handler.


##### Use case

```c++
//define a handler as member variable
    EventHandler handler;   

//.....
    void Init(){
        handler.mouseScrollHandler = std::bind(&Input::onMouseScrolled, this, std::placeholders::_1);
        //or 
        handler.mouseScrollHandler = [](auto event){
            //do you code
            return true;
            // if reture true. the current hanlder will consume the event. which means the event will not dispatch to other handlers
        };

        //register to dispatcher.
        Engine::Get().GetEventDispatcher().AddEventHandler(&handler);
        //don't be mind release (auto-release handler object)
    }

//the pre-defined callbacks are 
std::function<bool(MouseMoveEvent*)> mouseMoveHandler;
std::function<bool(MouseClickEvent*)> mouseClickHandler;
std::function<bool(MouseReleaseEvent*)> mouseRelaseHandler;
std::function<bool(MouseScrolledEvent*)> mouseScrollHandler;
std::function<bool(KeyPressedEvent*)> keyPressedHandler;
std::function<bool(KeyReleasedEvent*)> keyReleasedHandler;
std::function<bool(CharInputEvent*)> charInputHandler;

```




