#include "heads.hpp"
std::string exePath;
//===添加命令处理函数开始===//

int _Settings_cmd(int argc, std::vector<std::string>::iterator argv) {
    if (!sll::ifstate_now()) return sll::IFSTATES_FALSE;
    std::stringstream cmdvcp(subcmd);
    std::string rc, sett, state;
    cmdvcp >> rc >> sett >> state;
    if (rc == "m" || rc == "modify") {
        if (sett == "sendLog") {
            if (state == "on" || state == "true" || state == "1")
                sll::settings.sendLog = true;
            else if (state == "off" || state == "false" || state == "0")
                sll::settings.sendLog = false;
            else {
                sll::sendError("Unknown state '" + state + "'");
                return 0;
            }
        }
        else if (sett == "sendWarn") {
            if (state == "on" || state == "true" || state == "1")
                sll::settings.sendWarn = true;
            else if (state == "off" || state == "false" || state == "0")
                sll::settings.sendWarn = false;
            else {
                sll::sendError("Unknown state '" + state + "'");
                return 0;
            }
        }
        else if (sett == "safeMode") {
            if (state == "on" || state == "true" || state == "1")
                sll::settings.safeMode = true;
            else if (state == "off" || state == "false" || state == "0")
                sll::settings.safeMode = false;
            else {
                sll::sendError("Unknown state '" + state + "'");
                return 0;
            }
        }
        else {
            sll::sendError("Unknown setting option '" + sett + "'");
            return 0;
        }
        sll::sendLog("Setting option '" + sett + "' has been set to '" + state + "'");
        return 0;
    }
    else if (rc == "q" || rc == "query") {
        std::stringstream msgtemp;
        if (sett == "sendLog")
            msgtemp << "sendLog = " << sll::atob<bool, std::string>(sll::settings.sendLog);
        else if (sett == "sendWarn")
            msgtemp << "sendWarn = " << sll::atob<bool, std::string>(sll::settings.sendWarn);
        else if (sett == "safeMode")
            msgtemp << "safeMode = " << sll::atob<bool, std::string>(sll::settings.safeMode);
        else if (sett == "all" || sett == "*")
            msgtemp << "sendLog = " << sll::atob<bool, std::string>(sll::settings.sendLog) << std::endl
            << "sendWarn = " << sll::atob<bool, std::string>(sll::settings.sendWarn) << std::endl
            << "safeMode = " << sll::atob<bool, std::string>(sll::settings.safeMode);
        else {
            sll::sendError("Unknown setting option '" + sett + "'");
            return 0;
        }
        sll::sendInfo(msgtemp.str());
    }
    else {
        sll::sendError("Unknown subcommand '" + rc + "'");
    }
    return 0;
}
int _System_sqcmd(int argc, std::vector<std::string>::iterator argv) {
    if (!sll::ifstate_now()) return sll::IFSTATES_FALSE;
    sll::sendLog("Run system command");
    system(subcmd.c_str());
    return 0;
}
int output(int argc, std::vector<std::string>::iterator argv) {
    if (!sll::ifstate_now()) return sll::IFSTATES_FALSE;
    sll::sendOutput(subcmd, true);
    return 0;
}
int _Exit_sqcmd(int argc, std::vector<std::string>::iterator argv) {
    if (!sll::ifstate_now()) return sll::IFSTATES_FALSE;
    return sll::EXIT_MAIN;
}
int runfile(int argc, std::vector<std::string>::iterator argv) {
    if (!sll::ifstate_now()) return sll::IFSTATES_FALSE;
    std::string temp;
    std::vector<std::string> cmdlines;
    std::ifstream rf(subcmd.c_str());
    if (rf) {
        while (!rf.eof()) {
            std::getline(rf, temp);
            cmdlines.push_back(temp);
        }
    }
    else {
        sll::sendError("File '" + subcmd + "' does not exist");
        return 1;
    }
    rf.close();

    for (int i = 0; i < cmdlines.size(); i++) {
        for (int j = 0; j < cmdlines[i].size(); j++) {  //去掉行首空格
            if (cmdlines[i][j] != ' ') {
                cmdlines[i] = cmdlines[i].substr(j, sll::MAXN);
                break;
            }
        }
    }
loopagain:
    for (std::vector<std::string>::iterator i = cmdlines.begin(); i != cmdlines.end(); i++) {
        if (i->empty() || i->at(0) == '#')   //空行和'#'开头的注释不执行
            continue;
        else if (*i == "loop")
            if (sll::ifstate_now())
                goto loopagain;
            else;
        else if (*i == "(end)")   //文件关键字
            if (sll::ifstate_now())
                return 0;
            else;
        else if (sll::command.run(*i) == sll::EXIT_MAIN)
            return sll::EXIT_MAIN;
    }
    return 0;
}
int _Var_sqcmd(int argc, std::vector<std::string>::iterator argv) {
    if (!sll::ifstate_now()) return sll::IFSTATES_FALSE;
    std::string temp_rc;
    std::string temp_cg1;
    std::string temp_cg2;
    std::string temp_cg3;
    std::stringstream trc(subcmd);
    trc >> temp_rc >> temp_cg1 >> temp_cg2 >> temp_cg3;
    if (temp_rc == "new" || temp_rc == "create" || temp_rc == "def" || temp_rc == "define") {
        if (sll::var_list.count(temp_cg1) == 1) {
            sll::sendWarn("The variable '" + temp_cg1 + "' was already exists");
        }
        else {
            sll::var_list.insert(make_pair(temp_cg1, 0));
            sll::sendLog("New variable '" + temp_cg1 + "' has been created");
        }
    }
    else if (temp_rc == "list") {
        std::stringstream msgtemp;
        msgtemp << "There are " << sll::var_list.size() << " variables exist:" << std::endl;
        for (std::map<std::string, double>::iterator it = sll::var_list.begin(); it != sll::var_list.end(); it++)
            msgtemp << it->first << " = " << it->second << std::endl;
        sll::sendInfo(msgtemp.str());
    }
    else if (temp_rc == "operation" || temp_rc == "ope") {
        if (sll::var_list.count(temp_cg1) == 0) {
            sll::sendWarn("The variable '" + temp_cg1 + "' does not exist");
        }
        else {
            double cg_temp = sll::atob<std::string, double>(temp_cg3);
            if (temp_cg2 == "+" || temp_cg2 == "add" || temp_cg2 == "plus") {
                sll::var_list[temp_cg1] += cg_temp;
                std::stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been added by " << cg_temp << " (now " << sll::var_list[temp_cg1] << ")";
                sll::sendLog(msgtemp.str());
            }
            else if (temp_cg2 == "-" || temp_cg2 == "remove" || temp_cg2 == "minus") {
                sll::var_list[temp_cg1] -= cg_temp;
                std::stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been removed by " << cg_temp << " (now " << sll::var_list[temp_cg1] << ")";
                sll::sendLog(msgtemp.str());
            }
            else if (temp_cg2 == "*" || temp_cg2 == "multiply") {
                sll::var_list[temp_cg1] *= cg_temp;
                std::stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been multiplied by " << cg_temp << " (now " << sll::var_list[temp_cg1] << ")";
                sll::sendLog(msgtemp.str());
            }
            else if (temp_cg2 == "/" || temp_cg2 == "divide") {
                if (cg_temp == 0) sll::sendWarn("Cannot be divided by 0");
                else {
                    sll::var_list[temp_cg1] /= cg_temp;
                    std::stringstream msgtemp;
                    msgtemp << "Variable '" << temp_cg1 << "' has been divided by " << cg_temp << " (now " << sll::var_list[temp_cg1] << ")";
                    sll::sendLog(msgtemp.str());
                }
            }
            else if (temp_cg2 == "=" || temp_cg2 == "set") {
                sll::var_list[temp_cg1] = cg_temp;
                std::stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been setted to " << sll::var_list[temp_cg1];
                sll::sendLog(msgtemp.str());
            }
            else if (temp_cg2 == "pow" || temp_cg2 == "power" || temp_cg2 == "^") {
                sll::var_list[temp_cg1] = pow(sll::var_list[temp_cg1], cg_temp);
                std::stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been setted to " << sll::var_list[temp_cg1];
                sll::sendLog(msgtemp.str());
            }
            else {
                sll::sendError("Unknown oprator '" + temp_cg2 + "'");
            }
        }
    }
    else if (temp_rc == "delete" || temp_rc == "del" || temp_rc == "undef" || temp_rc == "define") {
        if (sll::var_list.count(temp_cg1) == 1) {
            sll::var_list.erase(temp_cg1);
            sll::sendLog("The variable '" + temp_cg1 + "' has been deleted");
        }
        else {
            sll::sendWarn("The variable '" + temp_cg1 + "' dose not exist");
        }
    }
    else {
        std::stringstream msgtemp;
        msgtemp << "Unknown subcommand '" << temp_rc << "'";
        sll::sendError(msgtemp.str());
    }
    return 0;
}
int _If_sqcmd(int argc, std::vector<std::string>::iterator argv) {
    if (!sll::ifstate_now()) return sll::IFSTATES_FALSE;
    std::stringstream comps(subcmd);
    sll::_tIfstate temp;
    comps >> temp.x1 >> temp.oprt >> temp.x2;
    sll::ifstatu.push_back(temp);
    return 0;
}
int _Endif_sqcmd(int argc, std::vector<std::string>::iterator argv) {
    if(!sll::ifstatu.empty()) sll::ifstatu.pop_back();
    return 0;
}
int _Waitfor_sqcmd(int argc, std::vector<std::string>::iterator argv) {
    if (!sll::ifstate_now()) return sll::IFSTATES_FALSE;
    int tm = sll::atob<std::string, int>(subcmd);
    clock_t st;
    st = clock();
    while (st > clock() - tm);
    return 0;
}

/*
    请使用这个格式来添加命令处理函数：
    int 函数名(std::string subcmd){...}
    subcmd是子命令字符串，即输入的命令文本除去根命令后第一个非空格字符开始的子字符串。

    为了使命令的运行受if状态的控制，除了无视if的特殊命令，请在函数的第一行加上：
    if (!sll::ifstate_now()) return sll::IFSTATES_FALSE;
    这样当if命令创建的条件判断不通过时处理函数将不会执行并返回。
*/

//===添加命令处理函数结束===//

void regist_command(void) { //注册命令
    sll::cmd_register.clear();

    sll::regcmd("settings", _Settings_cmd,  4, slt::tArgcp::less);
    sll::regcmd("system",   _System_sqcmd,  2, slt::tArgcp::mustmatch);
    sll::regcmd("output",   output,         2, slt::tArgcp::mustmatch);
    sll::regcmd("print",    output,         2, slt::tArgcp::mustmatch);
    sll::regcmd("echo",     output,         2, slt::tArgcp::mustmatch);
    sll::regcmd("exit",     _Exit_sqcmd,    1, slt::tArgcp::mustmatch);
    sll::regcmd("runfile",  runfile,        2, slt::tArgcp::mustmatch);
    sll::regcmd("script",   runfile,        2, slt::tArgcp::mustmatch);
    sll::regcmd("scr",      runfile,        2, slt::tArgcp::mustmatch);
    sll::regcmd("var",      _Var_sqcmd,     5, slt::tArgcp::less);
    sll::regcmd("variable", _Var_sqcmd,     5, slt::tArgcp::less);
    sll::regcmd("if",       _If_sqcmd,      4, slt::tArgcp::mustmatch);
    sll::regcmd("(endif)",  _Endif_sqcmd,   1, slt::tArgcp::mustmatch);
    sll::regcmd("wait",     _Waitfor_sqcmd, 2, slt::tArgcp::mustmatch);
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
        std::cout << ">>>";
        std::getline(std::cin, inp_com);
        if (sll::command.run(inp_com) == sll::EXIT_MAIN)
            return 0;
    }
}