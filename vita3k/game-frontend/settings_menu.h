// Vita3K emulator project
// Copyright (C) 2026 Vita3K team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#pragma once

// The in-game settings menu: an overlay over the paused game, built from the game's settings
// declarations (settings.h), one page per section. It only edits its own copy of the values; the
// changes go to the front end's main thread (take_changes), which writes the settings file and
// applies the live ones (frontend.cpp). Its last page, Exit, quits the game (exit_requested).

#include "settings.h"

#include <overlay/user_interface.h>

#include <atomic>
#include <mutex>
#include <string>
#include <vector>

namespace game_frontend {

class SettingsMenu : public ::overlay::user_interface {
public:
    struct Change {
        std::string section, key, value;
    };
    struct Row {
        std::string section;
        SettingSpec spec;
        std::string value;
        bool editable = true;
        bool needs_restart = false; // changed, and takes effect at the next start
        bool exit = false; // the Exit page's row: Cross twice quits the game
    };
    struct Page {
        std::string title;
        std::vector<Row> rows;
    };

    SettingsMenu(const IniLayout &layout, const Values &values, const std::string &game_name);

    ::overlay::compiled_resource get_compiled() override;
    void on_button_pressed(::overlay::pad_button button, bool is_auto_repeat) override;

    // the changes since the last call, in order (main thread)
    std::vector<Change> take_changes();
    // Circle (or the front end's F1 / Esc): the menu wants to close
    bool close_requested() const { return m_close_requested.load(); }
    void request_close();
    // Exit the game, confirmed: the front end quits as when the window is closed
    bool exit_requested() const { return m_exit_requested.load(); }

    // a text_entry row being edited (Cross on it): the front end then sends the keyboard here instead
    // of to the game, and turns on text input (Android's on-screen keyboard)
    bool text_editing() const { return m_text_editing.load(); }
    enum class TextKey { Backspace, Delete, Left, Right, Home, End, Enter, Escape };
    void text_input(const std::string &text); // typed characters (UTF-8; printable ASCII is kept)
    void text_key(TextKey key);

private:
    std::mutex m_mutex; // the pages and the selection (input thread and render thread)
    std::vector<Page> m_pages;
    std::string m_game_name;
    size_t m_page = 0, m_row = 0, m_scroll = 0;
    std::vector<Change> m_changes;
    std::atomic<bool> m_close_requested{ false };
    std::atomic<bool> m_exit_requested{ false };
    bool m_exit_armed = false; // Cross pressed once on Exit the game; moving away disarms it
    std::atomic<bool> m_text_editing{ false };
    std::string m_edit_text; // the selected row's text while it is edited
    size_t m_edit_cursor = 0; // 0 to its length (the end: a new character)

    void step(Row &row, int direction);
    Row *selected_row();
    void end_edit(bool commit);
    void edit_button(::overlay::pad_button button);
    void move_row(int direction);
    void move_page(int direction);
};

} // namespace game_frontend
