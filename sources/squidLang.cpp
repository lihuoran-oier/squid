#include "heads.hpp"
string exePath;
//===添加命令处理函数开始===//

int _Settings_cmd(string subcmd) {
    if (!squidlanglib.if_status.state()) return IFSTATES_FALSE;
    stringstream cmdvcp(subcmd);
    string rc, sett, state;
    cmdvcp >> rc >> sett >> state;
    if (rc == "m" || rc == "modify") {
        if (sett == "sendLog") {
            if (state == "on" || state == "true")
                squidlanglib.io_and_settings.settings.sendLog = true;
            else if (state == "off" || state == "false")
                squidlanglib.io_and_settings.settings.sendLog = false;
            else {
                squidlanglib.io_and_settings.sendError("Unknown state '" + state + "'");
                return 0;
            }
        }
        else if (sett == "sendWarn") {
            if (state == "on" || state == "true")
                squidlanglib.io_and_settings.settings.sendWarn = true;
            else if (state == "off" || state == "false")
                squidlanglib.io_and_settings.settings.sendWarn = false;
            else {
                squidlanglib.io_and_settings.sendError("Unknown state '" + state + "'");
                return 0;
            }
        }
        else if (sett == "systemCommandScriptWarn") {
            if (state == "on" || state == "true")
                squidlanglib.io_and_settings.settings.systemCommandScriptWarn = true;
            else if (state == "off" || state == "false")
                squidlanglib.io_and_settings.settings.systemCommandScriptWarn = false;
            else {
                squidlanglib.io_and_settings.sendError("Unknown state '" + state + "'");
                return 0;
            }
        }
        else {
            squidlanglib.io_and_settings.sendError("Unknown setting option '" + sett + "'");
            return 0;
        }
        squidlanglib.io_and_settings.sendLog("Setting option '" + sett + "' has been set to '" + state + "'");
        return 0;
    }
    else if (rc == "q" || rc == "query") {
        stringstream msgtemp;
        if (sett == "sendLog")
            msgtemp << "sendLog = " << squidlanglib.general.atob<bool, string>(squidlanglib.io_and_settings.settings.sendLog);
        else if (sett == "sendWarn")
            msgtemp << "sendWarn = " << squidlanglib.general.atob<bool, string>(squidlanglib.io_and_settings.settings.sendWarn);
        else if (sett == "systemCommandScriptWarn")
            msgtemp << "systemCommandScriptWarn = " << squidlanglib.general.atob<bool, string>(squidlanglib.io_and_settings.settings.systemCommandScriptWarn);
        else if (sett == "all" || sett == "*")
            msgtemp << "sendLog = " << squidlanglib.general.atob<bool, string>(squidlanglib.io_and_settings.settings.sendLog) << endl
            << "sendWarn = " << squidlanglib.general.atob<bool, string>(squidlanglib.io_and_settings.settings.sendWarn) << endl
            << "systemCommandScriptWarn = " << squidlanglib.general.atob<bool, string>(squidlanglib.io_and_settings.settings.systemCommandScriptWarn);
        else {
            squidlanglib.io_and_settings.sendError("Unknown setting option '" + sett + "'");
            return 0;
        }
        squidlanglib.io_and_settings.sendInfo(msgtemp.str());
    }
    else {
        squidlanglib.io_and_settings.sendError("Unknown subcommand '" + rc + "'");
    }
}
int _System_sqcmd(string subcmd) {
    if (!squidlanglib.if_status.state()) return IFSTATES_FALSE;
    squidlanglib.io_and_settings.sendLog("Run system command");
    system(subcmd.c_str());
    return 0;
}
int output(string subcmd) {
    if (!squidlanglib.if_status.state()) return IFSTATES_FALSE;
    squidlanglib.io_and_settings.sendOutput(subcmd, true);
    return 0;
}
int _Exit_sqcmd(string subcmd) {
    if (!squidlanglib.if_status.state()) return IFSTATES_FALSE;
    squidlanglib.io_and_settings.sendOutput("Bye!\nPress any key to exit", false);
    getchar();
    return EXIT_MAIN;
}
int runfile(string subcmd) {
    if (!squidlanglib.if_status.state()) return IFSTATES_FALSE;
    string temp;
    vector<string> cmdlines;
    ifstream rf(subcmd.c_str());
    if (rf) {
        while (!rf.eof()) {
            getline(rf, temp);
            cmdlines.push_back(temp);
        }
    }
    else {
        squidlanglib.io_and_settings.sendError("File '" + subcmd + "' does not exist");
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
    for (vector<string>::iterator i = cmdlines.begin(); i != cmdlines.end(); i++) {
        if (i->empty() || i->at(0) == '#')   //空行和'#'开头的注释不执行
            continue;
        else if (*i == "loop")
            if (squidlanglib.if_status.state())
                goto loopagain;
            else;
        else if (*i == "(end)" && squidlanglib.if_status.state())   //文件关键字
            if (squidlanglib.if_status.state())
                return 0;
            else;
        else if (squidlanglib.command.run(*i) == EXIT_MAIN)
            return EXIT_MAIN;
    }
    return 0;
}
int _Var_sqcmd(string subcmd) {
    if (!squidlanglib.if_status.state()) return IFSTATES_FALSE;
    string temp_rc;
    string temp_cg1;
    string temp_cg2;
    string temp_cg3;
    stringstream trc(subcmd);
    trc >> temp_rc >> temp_cg1 >> temp_cg2 >> temp_cg3;
    if (temp_rc == "new" || temp_rc == "create" || temp_rc == "def" || temp_rc == "define") {
        if (var_list.count(temp_cg1) == 1) {
            squidlanglib.io_and_settings.sendWarn("The variable '" + temp_cg1 + "' was already exists");
        }
        else {
            var_list.insert(make_pair(temp_cg1, 0));
            squidlanglib.io_and_settings.sendLog("New variable '" + temp_cg1 + "' has been created");
        }
    }
    else if (temp_rc == "list") {
        map<string, double>::iterator it;
        stringstream msgtemp;
        msgtemp << "There are " << var_list.size() << " variables exist:" << endl;
        for (it = var_list.begin(); it != var_list.end(); it++)
            msgtemp << it->first << " = " << it->second << endl;
        squidlanglib.io_and_settings.sendInfo(msgtemp.str());
    }
    else if (temp_rc == "operation" || temp_rc == "ope") {
        if (var_list.count(temp_cg1) == 0) {
            squidlanglib.io_and_settings.sendWarn("The variable '" + temp_cg1 + "' does not exist");
        }
        else {
            double cg_temp = squidlanglib.general.atob<string, double>(temp_cg3);
            if (temp_cg2 == "+" || temp_cg2 == "add" || temp_cg2 == "plus") {
                var_list[temp_cg1] += cg_temp;
                stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been added by " << cg_temp << " (now " << var_list[temp_cg1] << ")";
                squidlanglib.io_and_settings.sendLog(msgtemp.str());
            }
            else if (temp_cg2 == "-" || temp_cg2 == "remove" || temp_cg2 == "minus") {
                var_list[temp_cg1] -= cg_temp;
                stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been removed by " << cg_temp << " (now " << var_list[temp_cg1] << ")";
                squidlanglib.io_and_settings.sendLog(msgtemp.str());
            }
            else if (temp_cg2 == "*" || temp_cg2 == "multiply") {
                var_list[temp_cg1] *= cg_temp;
                stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been multiplied by " << cg_temp << " (now " << var_list[temp_cg1] << ")";
                squidlanglib.io_and_settings.sendLog(msgtemp.str());
            }
            else if (temp_cg2 == "/" || temp_cg2 == "divide") {
                if (cg_temp == 0) squidlanglib.io_and_settings.sendWarn("Cannot be divided by 0");
                else {
                    var_list[temp_cg1] /= cg_temp;
                    stringstream msgtemp;
                    msgtemp << "Variable '" << temp_cg1 << "' has been divided by " << cg_temp << " (now " << var_list[temp_cg1] << ")";
                    squidlanglib.io_and_settings.sendLog(msgtemp.str());
                }
            }
            else if (temp_cg2 == "=" || temp_cg2 == "set") {
                var_list[temp_cg1] = cg_temp;
                stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been setted to " << var_list[temp_cg1];
                squidlanglib.io_and_settings.sendLog(msgtemp.str());
            }
            else if (temp_cg2 == "pow" || temp_cg2 == "power" || temp_cg2 == "^") {
                var_list[temp_cg1] = pow(var_list[temp_cg1], cg_temp);
                stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been setted to " << var_list[temp_cg1];
                squidlanglib.io_and_settings.sendLog(msgtemp.str());
            }
            else {
                squidlanglib.io_and_settings.sendError("Unknown oprator '" + temp_cg2 + "'");
            }
        }
    }
    else if (temp_rc == "delete" || temp_rc == "del" || temp_rc == "undef" || temp_rc == "define") {
        if (var_list.count(temp_cg1) == 1) {
            var_list.erase(temp_cg1);
            squidlanglib.io_and_settings.sendLog("The variable '" + temp_cg1 + "' has been deleted");
        }
        else {
            squidlanglib.io_and_settings.sendWarn("The variable '" + temp_cg1 + "' dose not exist");
        }
    }
    else {
        stringstream msgtemp;
        msgtemp << "Unknown subcommand '" << temp_rc << "'";
        squidlanglib.io_and_settings.sendError(msgtemp.str());
    }
    return 0;
}
int _If_sqcmd(string subcmd) {
    if (!squidlanglib.if_status.state()) return IFSTATES_FALSE;
    stringstream comps(subcmd);
    comps >> squidlanglib.if_status.x1 >> squidlanglib.if_status.oprt >> squidlanglib.if_status.x2;
    squidlanglib.if_status.enable = true;
    squidlanglib.io_and_settings.sendLog("Conditional judgment has been enabled");
    return 0;
}
int _Endif_sqcmd(string subcmd) {
    if (!subcmd.empty()) return 1;
    squidlanglib.if_status.enable = false;
    squidlanglib.io_and_settings.sendLog("Conditional judgment has been disabled");
    return 0;
}
int _Waitfor_sqcmd(string subcmd) {
    if (!squidlanglib.if_status.state()) return IFSTATES_FALSE;
    int tm = squidlanglib.general.atob<string, int>(subcmd);
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
    if (!squidlib.if_status.state()) return IFSTATES_FALSE;
    这样当if命令创建的条件判断不通过时处理函数将不会执行并返回。
*/

//===添加命令处理函数结束===//

void regist_command(void) { //注册命令
    cmd_register.clear();

    squidlanglib.regcmd("settings", _Settings_cmd);
    squidlanglib.regcmd("system", _System_sqcmd);
    squidlanglib.regcmd("output", output);
    squidlanglib.regcmd("print", output);
    squidlanglib.regcmd("echo", output);
    squidlanglib.regcmd("exit", _Exit_sqcmd);
    squidlanglib.regcmd("runfile", runfile);
    squidlanglib.regcmd("script", runfile);
    squidlanglib.regcmd("scr", runfile);
    squidlanglib.regcmd("var", _Var_sqcmd);
    squidlanglib.regcmd("variable", _Var_sqcmd);
    squidlanglib.regcmd("if", _If_sqcmd);
    squidlanglib.regcmd("(endif)", _Endif_sqcmd);
    squidlanglib.regcmd("wait", _Waitfor_sqcmd);
    /*
        请使用这个格式来注册命令：
        squidlib.regcmd("根命令字符串", 函数名指针);
        若执行的根命令与字符串匹配，将会调用函数名指针指向的函数。
    */
}



int main(int argc, char *argv[])
{
    /*
    string path("runfile ");
    path.append(argv[1]);
    if(argc>=2)
        if(run_command(path))
            return 0;
    */  //:thonk:
    exePath = argv[0];
    for (int i = exePath.size() - 1; i > 0; i--) {
        if (exePath[i] == '/' || exePath[i] == '\\') {
            exePath.erase(i + 1, MAXN);
            break;
        }
    }
    system("title SquidLang Beta v0.2 - By MineCommander");
    squidlanglib.io_and_settings.sendOutput("SquidLang Beta  v0b2\nCopyright MineCommander (C) 2020", false);
    regist_command();
    string inp_com;
    while(1)
    {
        cout << ">>>";
        getline(cin,inp_com);
        if (squidlanglib.command.run(inp_com) == EXIT_MAIN)
            return 0;
    }
}
