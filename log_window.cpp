#include "log_window.h"

log_window::log_window() {
  /// Default constructor
  clear();
}

void log_window::clear() {
  Buf.clear();
  LineOffsets.clear();
  LineOffsets.push_back(0);
}

void log_window::add(std::string const &line) {
  int old_size = Buf.size();
  Buf.append(line.c_str());
  for(int new_size = Buf.size(); old_size != new_size; ++old_size) {
    if(Buf[old_size] == '\n') LineOffsets.push_back(old_size + 1);
  }
}

void log_window::draw() {
  if(!ImGui::Begin("Event log")) {
    ImGui::End();
    return;
  }

  ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
  const char *buf = Buf.begin();
  const char *buf_end = Buf.end();
  {
    ImGuiListClipper clipper;
    clipper.Begin(LineOffsets.Size);
    while(clipper.Step()) {
      for(int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++) {
        const char *line_start = buf + LineOffsets[line_no];
        const char *line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
        ImGui::TextUnformatted(line_start, line_end);
      }
    }
    clipper.End();
  }
  ImGui::PopStyleVar();

  if(ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) ImGui::SetScrollHereY(1.0f);

  ImGui::EndChild();
  ImGui::End();
}
