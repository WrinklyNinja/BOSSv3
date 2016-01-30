/*  LOOT

    A load order optimisation tool for Oblivion, Skyrim, Fallout 3 and
    Fallout: New Vegas.

    Copyright (C) 2012-2016    WrinklyNinja

    This file is part of LOOT.

    LOOT is free software: you can redistribute
    it and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version.

    LOOT is distributed in the hope that it will
    be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with LOOT.  If not, see
    <http://www.gnu.org/licenses/>.
    */

#ifndef __LOOT_ERROR__
#define __LOOT_ERROR__

#include <exception>
#include <string>

namespace loot {
    class error : public std::exception {
    public:
        error(const unsigned int code_arg, const std::string& what_arg) : _code(code_arg), _what(what_arg) {}
        ~error() throw() {};

        unsigned int code() const { return _code; }
        const char * what() const throw() { return _what.c_str(); }

        /* These must not be changed for API stability. */
        static const unsigned int ok = 0;
        static const unsigned int liblo_error = 1;
        static const unsigned int path_write_fail = 2;
        static const unsigned int path_read_fail = 3;
        static const unsigned int condition_eval_fail = 4;
        static const unsigned int regex_eval_fail = 5;
        static const unsigned int no_mem = 6;
        static const unsigned int invalid_args = 7;
        static const unsigned int no_tag_map = 8;
        static const unsigned int path_not_found = 9;
        static const unsigned int no_game_detected = 10;
        //11 was subversion_error, and was removed along with svn support.
        static const unsigned int git_error = 12;
        static const unsigned int windows_error = 13;
        static const unsigned int sorting_error = 14;
    private:
        unsigned int _code;
        std::string _what;
    };
}

#endif
