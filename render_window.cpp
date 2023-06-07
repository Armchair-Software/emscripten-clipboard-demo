#include "render_window.h"
#include <iostream>
#include <emscripten/val.h>

namespace render {

EM_BOOL callback_window_resize([[maybe_unused]] int event_type, const EmscriptenUiEvent *event, void *data) { // event_type == EMSCRIPTEN_EVENT_RESIZE, docs: https://emscripten.org/docs/api_reference/html5.h.html#id16
  /// Handle a browser window resize event
  auto &this_window{*static_cast<window*>(data)};
  this_window.document_body_size.assign(static_cast<unsigned int>(event->documentBodyClientWidth), static_cast<unsigned int>(event->documentBodyClientHeight));
  emscripten_get_canvas_element_size("#canvas", &this_window.canvas_size.x, &this_window.canvas_size.y); // update the cached canvas size
  this_window.device_pixel_ratio = emscripten::val::global("window")["devicePixelRatio"].as<float>(); // query device pixel ratio using JS
  this_window.update_viewport_size();                                           // update the viewport
  return false;                                                                 // allow other handlers to handle this event also
}

bool window::init() {
  /// Initialise the window and graphics context
  if(glfwInit() != GLFW_TRUE) {                                                 // initialise the opengl window
    std::cerr << "render::window: ERROR: Graphics initialisation failed!  Cannot continue." << std::endl;
    return false;
  }

  emscripten_get_canvas_element_size("#canvas", &canvas_size.x, &canvas_size.y);
  document_body_size.assign(emscripten::val::global("document")["body"]["clientWidth"].as<unsigned int>(),
                            emscripten::val::global("document")["body"]["clientHeight"].as<unsigned int>());
  device_pixel_ratio = emscripten::val::global("window")["devicePixelRatio"].as<float>(); // query device pixel ratio using JS
  viewport_size = document_body_size;

  emscripten_set_resize_callback(
    EMSCRIPTEN_EVENT_TARGET_WINDOW,                                             // target = EMSCRIPTEN_EVENT_TARGET_WINDOW to get resize events from the Window object
    this,                                                                       // userData
    false,                                                                      // useCapture
    callback_window_resize                                                      // callback
  );

  // set up the main window's hints in advance
  glfwWindowHint(GLFW_DEPTH_BITS, 24);
  glfwWindowHint(GLFW_STENCIL_BITS, 8);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);                         // forward compat disables all deprecated functions - we don't want that

  #ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  #endif // NDEBUG
  glfw_window = glfwCreateWindow(canvas_size.x,                                 // use initial canvas size
                                 canvas_size.y,
                                 window_title.c_str(),                          // window title
                                 nullptr,                                       // monitor to use fullscreen, NULL here means run windowed - we always do under emscripten
                                 nullptr);                                      // the context to share with, see http://stackoverflow.com/a/17792242/1678468

  if(!glfw_window) {                                                            // exit if this didn't work
    std::cerr << "render::window: ERROR: Failed to open window!  Cannot continue." << std::endl;
    return false;
  }
  glfwMakeContextCurrent(glfw_window);

  glGetIntegerv(GL_MAX_VIEWPORT_DIMS, max_viewport_size);

  update_viewport_size();

  initialised = true;
  return true;
}

void window::set_window_title(std::string const &new_window_title) {
  /// Update the title of the displayed window
  window_title = new_window_title;

  if(initialised) {
    glfwSetWindowTitle(glfw_window, window_title.c_str());
  }
}

void window::update_viewport_size() {
  /// Update the viewport size based on what we know about the window at present
  vec2f viewport_size_scaled = static_cast<vec2f>(document_body_size) * device_pixel_ratio; // use document body size - make sure css is configured for html and body to take up 100% of the browser window height
  viewport_size.assign(static_cast<int>(std::round(viewport_size_scaled.x)), static_cast<int>(std::round(viewport_size_scaled.y))); // round the scaled size
  viewport_size = std::min(viewport_size, max_viewport_size);                   // clamp viewport size to permitted max

  glfwSetWindowSize(glfw_window, viewport_size.x, viewport_size.y);
  glViewport(0, 0, viewport_size.x, viewport_size.y);
  glScissor(0, 0, viewport_size.x, viewport_size.y);
  emscripten_get_canvas_element_size("#canvas", &canvas_size.x, &canvas_size.y);

  if(callback_refresh) callback_refresh(*this);
}

}
