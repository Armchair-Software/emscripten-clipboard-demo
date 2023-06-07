#pragma once
#include <string>
#include <imgui/imgui.h>

/// Usage:
///  static log_window my_log;
///  my_log.add("Hello %d world\n", 123);
///  my_log.draw("title");
struct log_window {
  ImGuiTextBuffer Buf;
  ImVector<int> LineOffsets;                                                    // Index to lines offset. We maintain this with AddLog() calls.

  log_window();

  void clear();

  void add(std::string const &line);

  void draw();
};
