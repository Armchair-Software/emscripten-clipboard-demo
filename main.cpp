#include <iostream>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_stdlib.h>
#include <emscripten_browser_clipboard.h>
#include "render_window.h"
#include "log_window.h"

static render::window window;
static log_window event_log;

static std::string clipboard_content;

auto loop(void *data)->void;
char const *get_clipboard_for_imgui(void *user_data [[maybe_unused]]);
void set_clipboard_from_imgui(void *user_data [[maybe_unused]], char const *text);

[[noreturn]] auto main()->int {                                                 // noreturn here is not standards-compliant, but is appropriate for emscripten with a main loop
  window.set_window_title("Loading Emscripten ImGui Clipboard Demo");
  if(!window.init()) {
    std::cerr << "ERROR: Failed to create a window, cannot continue." << std::endl;
    EM_ASM(
      if(confirm("Error - unable to create a graphics window.  Please check that WebGL 2.0 is supported by your browser, and is enabled.  Press OK to visit a validation page.")) {
        window.location.replace("https://get.webgl.org/webgl2/");
      }
    );
    std::abort();
  }
  window.set_window_title("Emscripten ImGui Clipboard Demo");

  ImGui::CreateContext();
  ImGuiIO &imgui_io{ImGui::GetIO()};
  imgui_io.IniFilename = nullptr;                                               // disable saving settings to disk
  imgui_io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;                 // don't attempt to change mouse cursors
  imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;                   // enable keyboard controls

  ImGui_ImplGlfw_InitForOpenGL(window.glfw_window, true);                       // set up imgui backends
  ImGui_ImplOpenGL3_Init("#version 300 es");                                    // version 300 es for OpenGL ES 3.0 applies to WebGL 2.0

  emscripten_browser_clipboard::paste([](std::string const &paste_data, void *callback_data [[maybe_unused]]){
    /// Callback to handle clipboard paste from browser
    event_log.add("Browser pasted: " + paste_data + "\n");
    clipboard_content = std::move(paste_data);
  });
  /// Not needed for this example, as ImGui sets the content - but if you want to respond to browser copy request events, use this:
  //emscripten_browser_clipboard::copy([](void *callback_data [[maybe_unused]]){
  //  /// Callback to offer data for clipboard copy to browser
  //  event_log.add("Browser copied: " + clipboard_content + "\n");
  //  return clipboard_content.c_str();
  //});

  imgui_io.GetClipboardTextFn = get_clipboard_for_imgui;
  imgui_io.SetClipboardTextFn = set_clipboard_from_imgui;

  event_log.add("Initialised and ready." "\n");

  EM_ASM(
    if("virtualKeyboard" in navigator) {
      alert("Virtual keyboard API available");
      navigator.virtualKeyboard.overlaysContent = true;
    } else {
      alert("No virtual keyboard API available");
      var input = document.createElement("input");
      input.type = "text";
      input.value = "testing";
      input.id = "text_input";
      document.body.appendChild(input); // put it into the DOM
    }
  );
  // TODO: implement input to imgui on change (for swipe etc), see https://stackoverflow.com/questions/574941/best-way-to-track-onchange-as-you-type-in-input-type-text

  emscripten_set_main_loop_arg(&loop, nullptr, 0, true);                        // loop function, user data, FPS (0 to use browser requestAnimationFrame mechanism), simulate infinite loop
  std::unreachable();                                                           // execution never returns to this point
}

auto loop(void*)->void {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();


  ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f)); // set next window position in the centre of the frame, use pivot=(0.5f,0.5f) to center on given point, etc.
  if(ImGui::Begin("Emscripten ImGui Clipboard Demo")) {
    ImGui::TextUnformatted("Use the box below to type, copy and paste text");
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    static std::string text_box_content;
    ImGui::InputTextMultiline("##text_box", &text_box_content, ImVec2(400, 200));
  }
  ImGui::End();

  ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_FirstUseEver);
  event_log.draw();


  ImGuiIO &imgui_io{ImGui::GetIO()};
  if(imgui_io.WantTextInput) {
    EM_ASM(
      if("virtualKeyboard" in navigator) {
        navigator.virtualKeyboard.show();
      } else {
        document.getElementById("text_input").focus();
      }
    );
  } else {
    EM_ASM(
      if("virtualKeyboard" in navigator) {
        navigator.virtualKeyboard.hide();
      }
    );
  }

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  glfwSwapBuffers(window.glfw_window);
}

char const *get_clipboard_for_imgui(void *user_data [[maybe_unused]]) {
  /// Callback for imgui, to return clipboard content
  event_log.add("ImGui requested content: " + clipboard_content + "\n");
  return clipboard_content.c_str();
}

void set_clipboard_from_imgui(void *user_data [[maybe_unused]], char const *text) {
  /// Callback for imgui, to set clipboard content
  clipboard_content = text;
  event_log.add("ImGui set content: " + clipboard_content + "\n");
  emscripten_browser_clipboard::copy(clipboard_content);
}
