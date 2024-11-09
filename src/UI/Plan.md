# UI subsystem

Context

* Receives widget configuration from user-space
* On draw(), assembles the widget models to submit to the renderer

How should it work?

## General Workflow

* Calling `Context::begin()` enters a "window" subcontext
* Subsequent UI creation calls will assign child widgets to the subcontext
  * `void Context::text(...)`
  * `bool Context::button(...)`
  * `std::string Context::text_input(...)`
* Calling `Context::end()` leaves the subcontext

## UI Assembly


## UI Rendering

* Calling `Context::draw()` handles rendering
  * Traverse widget tree, bottom-up
    * Compute widget dimensions and offset in screen space
    * Assign a `WidgetMaterial` from the pool or creates a new one
    * Build a `Graphics::Model` and submit to the renderer
