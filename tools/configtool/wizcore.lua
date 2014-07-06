require("wx")

wizcore = {}
wizcore.WINDOW_X_SIZE    = 800
wizcore.WINDOW_Y_SIZE    = 600
wizcore.CONTROL_X_SIZE   = 550
wizcore.HEADER_FONT_SIZE = 14

wizcore.PROJECT = {}
wizcore.PROJECT.FILE = {}
wizcore.PROJECT.KEY = {}
wizcore.PROJECT.DEF = {}
wizcore.PROJECT.ARCH_LIST                                       = {"stm32f1"}

wizcore.PROJECT.FILE.FLAGS_H                                    = "../../config/project/flags.h"
wizcore.PROJECT.FILE.MAKEFILE                                   = "../../config/project/Makefile"

wizcore.PROJECT.DEF.NO                                          = "__NO__"
wizcore.PROJECT.DEF.YES                                         = "__YES__"
wizcore.PROJECT.DEF.STM32F1                                     = "stm32f1"

wizcore.PROJECT.KEY.PROJECT_NAME                                = {wizcore.PROJECT.FILE.MAKEFILE, "PROJECT_NAME"}
wizcore.PROJECT.KEY.PROJECT_CPU_ARCH                            = {wizcore.PROJECT.FILE.MAKEFILE, "PROJECT_CPU_ARCH"}
wizcore.PROJECT.KEY.PROJECT_TOOLCHAIN                           = {wizcore.PROJECT.FILE.MAKEFILE, "PROJECT_TOOLCHAIN"}
wizcore.PROJECT.KEY.ENABLE_LFS_MK                               = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_LFS"}
wizcore.PROJECT.KEY.ENABLE_DEVFS_MK                             = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_DEVFS"}
wizcore.PROJECT.KEY.ENABLE_PROCFS_MK                            = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_PROCFS"}
wizcore.PROJECT.KEY.ENABLE_FATFS_MK                             = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_FATFS"}
wizcore.PROJECT.KEY.ENABLE_GPIO_MK                              = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_GPIO"}
wizcore.PROJECT.KEY.ENABLE_AFIO_MK                              = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_AFIO"}
wizcore.PROJECT.KEY.ENABLE_CRC_MK                               = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_CRC"}
wizcore.PROJECT.KEY.ENABLE_ETH_MK                               = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_ETH"}
wizcore.PROJECT.KEY.ENABLE_PLL_MK                               = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_PLL"}
wizcore.PROJECT.KEY.ENABLE_SDSPI_MK                             = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_SDSPI"}
wizcore.PROJECT.KEY.ENABLE_SPI_MK                               = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_SPI"}
wizcore.PROJECT.KEY.ENABLE_TTY_MK                               = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_TTY"}
wizcore.PROJECT.KEY.ENABLE_UART_MK                              = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_UART"}
wizcore.PROJECT.KEY.ENABLE_WDG_MK                               = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_WDG"}
wizcore.PROJECT.KEY.ENABLE_I2S_MK                               = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_I2S"}
wizcore.PROJECT.KEY.ENABLE_USB_MK                               = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_USB"}
wizcore.PROJECT.KEY.ENABLE_USBOTG_MK                            = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_USBOTG"}
wizcore.PROJECT.KEY.ENABLE_I2C_MK                               = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_I2C"}
wizcore.PROJECT.KEY.ENABLE_ADC_MK                               = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_ADC"}
wizcore.PROJECT.KEY.ENABLE_DAC_MK                               = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_DAC"}
wizcore.PROJECT.KEY.ENABLE_SDIO_MK                              = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_SDIO"}
wizcore.PROJECT.KEY.ENABLE_FSMC_MK                              = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_FSMC"}
wizcore.PROJECT.KEY.ENABLE_FDMC_MK                              = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_FDMC"}
wizcore.PROJECT.KEY.ENABLE_HDMICEC_MK                           = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_HDMICEC"}
wizcore.PROJECT.KEY.ENABLE_CAN_MK                               = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_CAN"}
wizcore.PROJECT.KEY.ENABLE_NETWORK_MK                           = {wizcore.PROJECT.FILE.MAKEFILE, "ENABLE_NETWORK"}

wizcore.PROJECT.KEY.CPU_ARCH                                    = {wizcore.PROJECT.FILE.FLAGS_H, "__CPU_ARCH__"}
wizcore.PROJECT.KEY.CPU_START_FREQ                              = {wizcore.PROJECT.FILE.FLAGS_H, "__CPU_START_FREQ__"}
wizcore.PROJECT.KEY.CPU_OSC_FREQ                                = {wizcore.PROJECT.FILE.FLAGS_H, "__CPU_OSC_FREQ__"}
wizcore.PROJECT.KEY.IRQ_RTOS_KERNEL_PRIORITY                    = {wizcore.PROJECT.FILE.FLAGS_H, "__IRQ_RTOS_KERNEL_PRIORITY__"}
wizcore.PROJECT.KEY.IRQ_RTOS_SYSCALL_PRIORITY                   = {wizcore.PROJECT.FILE.FLAGS_H, "__IRQ_RTOS_SYSCALL_PRIORITY__"}
wizcore.PROJECT.KEY.IRQ_USER_PRIORITY                           = {wizcore.PROJECT.FILE.FLAGS_H, "__IRQ_USER_PRIORITY__"}
wizcore.PROJECT.KEY.HEAP_BLOCK_SIZE                             = {wizcore.PROJECT.FILE.FLAGS_H, "__HEAP_BLOCK_SIZE__"}
wizcore.PROJECT.KEY.OS_TASK_MIN_STACK_DEPTH                     = {wizcore.PROJECT.FILE.FLAGS_H, "__OS_TASK_MIN_STACK_DEPTH__ "}
wizcore.PROJECT.KEY.OS_FILE_SYSTEM_STACK_DEPTH                  = {wizcore.PROJECT.FILE.FLAGS_H, "__OS_FILE_SYSTEM_STACK_DEPTH__"}
wizcore.PROJECT.KEY.OS_IRQ_STACK_DEPTH                          = {wizcore.PROJECT.FILE.FLAGS_H, "__OS_IRQ_STACK_DEPTH__"}
wizcore.PROJECT.KEY.OS_TASK_MAX_PRIORITIES                      = {wizcore.PROJECT.FILE.FLAGS_H, "__OS_TASK_MAX_PRIORITIES__"}
wizcore.PROJECT.KEY.OS_TASK_NAME_LEN                            = {wizcore.PROJECT.FILE.FLAGS_H, "__OS_TASK_NAME_LEN__"}
wizcore.PROJECT.KEY.OS_TASK_SCHED_FREQ                          = {wizcore.PROJECT.FILE.FLAGS_H, "__OS_TASK_SCHED_FREQ__"}
wizcore.PROJECT.KEY.OS_SLEEP_ON_IDLE                            = {wizcore.PROJECT.FILE.FLAGS_H, "__OS_SLEEP_ON_IDLE__"}
wizcore.PROJECT.KEY.OS_PRINTF_ENABLE                            = {wizcore.PROJECT.FILE.FLAGS_H, "__OS_PRINTF_ENABLE__"}
wizcore.PROJECT.KEY.OS_SCANF_ENABLE                             = {wizcore.PROJECT.FILE.FLAGS_H, "__OS_SCANF_ENABLE__"}
wizcore.PROJECT.KEY.OS_SYSTEM_MSG_ENABLE                        = {wizcore.PROJECT.FILE.FLAGS_H, "__OS_SYSTEM_MSG_ENABLE__"}
wizcore.PROJECT.KEY.OS_COLOR_TERMINAL_ENABLE                    = {wizcore.PROJECT.FILE.FLAGS_H, "__OS_COLOR_TERMINAL_ENABLE__"}
wizcore.PROJECT.KEY.OS_STREAM_BUFFER_LENGTH                     = {wizcore.PROJECT.FILE.FLAGS_H, "__OS_STREAM_BUFFER_LENGTH__"}
wizcore.PROJECT.KEY.OS_PIPE_LENGTH                              = {wizcore.PROJECT.FILE.FLAGS_H, "__OS_PIPE_LENGTH__"}
wizcore.PROJECT.KEY.OS_ERRNO_STRING_LEN                         = {wizcore.PROJECT.FILE.FLAGS_H, "__OS_ERRNO_STRING_LEN__"}
wizcore.PROJECT.KEY.OS_MONITOR_TASK_MEMORY_USAGE                = {wizcore.PROJECT.FILE.FLAGS_H, "__OS_MONITOR_TASK_MEMORY_USAGE__"}
wizcore.PROJECT.KEY.OS_MONITOR_TASK_FILE_USAGE                  = {wizcore.PROJECT.FILE.FLAGS_H, "__OS_MONITOR_TASK_FILE_USAGE__"}
wizcore.PROJECT.KEY.OS_MONITOR_KERNEL_MEMORY_USAGE              = {wizcore.PROJECT.FILE.FLAGS_H, "__OS_MONITOR_KERNEL_MEMORY_USAGE__"}
wizcore.PROJECT.KEY.OS_MONITOR_MODULE_MEMORY_USAGE              = {wizcore.PROJECT.FILE.FLAGS_H, "__OS_MONITOR_MODULE_MEMORY_USAGE__"}
wizcore.PROJECT.KEY.OS_MONITOR_SYSTEM_MEMORY_USAGE              = {wizcore.PROJECT.FILE.FLAGS_H, "__OS_MONITOR_SYSTEM_MEMORY_USAGE__"}
wizcore.PROJECT.KEY.OS_MONITOR_CPU_LOAD                         = {wizcore.PROJECT.FILE.FLAGS_H, "__OS_MONITOR_CPU_LOAD__"}
wizcore.PROJECT.KEY.OS_MONITOR_NETWORK_MEMORY_USAGE             = {wizcore.PROJECT.FILE.FLAGS_H, "__OS_MONITOR_NETWORK_MEMORY_USAGE__"}
wizcore.PROJECT.KEY.OS_MONITOR_NETWORK_MEMORY_USAGE_LIMIT       = {wizcore.PROJECT.FILE.FLAGS_H, "__OS_MONITOR_NETWORK_MEMORY_USAGE_LIMIT__"}
wizcore.PROJECT.KEY.OS_HOSTNAME                                 = {wizcore.PROJECT.FILE.FLAGS_H, "__OS_HOSTNAME__"}
wizcore.PROJECT.KEY.OS_SYSTEM_STOP_MACRO                        = {wizcore.PROJECT.FILE.FLAGS_H, "__OS_SYSTEM_STOP_MACRO__"}
wizcore.PROJECT.KEY.NETWORK_ENABLE                              = {wizcore.PROJECT.FILE.FLAGS_H, "__NETWORK_ENABLE__ "}
wizcore.PROJECT.KEY.NETWORK_MAC_ADDR_0                          = {wizcore.PROJECT.FILE.FLAGS_H, "__NETWORK_MAC_ADDR_0__"}
wizcore.PROJECT.KEY.NETWORK_MAC_ADDR_1                          = {wizcore.PROJECT.FILE.FLAGS_H, "__NETWORK_MAC_ADDR_1__"}
wizcore.PROJECT.KEY.NETWORK_MAC_ADDR_2                          = {wizcore.PROJECT.FILE.FLAGS_H, "__NETWORK_MAC_ADDR_2__"}
wizcore.PROJECT.KEY.NETWORK_MAC_ADDR_3                          = {wizcore.PROJECT.FILE.FLAGS_H, "__NETWORK_MAC_ADDR_3__"}
wizcore.PROJECT.KEY.NETWORK_MAC_ADDR_4                          = {wizcore.PROJECT.FILE.FLAGS_H, "__NETWORK_MAC_ADDR_4__"}
wizcore.PROJECT.KEY.NETWORK_MAC_ADDR_5                          = {wizcore.PROJECT.FILE.FLAGS_H, "__NETWORK_MAC_ADDR_5__"}
wizcore.PROJECT.KEY.NETWORK_ETHIF_FILE                          = {wizcore.PROJECT.FILE.FLAGS_H, "__NETWORK_ETHIF_FILE__"}
wizcore.PROJECT.KEY.ENABLE_DEVFS_H                              = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_DEVFS__"}
wizcore.PROJECT.KEY.ENABLE_LFS_H                                = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_LFS__"}
wizcore.PROJECT.KEY.ENABLE_FATFS_H                              = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_FATFS__"}
wizcore.PROJECT.KEY.ENABLE_PROCFS_H                             = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_PROCFS__"}
wizcore.PROJECT.KEY.ENABLE_GPIO_H                               = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_GPIO__"}
wizcore.PROJECT.KEY.ENABLE_AFIO_H                               = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_AFIO__"}
wizcore.PROJECT.KEY.ENABLE_CRC_H                                = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_CRC__"}
wizcore.PROJECT.KEY.ENABLE_ETH_H                                = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_ETH__"}
wizcore.PROJECT.KEY.ENABLE_PLL_H                                = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_PLL__"}
wizcore.PROJECT.KEY.ENABLE_SDSPI_H                              = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_SDSPI__"}
wizcore.PROJECT.KEY.ENABLE_SPI_H                                = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_SPI__"}
wizcore.PROJECT.KEY.ENABLE_TTY_H                                = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_TTY__"}
wizcore.PROJECT.KEY.ENABLE_UART_H                               = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_UART__"}
wizcore.PROJECT.KEY.ENABLE_WDG_H                                = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_WDG__"}
wizcore.PROJECT.KEY.ENABLE_I2S_H                                = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_I2S__"}
wizcore.PROJECT.KEY.ENABLE_USB_H                                = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_USB__"}
wizcore.PROJECT.KEY.ENABLE_USBOTG_H                             = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_USBOTG__"}
wizcore.PROJECT.KEY.ENABLE_I2C_H                                = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_I2C__"}
wizcore.PROJECT.KEY.ENABLE_ADC_H                                = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_ADC__"}
wizcore.PROJECT.KEY.ENABLE_DAC_H                                = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_DAC__"}
wizcore.PROJECT.KEY.ENABLE_SDIO_H                               = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_SDIO__"}
wizcore.PROJECT.KEY.ENABLE_FSMC_H                               = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_FSMC__"}
wizcore.PROJECT.KEY.ENABLE_FDMC_H                               = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_FDMC__"}
wizcore.PROJECT.KEY.ENABLE_HDMICEC_H                            = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_HDMICEC__"}
wizcore.PROJECT.KEY.ENABLE_CAN_H                                = {wizcore.PROJECT.FILE.FLAGS_H, "__ENABLE_CAN__"}

wizcore.ARCH                                                    = {}
wizcore.ARCH.STM32F1                                            = {}
wizcore.ARCH.STM32F1.FILE                                       = {}
wizcore.ARCH.STM32F1.FILE.CPU_H                                 = "../../config/stm32f1/cpu.h"
wizcore.ARCH.STM32F1.FILE.MAKEFILE                              = "../../config/stm32f1/Makefile"
wizcore.ARCH.STM32F1.KEY                                        = {}
wizcore.ARCH.STM32F1.KEY.CPU_NAME                               = {wizcore.ARCH.STM32F1.FILE.CPU_H, "__CPU_NAME__"}
wizcore.ARCH.STM32F1.KEY.CPU_FAMILY                             = {wizcore.ARCH.STM32F1.FILE.CPU_H, "__CPU_FAMILY__"}
wizcore.ARCH.STM32F1.CPU_LIST                                   = {"STM32F100C8xx",
                                                                   "STM32F100RBxx",
                                                                   "STM32F100RExx",
                                                                   "STM32F100VBxx",
                                                                   "STM32F100ZCxx",
                                                                   "STM32F101C8xx",
                                                                   "STM32F101CBxx",
                                                                   "STM32F101R8xx",
                                                                   "STM32F101RBxx",
                                                                   "STM32F101V8xx",
                                                                   "STM32F101VBxx",
                                                                   "STM32F103C6xx",
                                                                   "STM32F103C8xx",
                                                                   "STM32F103CBxx",
                                                                   "STM32F103R6xx",
                                                                   "STM32F103R8xx",
                                                                   "STM32F103RBxx",
                                                                   "STM32F103RCxx",
                                                                   "STM32F103RDxx",
                                                                   "STM32F103RExx",
                                                                   "STM32F103RGxx",
                                                                   "STM32F103T8xx",
                                                                   "STM32F103V8xx",
                                                                   "STM32F103VBxx",
                                                                   "STM32F103VCxx",
                                                                   "STM32F103VDxx",
                                                                   "STM32F103VExx",
                                                                   "STM32F103VGxx",
                                                                   "STM32F103ZCxx",
                                                                   "STM32F103ZExx",
                                                                   "STM32F103ZGxx",
                                                                   "STM32F105RBxx",
                                                                   "STM32F105RCxx",
                                                                   "STM32F105VBxx",
                                                                   "STM32F105VCxx",
                                                                   "STM32F107RCxx",
                                                                   "STM32F107VBxx",
                                                                   "STM32F107VCxx"}
wizcore.ARCH.STM32F1.CPU_PRIORITY_LIST                          = {"Priority 0 (the highest)",
                                                                   "Priority 1",
                                                                   "Priority 2",
                                                                   "Priority 3",
                                                                   "Priority 4",
                                                                   "Priority 5",
                                                                   "Priority 6",
                                                                   "Priority 7",
                                                                   "Priority 8",
                                                                   "Priority 9",
                                                                   "Priority 10",
                                                                   "Priority 11",
                                                                   "Priority 12",
                                                                   "Priority 13 (the lowest)"}


local FILETYPE_HEADER   = 0
local FILETYPE_MAKEFILE = 1


local function line_found(line, filetype, key)
        if filetype == FILETYPE_HEADER then
                return line:match('^%s*#define%s+'..key..'%s*.*')

        elseif filetype == FILETYPE_MAKEFILE then
                return line:match('^%s*'..key..'%s*=%s*.*')
        end

        return false
end


local function modify_line(line, filetype, key, value)
        if filetype == FILETYPE_HEADER then
                return "#define "..key.." "..value
        elseif filetype == FILETYPE_MAKEFILE then
                return key.." = "..value
        end

        return line
end


local function get_line_value(line, filetype, key)
        if filetype == FILETYPE_HEADER then
                local _, _, value = line:find("%s*#define%s+"..key.."%s+(.*)")
                return value
        elseif filetype == FILETYPE_MAKEFILE then
                local _, _, value = line:find("%s*"..key.."%s*=%s*(.*)")
                return value
        end

        return ""
end


function wizcore:show_error_msg(title, caption)
        print("["..title.."] "..caption)
        dialog = wx.wxMessageDialog(wx.NULL, caption, title, bit.bor(wx.wxOK, wx.wxICON_ERROR))
        dialog:ShowModal()
        wx.wxGetApp():ExitMainLoop()
end


function wizcore:show_info_msg(title, caption)
        print("["..title.."] "..caption)
        dialog = wx.wxMessageDialog(wx.NULL, caption, title, bit.bor(wx.wxOK, wx.wxICON_INFORMATION))
        dialog:ShowModal()
end


function wizcore:show_question_msg(title, caption)
        print("["..title.."] "..caption)
        dialog = wx.wxMessageDialog(wx.NULL, caption, title, bit.bor(wx.wxYES_NO, wx.wxICON_QUESTION))
        return dialog:ShowModal()
end


function wizcore:get_window_size()
        return wizcore.WINDOW_X_SIZE, wizcore.WINDOW_Y_SIZE
end


function wizcore:key_write(keypath, value)
        local filename = keypath[1]
        local key      = keypath[2]

        -- type check
        if type(filename) ~= "string" or type(key) ~= "string" or type(value) ~= "string" then
                wizcore:show_error_msg("Error", "key_write(): Invalid type of 'filename' or 'key' or 'value'\n"..debug.traceback())
                return false
        end

        -- read file
        local file = io.open(filename, "r")
        if file == nil then
                wizcore:show_error_msg("Error", "key_write(): "..filename..": Cannot open file specified\n"..debug.traceback())
                return false
        end

        -- file type check
        local filetype
        if filename:find(".h") or filename:find(".hpp") or filename:find(".hh") or filename:find(".hxx") then
                filetype = FILETYPE_HEADER
        elseif filename:find(".mk") or filename:find(".mak") or filename:find(".makefile") or filename:find("Makefile") or filename:find("makefile") then
                filetype = FILETYPE_MAKEFILE
        else
                wizcore:show_error_msg("Error", "key_read(): Unknown file type\n"..debug.traceback())
                return false
        end

        -- key modifing
        local lines = {}
        local line_cnt = 1
        for line in file:lines() do

                if line_found(line, filetype, key) then
                        lines[#lines + 1] = modify_line(line, filetype, key, value)
                else
                        lines[#lines + 1] = line
                end

                line_cnt = line_cnt + 1
        end
        file:close()

        -- write the file.
        file = io.open(filename, "w")
        if file == nil then
                wizcore:show_error_msg("Error", "key_write(): File write protected\n"..debug.traceback())
                return false
        end

        for i, line in ipairs(lines) do
                file:write(line, "\n")
        end
        file:close()

        return true
end


function wizcore:key_read(keypath)
        local filename = keypath[1]
        local key      = keypath[2]

        -- type check
        if type(filename) ~= "string" or type(key) ~= "string" then
                wizcore:show_error_msg("Error", "key_read(): Invalid type of 'filename' or 'key'\n"..debug.traceback())
                return false
        end

        -- read file
        local file = io.open(filename, "r")
        if file == nil then
                wizcore:show_error_msg("Error", "key_read(): "..filename..": Cannot open specified file\n"..debug.traceback())
                return false
        end

        -- file type check
        local filetype
        if filename:find(".h") or filename:find(".hpp") or filename:find(".hh") or filename:find(".hxx") then
                filetype = FILETYPE_HEADER
        elseif filename:find(".mk") or filename:find(".mak") or filename:find(".makefile") or filename:find("Makefile") or filename:find("makefile") then
                filetype = FILETYPE_MAKEFILE
        else
                wizcore:show_error_msg("Error", "key_read(): Unknown file type\n"..debug.traceback())
                return false
        end

        -- key finding
        local value = nil
        for line in file:lines() do
                if line_found(line, filetype, key) then
                        value = get_line_value(line, filetype, key)
                        break
                end
        end
        file:close()

        if value == nil then
                wizcore:show_error_msg("Error", "key_read(): "..key..": key not found\n"..debug.traceback())
        end

        return value
end


function wizcore:get_string_index(table, string)
        for i, s in ipairs(table) do
                if s:match(string) then
                        return i
                end
        end

        return -1
end


function wizcore:enable_module(name, state)
--TODO
end


function wizcore:get_module_state(name)
--TODO
end

