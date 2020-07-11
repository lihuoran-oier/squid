#include "heads.hpp"
std::string exePath;
//===添加命令处理函数开始===//

int _Settings_cmd(const lcmd& args) {
    if (!sll::ifstate_now()) return sll::IFSTATES_FALSE;
    if (args[1] == "m" || args[1] == "modify") {
        if (args[2] == "sendLog") {
            if (args[3] == "on" || args[3] == "true" || args[3] == "1")
                sll::settings.sendLog = true;
            else if (args[3] == "off" || args[3] == "false" || args[3] == "0")
                sll::settings.sendLog = false;
            else {
                sll::sendError("Unknown state '" + args[3] + "'");
                return 0;
            }
        }
        else if (args[2] == "sendWarn") {
            if (args[3] == "on" || args[3] == "true" || args[3] == "1")
                sll::settings.sendWarn = true;
            else if (args[3] == "off" || args[3] == "false" || args[3] == "0")
                sll::settings.sendWarn = false;
            else {
                sll::sendError("Unknown state '" + args[3] + "'");
                return 0;
            }
        }
        else if (args[2] == "safeMode") {
            if (args[3] == "on" || args[3] == "true" || args[3] == "1")
                sll::settings.safeMode = true;
            else if (args[3] == "off" || args[3] == "false" || args[3] == "0")
                sll::settings.safeMode = false;
            else {
                sll::sendError("Unknown state '" + args[3] + "'");
                return 0;
            }
        }
        else {
            sll::sendError("Unknown setting option '" + args[2] + "'");
            return 0;
        }
        sll::sendLog("Setting option '" + args[2] + "' has been set to '" + args[2] + "'");
        return 0;
    }
    else if (args[1] == "q" || args[1] == "query") {
        std::stringstream msgtemp;
        if (args[2] == "sendLog")
            msgtemp << "sendLog = " << sll::atob<bool, std::string>(sll::settings.sendLog);
        else if (args[2] == "sendWarn")
            msgtemp << "sendWarn = " << sll::atob<bool, std::string>(sll::settings.sendWarn);
        else if (args[2] == "safeMode")
            msgtemp << "safeMode = " << sll::atob<bool, std::string>(sll::settings.safeMode);
        else if (args[2] == "all" || args[2] == "*")
            msgtemp << "sendLog = " << sll::atob<bool, std::string>(sll::settings.sendLog) << std::endl
            << "sendWarn = " << sll::atob<bool, std::string>(sll::settings.sendWarn) << std::endl
            << "safeMode = " << sll::atob<bool, std::string>(sll::settings.safeMode);
        else {
            sll::sendError("Unknown setting option '" + args[2] + "'");
            return 0;
        }
        sll::sendInfo(msgtemp.str());
    }
    else {
        sll::sendError("Unknown subcommand '" + args[1] + "'");
    }
    return 0;
}
int _System_sqcmd(const lcmd &args) {
    if (!sll::ifstate_now()) return sll::IFSTATES_FALSE;
    sll::sendLog("Run system command");
    system(args[1].c_str());
    return 0;
}
int output(const lcmd &args) {
    if (!sll::ifstate_now()) return sll::IFSTATES_FALSE;
    sll::sendOutput(args[1], true);
    return 0;
}
int _Exit_sqcmd(const lcmd &args) {
    if (!sll::ifstate_now()) return sll::IFSTATES_FALSE;
    return sll::EXIT_MAIN;
}
int runfile(const lcmd &args) {
    if (!sll::ifstate_now()) return sll::IFSTATES_FALSE;
    std::ifstream rf(args[1].c_str());
    if (rf) {
        std::stringstream rawf;
        rawf << rf.rdbuf();
        std::string stemp = rawf.str();
        sll::command.run(stemp);
    }
    else {
        sll::sendError("File " + args[1] + " dose not exist");
    }
}
int _loop_Sqcmd(const lcmd &args) {
    return 0;
}
int _Endloop_Sqcmd(const lcmd &args) {
    return 0;
}
int _Var_sqcmd(const lcmd &args) {
    if (!sll::ifstate_now()) return sll::IFSTATES_FALSE;
    if (args[1] == "new" || args[1] == "create" || args[1] == "def" || args[1] == "define") {
        if (sll::var_list.count(args[2]) == 1) {
            sll::sendWarn("The variable '" + args[2] + "' was already exists");
        }
        else {
            sll::var_list.insert(make_pair(args[2], 0));
            sll::sendLog("New variable '" + args[2] + "' has been created");
        }
    }
    else if (args[1] == "list") {
        std::stringstream msgtemp;
        msgtemp << "There are " << sll::var_list.size() << " variables exist:" << std::endl;
        for (std::map<std::string, double>::iterator it = sll::var_list.begin(); it != sll::var_list.end(); it++)
            msgtemp << it->first << " = " << it->second << std::endl;
        sll::sendInfo(msgtemp.str());
    }
    else if (args[1] == "operation" || args[1] == "ope") {
        if (sll::var_list.count(args[2]) == 0) {
            sll::sendWarn("The variable '" + args[2] + "' does not exist");
        }
        else {
            double cg_temp = sll::atob<std::string, double>(args[4]);
            if (args[3] == "+" || args[3] == "add" || args[3] == "plus") {
                sll::var_list[args[2]] += cg_temp;
                std::stringstream msgtemp;
                msgtemp << "Variable '" << args[2] << "' has been added by " << cg_temp << " (now " << sll::var_list[args[2]] << ")";
                sll::sendLog(msgtemp.str());
            }
            else if (args[3] == "-" || args[3] == "remove" || args[3] == "minus") {
                sll::var_list[args[2]] -= cg_temp;
                std::stringstream msgtemp;
                msgtemp << "Variable '" << args[2] << "' has been removed by " << cg_temp << " (now " << sll::var_list[args[2]] << ")";
                sll::sendLog(msgtemp.str());
            }
            else if (args[3] == "*" || args[3] == "multiply") {
                sll::var_list[args[2]] *= cg_temp;
                std::stringstream msgtemp;
                msgtemp << "Variable '" << args[2] << "' has been multiplied by " << cg_temp << " (now " << sll::var_list[args[2]] << ")";
                sll::sendLog(msgtemp.str());
            }
            else if (args[3] == "/" || args[3] == "divide") {
                if (cg_temp == 0) sll::sendWarn("Cannot be divided by 0");
                else {
                    sll::var_list[args[2]] /= cg_temp;
                    std::stringstream msgtemp;
                    msgtemp << "Variable '" << args[2] << "' has been divided by " << cg_temp << " (now " << sll::var_list[args[2]] << ")";
                    sll::sendLog(msgtemp.str());
                }
            }
            else if (args[3] == "=" || args[3] == "set") {
                sll::var_list[args[2]] = cg_temp;
                std::stringstream msgtemp;
                msgtemp << "Variable '" << args[2] << "' has been setted to " << sll::var_list[args[2]];
                sll::sendLog(msgtemp.str());
            }
            else if (args[3] == "pow" || args[3] == "power" || args[3] == "^") {
                sll::var_list[args[2]] = pow(sll::var_list[args[2]], cg_temp);
                std::stringstream msgtemp;
                msgtemp << "Variable '" << args[2] << "' has been setted to " << sll::var_list[args[2]];
                sll::sendLog(msgtemp.str());
            }
            else {
                sll::sendError("Unknown oprator '" + args[3] + "'");
            }
        }
    }
    else if (args[1] == "delete" || args[1] == "del" || args[1] == "undef" || args[1] == "define") {
        if (sll::var_list.count(args[2]) == 1) {
            sll::var_list.erase(args[2]);
            sll::sendLog("The variable '" + args[2] + "' has been deleted");
        }
        else {
            sll::sendWarn("The variable '" + args[2] + "' dose not exist");
        }
    }
    else {
        std::stringstream msgtemp;
        msgtemp << "Unknown subcommand '" << args[1] << "'";
        sll::sendError(msgtemp.str());
    }
    return 0;
}
int _If_sqcmd(const lcmd &args) {
    if (!sll::ifstate_now()) return sll::IFSTATES_FALSE;
    sll::_tIfstate temp = { sll::atob<std::string,float>(args[1]),sll::atob<std::string,float>(args[2]),args[3] };
    sll::ifstatu.push_back(temp);
    return 0;
}
int _Endif_sqcmd(const lcmd &args) {
    if(!sll::ifstatu.empty()) sll::ifstatu.pop_back();
    return 0;
}
int _Waitfor_sqcmd(const lcmd &args) {
    if (!sll::ifstate_now()) return sll::IFSTATES_FALSE;
    int tm = sll::atob<std::string, int>(args[1]);
    clock_t st;
    st = clock();
    while (st > clock() - tm);
    return 0;
}

/*
    请使用这个格式来添加命令处理函数：
    int 函数名(const lcmd &args){...}
    args是用vector<string>形式储存的命令参数列表。

    为了使命令的运行受if状态的控制，除了无视if的特殊命令，请在函数的第一行加上：
    if (!sll::ifstate_now()) return sll::IFSTATES_FALSE;
    这样当if命令创建的条件判断不通过时处理函数将不会执行并返回。
*/

//===添加命令处理函数结束===//

void regist_command(void) { //注册命令
    sll::cmd_register.clear();

    sll::regcmd("settings", _Settings_cmd,  3, 4);
    sll::regcmd("system",   _System_sqcmd,  2, 2);
    sll::regcmd("output",   output,         2, 2);
    sll::regcmd("print",    output,         2, 2);
    sll::regcmd("echo",     output,         2, 2);
    sll::regcmd("exit",     _Exit_sqcmd,    1, 1);
    sll::regcmd("runfile",  runfile,        2, 2);
    sll::regcmd("script",   runfile,        2, 2);
    sll::regcmd("scr",      runfile,        2, 2);
    sll::regcmd("var",      _Var_sqcmd,     2, 5);
    sll::regcmd("variable", _Var_sqcmd,     2, 5);
    sll::regcmd("if",       _If_sqcmd,      4, 4);
    sll::regcmd("(endif)",    _Endif_sqcmd,   1, 1);
    sll::regcmd("wait",     _Waitfor_sqcmd, 2, 2);
    /*
        请参照sll::regcmd函数的用法来注册命令。
    */
}

int main(int argc, char* argv[])
{
    regist_command();
    exePath = argv[0];
    for (int i = exePath.size() - 1; i > 0; i--) {
        if (exePath[i] == '/' || exePath[i] == '\\') {
            exePath.erase(i + 1, sll::MAXN);
            break;
        }
    }
    //从启动参数执行脚本
    if (argc >= 2) {
        std::fstream ftest(argv[1]);
        if (ftest) {
            std::string path("runfile ");
            path.append(argv[1]);
            if (sll::command.run(path) == sll::EXIT_MAIN)
                return 0;
        }
    }
    else {
        system("title SquidLang Beta v0.3 - By MineCommander");
        sll::sendOutput("SquidLang Beta  v0b3\nCopyright MineCommander (C) 2020", false);
    }
    std::string inp_com;
    while(1)
    {
        std::cout << ">>> ";
        std::getline(std::cin, inp_com);
        if (sll::command.run(inp_com) == sll::EXIT_MAIN)
            return 0;
    }
}