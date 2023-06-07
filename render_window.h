#ifndef RENDER_WINDOW_H_INCLUDED
#define RENDER_WINDOW_H_INCLUDED

#include <string>
#include <emscripten/html5.h>
#include <GLFW/glfw3.h>
#include <vectorstorm/vector/vector2.h>

namespace render {

class window {
  bool initialised = false;

  std::string window_title;

  vec2i viewport_size;                                                          // our idea of the size of the viewport we render to, in real pixels
  vec2i max_viewport_size;                                                      // max implementation supported viewport size
  vec2i canvas_size;                                                            // implementation-reported canvas size
  vec2ui document_body_size;                                                    // these sizes are before pixel ratio scaling, i.e. they change when the browser window is zoomed
  float device_pixel_ratio = 1.0f;

  std::function<void(int, char const*)> callback_error = nullptr;
  std::function<void(window &this_window)> callback_refresh = nullptr;

public:
  GLFWwindow *glfw_window = nullptr;

  bool init();

  void set_window_title(std::string const &new_window_title);

  void update_viewport_size();

private:
  friend EM_BOOL callback_window_resize(int event_type, const EmscriptenUiEvent *event, void *data);
};

}

#endif // RENDER_WINDOW_H_INCLUDED
