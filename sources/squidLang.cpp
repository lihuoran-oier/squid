#include "heads.hpp"
std::string exePath;
//===添加命令处理函数开始===//

int _Settings_cmd(std::string subcmd) {
    if (!squidlanglib::ifstate_now()) return IFSTATES_FALSE;
    std::stringstream cmdvcp(subcmd);
    std::string rc, sett, state;
    cmdvcp >> rc >> sett >> state;
    if (rc == "m" || rc == "modify") {
        if (sett == "sendLog") {
            if (state == "on" || state == "true" || state == "1")
                squidlanglib::settings.sendLog = true;
            else if (state == "off" || state == "false" || state == "0")
                squidlanglib::settings.sendLog = false;
            else {
                squidlanglib::sendError("Unknown state '" + state + "'");
                return 0;
            }
        }
        else if (sett == "sendWarn") {
            if (state == "on" || state == "true" || state == "1")
                squidlanglib::settings.sendWarn = true;
            else if (state == "off" || state == "false" || state == "0")
                squidlanglib::settings.sendWarn = false;
            else {
                squidlanglib::sendError("Unknown state '" + state + "'");
                return 0;
            }
        }
        else if (sett == "safeMode") {
            if (state == "on" || state == "true" || state == "1")
                squidlanglib::settings.safeMode = true;
            else if (state == "off" || state == "false" || state == "0")
                squidlanglib::settings.safeMode = false;
            else {
                squidlanglib::sendError("Unknown state '" + state + "'");
                return 0;
            }
        }
        else {
            squidlanglib::sendError("Unknown setting option '" + sett + "'");
            return 0;
        }
        squidlanglib::sendLog("Setting option '" + sett + "' has been set to '" + state + "'");
        return 0;
    }
    else if (rc == "q" || rc == "query") {
        std::stringstream msgtemp;
        if (sett == "sendLog")
            msgtemp << "sendLog = " << squidlanglib::atob<bool, std::string>(squidlanglib::settings.sendLog);
        else if (sett == "sendWarn")
            msgtemp << "sendWarn = " << squidlanglib::atob<bool, std::string>(squidlanglib::settings.sendWarn);
        else if (sett == "safeMode")
            msgtemp << "safeMode = " << squidlanglib::atob<bool, std::string>(squidlanglib::settings.safeMode);
        else if (sett == "all" || sett == "*")
            msgtemp << "sendLog = " << squidlanglib::atob<bool, std::string>(squidlanglib::settings.sendLog) << std::endl
            << "sendWarn = " << squidlanglib::atob<bool, std::string>(squidlanglib::settings.sendWarn) << std::endl
            << "safeMode = " << squidlanglib::atob<bool, std::string>(squidlanglib::settings.safeMode);
        else {
            squidlanglib::sendError("Unknown setting option '" + sett + "'");
            return 0;
        }
        squidlanglib::sendInfo(msgtemp.str());
    }
    else {
        squidlanglib::sendError("Unknown subcommand '" + rc + "'");
    }
}
int _System_sqcmd(std::string subcmd) {
    if (!squidlanglib::ifstate_now()) return IFSTATES_FALSE;
    squidlanglib::sendLog("Run system command");
    system(subcmd.c_str());
    return 0;
}
int output(std::string subcmd) {
    if (!squidlanglib::ifstate_now()) return IFSTATES_FALSE;
    squidlanglib::sendOutput(subcmd, true);
    return 0;
}
int _Exit_sqcmd(std::string subcmd) {
    if (!squidlanglib::ifstate_now()) return IFSTATES_FALSE;
    return EXIT_MAIN;
}
int runfile(std::string subcmd) {
    if (!squidlanglib::ifstate_now()) return IFSTATES_FALSE;
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
        squidlanglib::sendError("File '" + subcmd + "' does not exist");
        return 1;
    }
    rf.close();

    for (int i = 0; i < cmdlines.size(); i++) {
        for (int j = 0; j < cmdlines[i].size(); j++) {  //去掉行首空格
            if (cmdlines[i][j] != ' ') {
                cmdlines[i] = cmdlines[i].substr(j, MAXN);
                break;
            }
        }
    }
loopagain:
    for (std::vector<std::string>::iterator i = cmdlines.begin(); i != cmdlines.end(); i++) {
        if (i->empty() || i->at(0) == '#')   //空行和'#'开头的注释不执行
            continue;
        else if (*i == "loop")
            if (squidlanglib::ifstate_now())
                goto loopagain;
            else;
        else if (*i == "(end)")   //文件关键字
            if (squidlanglib::ifstate_now())
                return 0;
            else;
        else if (squidlanglib::command.run(*i) == EXIT_MAIN)
            return EXIT_MAIN;
    }
    return 0;
}
int _Var_sqcmd(std::string subcmd) {
    if (!squidlanglib::ifstate_now()) return IFSTATES_FALSE;
    std::string temp_rc;
    std::string temp_cg1;
    std::string temp_cg2;
    std::string temp_cg3;
    std::stringstream trc(subcmd);
    trc >> temp_rc >> temp_cg1 >> temp_cg2 >> temp_cg3;
    if (temp_rc == "new" || temp_rc == "create" || temp_rc == "def" || temp_rc == "define") {
        if (var_list.count(temp_cg1) == 1) {
            squidlanglib::sendWarn("The variable '" + temp_cg1 + "' was already exists");
        }
        else {
            var_list.insert(make_pair(temp_cg1, 0));
            squidlanglib::sendLog("New variable '" + temp_cg1 + "' has been created");
        }
    }
    else if (temp_rc == "list") {
        std::stringstream msgtemp;
        msgtemp << "There are " << var_list.size() << " variables exist:" << std::endl;
        for (std::map<std::string, double>::iterator it = var_list.begin(); it != var_list.end(); it++)
            msgtemp << it->first << " = " << it->second << std::endl;
        squidlanglib::sendInfo(msgtemp.str());
    }
    else if (temp_rc == "operation" || temp_rc == "ope") {
        if (var_list.count(temp_cg1) == 0) {
            squidlanglib::sendWarn("The variable '" + temp_cg1 + "' does not exist");
        }
        else {
            double cg_temp = squidlanglib::atob<std::string, double>(temp_cg3);
            if (temp_cg2 == "+" || temp_cg2 == "add" || temp_cg2 == "plus") {
                var_list[temp_cg1] += cg_temp;
                std::stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been added by " << cg_temp << " (now " << var_list[temp_cg1] << ")";
                squidlanglib::sendLog(msgtemp.str());
            }
            else if (temp_cg2 == "-" || temp_cg2 == "remove" || temp_cg2 == "minus") {
                var_list[temp_cg1] -= cg_temp;
                std::stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been removed by " << cg_temp << " (now " << var_list[temp_cg1] << ")";
                squidlanglib::sendLog(msgtemp.str());
            }
            else if (temp_cg2 == "*" || temp_cg2 == "multiply") {
                var_list[temp_cg1] *= cg_temp;
                std::stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been multiplied by " << cg_temp << " (now " << var_list[temp_cg1] << ")";
                squidlanglib::sendLog(msgtemp.str());
            }
            else if (temp_cg2 == "/" || temp_cg2 == "divide") {
                if (cg_temp == 0) squidlanglib::sendWarn("Cannot be divided by 0");
                else {
                    var_list[temp_cg1] /= cg_temp;
                    std::stringstream msgtemp;
                    msgtemp << "Variable '" << temp_cg1 << "' has been divided by " << cg_temp << " (now " << var_list[temp_cg1] << ")";
                    squidlanglib::sendLog(msgtemp.str());
                }
            }
            else if (temp_cg2 == "=" || temp_cg2 == "set") {
                var_list[temp_cg1] = cg_temp;
                std::stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been setted to " << var_list[temp_cg1];
                squidlanglib::sendLog(msgtemp.str());
            }
            else if (temp_cg2 == "pow" || temp_cg2 == "power" || temp_cg2 == "^") {
                var_list[temp_cg1] = pow(var_list[temp_cg1], cg_temp);
                std::stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been setted to " << var_list[temp_cg1];
                squidlanglib::sendLog(msgtemp.str());
            }
            else {
                squidlanglib::sendError("Unknown oprator '" + temp_cg2 + "'");
            }
        }
    }
    else if (temp_rc == "delete" || temp_rc == "del" || temp_rc == "undef" || temp_rc == "define") {
        if (var_list.count(temp_cg1) == 1) {
            var_list.erase(temp_cg1);
            squidlanglib::sendLog("The variable '" + temp_cg1 + "' has been deleted");
        }
        else {
            squidlanglib::sendWarn("The variable '" + temp_cg1 + "' dose not exist");
        }
    }
    else {
        std::stringstream msgtemp;
        msgtemp << "Unknown subcommand '" << temp_rc << "'";
        squidlanglib::sendError(msgtemp.str());
    }
    return 0;
}
int _If_sqcmd(std::string subcmd) {
    if (!squidlanglib::ifstate_now()) return IFSTATES_FALSE;
    std::stringstream comps(subcmd);
    _tIfstate temp;
    comps >> temp.x1 >> temp.oprt >> temp.x2;
    ifstatu.push(temp);
    return 0;
}
int _Endif_sqcmd(std::string subcmd) {
    ifstatu.pop();
    return 0;
}
int _Waitfor_sqcmd(std::string subcmd) {
    if (!squidlanglib::ifstate_now()) return IFSTATES_FALSE;
    int tm = squidlanglib::atob<std::string, int>(subcmd);
    clock_t st;
    st = clock();
    while (st > clock() - tm);
    return 0;
}

/*
    请使用这个格式来添加命令处理函数：
    int 函数名(string subcmd){...}
    subcmd是子命令字符串，即输入的命令文本除去根命令后第一个非空格字符开始的子字符串。

    为了使命令的运行受if状态的控制，除了无视if的特殊命令，请在函数的第一行加上：
    if (!squidlanglib::ifstate_now()) return IFSTATES_FALSE;
    这样当if命令创建的条件判断不通过时处理函数将不会执行并返回。
*/

//===添加命令处理函数结束===//

void regist_command(void) { //注册命令
    cmd_register.clear();

    squidlanglib::regcmd("settings", _Settings_cmd);
    squidlanglib::regcmd("system", _System_sqcmd);
    squidlanglib::regcmd("output", output);
    squidlanglib::regcmd("print", output);
    squidlanglib::regcmd("echo", output);
    squidlanglib::regcmd("exit", _Exit_sqcmd);
    squidlanglib::regcmd("runfile", runfile);
    squidlanglib::regcmd("script", runfile);
    squidlanglib::regcmd("scr", runfile);
    squidlanglib::regcmd("var", _Var_sqcmd);
    squidlanglib::regcmd("variable", _Var_sqcmd);
    squidlanglib::regcmd("if", _If_sqcmd);
    squidlanglib::regcmd("(endif)", _Endif_sqcmd);
    squidlanglib::regcmd("wait", _Waitfor_sqcmd);
    /*
        请使用这个格式来注册命令：
        squidlib.regcmd("根命令字符串", 函数名指针);
        若执行的根命令与字符串匹配，将会调用函数名指针指向的函数。
    */
}

int main(int argc, char* argv[])
{
    regist_command();
    exePath = argv[0];
    for (int i = exePath.size() - 1; i > 0; i--) {
        if (exePath[i] == '/' || exePath[i] == '\\') {
            exePath.erase(i + 1, MAXN);
            break;
        }
    }
    //从启动参数执行脚本
    if (argc >= 2) {
        std::fstream ftest(argv[1]);
        if (ftest) {
            std::string path("runfile ");
            path.append(argv[1]);
            if (squidlanglib::command.run(path) == EXIT_MAIN)
                return 0;
        }
    }
    else {
        system("title SquidLang Beta v0.3 - By MineCommander");
        squidlanglib::sendOutput("SquidLang Beta  v0b3\nCopyright MineCommander (C) 2020", false);
    }
    std::string inp_com;
    while(1)
    {
        std::cout << ">>>";
        std::getline(std::cin,inp_com);
        if (squidlanglib::command.run(inp_com) == EXIT_MAIN)
            return 0;
    }
}
