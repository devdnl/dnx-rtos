--[[============================================================================
@file    sdspi.lua

@author  Daniel Zorychta

@brief   SDSPI configuration wizard.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the  Free Software  Foundation;  either version 2 of the License, or
         any later version.

         This  program  is  distributed  in the hope that  it will be useful,
         but  WITHOUT  ANY  WARRANTY;  without  even  the implied warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         You  should  have received a copy  of the GNU General Public License
         along  with  this  program;  if not,  write  to  the  Free  Software
         Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


*//*========================================================================--]]

require "defs"
require "db"

--------------------------------------------------------------------------------
-- OBJECTS
--------------------------------------------------------------------------------
tty = {}

--------------------------------------------------------------------------------
-- FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Ask user to select enable/disable module
--------------------------------------------------------------------------------
local function ask_for_enable()
        local choice = key_read(db.path.project.flags, "__ENABLE_TTY__")
        msg(progress(1, 8).."Do you want to enable TTY module?")
        msg("Current selection is: "..filter_yes_no(choice)..".")
        add_item(yes, "Yes")
        add_item(no, "No")
        choice = get_selection()
        if can_be_saved(choice) then
                key_save(db.path.project.flags, "__ENABLE_TTY__", choice)
                key_save(db.path.project.mk, "ENABLE_TTY", choice)
        end

        progress(2)

        return choice
end

--------------------------------------------------------------------------------
-- @brief Configuration
--------------------------------------------------------------------------------
local function configure_tty()

        local function configure_term_cols()
                local choice = key_read(db.path.noarch.tty.flags, "__TTY_TERM_COLS__")
                msg(progress() .. "Configure number of terminal's columns. Typical value for VT100 is 80 columns.")
                msg("Current choice is: " .. choice .. " columns.")
                choice = get_number("dec", 16, 250)
                if (can_be_saved(choice)) then
                        key_save(db.path.noarch.tty.flags, "__TTY_TERM_COLS__", choice)
                end

                return choice
        end

        local function configure_term_rows()
                local choice = key_read(db.path.noarch.tty.flags, "__TTY_TERM_ROWS__")
                msg(progress() .. "Configure number of terminal's rows Typical value for VT100 is 24 rows.")
                msg("Current choice is: " .. choice .. " rows.")
                choice = get_number("dec", 4, 100)
                if (can_be_saved(choice)) then
                        key_save(db.path.noarch.tty.flags, "__TTY_TERM_ROWS__", choice)
                end

                return choice
        end

        local function configure_out_stream_len()
                local choice = key_read(db.path.noarch.tty.flags, "__TTY_OUT_STREAM_LEN__")
                msg(progress() .. "Configure output stream length. This value determine output buffer length in the read access to TTY.")

                msg("Current choice is: " .. choice .. " characters.")
                choice = get_number("dec", 32, 256)
                if (can_be_saved(choice)) then
                        key_save(db.path.noarch.tty.flags, "__TTY_OUT_STREAM_LEN__", choice)
                end

                return choice
        end

        local function configure_number_of_terminals()
                local choice = key_read(db.path.noarch.tty.flags, "__TTY_NUMBER_OF_TERM__")
                msg(progress() .. "Configure the number of supported virtual terminals.")
                msg("Current choice is: " .. choice .. ".")
                for i = 1, 4 do
                        add_item(i, i)
                end
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save(db.path.noarch.tty.flags, "__TTY_NUMBER_OF_TERM__", choice)
                end

                return choice
        end

        local function configure_term_size_check_enable()
                local choice = key_read(db.path.noarch.tty.flags, "__TTY_ENABLE_TERM_SIZE_CHECK__")
                msg(progress() .. "Do you want to enable terminal size check? If enabled, then virtual terminal will check real terminal size from host.")
                msg("Current choice is: " .. filter_yes_no(choice) .. ".")
                add_item(yes, "Yes")
                add_item(no, "No")
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save(db.path.noarch.tty.flags, "__TTY_ENABLE_TERM_SIZE_CHECK__", choice)
                end

                return choice
        end

        local function configure_term_in_file()
                local choice = key_read(db.path.noarch.tty.flags, "__TTY_TERM_IN_FILE__")
                msg(progress() .. "Enter terminal input file. File must support multithreading. "..
                                  "Both output and input files, can be the same file; typically it is an UART device file.")
                msg("Current file is: " .. choice .. ".")
                choice = get_string()
                if (can_be_saved(choice)) then
                        key_save(db.path.noarch.tty.flags, "__TTY_TERM_IN_FILE__", choice)
                end

                return choice
        end

        local function configure_term_out_file()
                local choice = key_read(db.path.noarch.tty.flags, "__TTY_TERM_OUT_FILE__")
                msg(progress() .. "Enter terminal output file. File must support multithreading. "..
                                  "Both output and input files, can be the same file; typically it is an UART device file.")
                msg("Current file is: " .. choice .. ".")
                choice = get_string()
                if (can_be_saved(choice)) then
                        key_save(db.path.noarch.tty.flags, "__TTY_TERM_OUT_FILE__", choice)
                end

                return choice
        end

        local function print_summary()
                local cols            = key_read(db.path.noarch.tty.flags, "__TTY_TERM_COLS__")
                local rows            = key_read(db.path.noarch.tty.flags, "__TTY_TERM_ROWS__")
                local out_stream_len  = key_read(db.path.noarch.tty.flags, "__TTY_OUT_STREAM_LEN__")
                local no_of_term      = key_read(db.path.noarch.tty.flags, "__TTY_NUMBER_OF_TERM__")
                local term_size_check = key_read(db.path.noarch.tty.flags, "__TTY_ENABLE_TERM_SIZE_CHECK__")
                local in_file         = key_read(db.path.noarch.tty.flags, "__TTY_TERM_IN_FILE__")
                local out_file        = key_read(db.path.noarch.tty.flags, "__TTY_TERM_OUT_FILE__")

                msg("TTY module configuration summary:")
                msg("Terminal size: "..cols.."x"..rows.."\n"..
                    "Output stream length: "..out_stream_len.."\n"..
                    "Number of virtual terminals: "..no_of_term.."\n"..
                    "Termial size check: "..filter_yes_no(term_size_check).."\n"..
                    "Input file: "..in_file.."\n"..
                    "Output file: "..out_file)

                pause()
        end

        if key_read(db.path.project.mk, "ENABLE_TTY") == yes then

                ::term_cols::     if configure_term_cols()              == back then return back       end
                ::term_rows::     if configure_term_rows()              == back then goto term_cols    end
                ::term_os::       if configure_out_stream_len()         == back then goto term_rows    end
                ::no_of_term::    if configure_number_of_terminals()    == back then goto term_os      end
                ::term_size_ch::  if configure_term_size_check_enable() == back then goto no_of_term   end
                ::term_in_file::  if configure_term_in_file()           == back then goto term_size_ch end
                ::term_out_file:: if configure_term_out_file()          == back then goto term_in_file end

                print_summary()
        end

        return next
end

--------------------------------------------------------------------------------
-- METHODS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Function execute configuration
--------------------------------------------------------------------------------
function tty:configure()
        title("TTY module configuration")
        navigation("Home/Modules/TTY")

        ::enable::
        if ask_for_enable() == back then
                return back
        end

        if key_read(db.path.project.mk, "ENABLE_TTY") == yes then
                if configure_tty() == back then
                        goto enable
                end
        end

        return next
end

-- started without master file
if (master ~= true) then
        show_no_master_info()
end

--------------------------------------------------------------------------------
-- END OF FILE
--------------------------------------------------------------------------------
