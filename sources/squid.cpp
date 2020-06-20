#include "heads.hpp"
string exePath;
//===添加命令处理函数开始===//

int _Settings_cmd(string subcmd) {   if (!squidlib.general.ifstate()) return IFSTATES_FALSE;
    stringstream cmdvcp(subcmd);
    string rc, sett, state;
    cmdvcp >> rc >> sett >> state;
    if (rc == "m" || rc == "modify") {
        if (sett == "sendLog") {
            if (state == "on" || state == "true")
                squidlib.settings.sendLog = true;
            else if (state == "off" || state == "false")
                squidlib.settings.sendLog = false;
            else {
                squidlib.io.sendError("Unknown state '" + state + "'");
                return 0;
            }
        }
        else if (sett == "sendWarn") {
            if (state == "on" || state == "true")
                squidlib.settings.sendWarn = true;
            else if (state == "off" || state == "false")
                squidlib.settings.sendWarn = false;
            else {
                squidlib.io.sendError("Unknown state '" + state + "'");
                return 0;
            }
        }
        else if (sett == "systemCommandScriptWarn") {
            if (state == "on" || state == "true")
                squidlib.settings.systemCommandScriptWarn = true;
            else if (state == "off" || state == "false")
                squidlib.settings.systemCommandScriptWarn = false;
            else {
                squidlib.io.sendError("Unknown state '" + state + "'");
                return 0;
            }
        }
        else {
            squidlib.io.sendError("Unknown setting option '" + sett + "'");
            return 0;
        }
        squidlib.io.sendLog("Setting option '" + sett + "' has been set to '" + state + "'");
        return 0;
    }
    else if (rc == "q" || rc == "query") {
        stringstream msgtemp;
        if (sett == "sendLog")
            msgtemp << "sendLog = " << squidlib.general.atob<bool, string>(squidlib.settings.sendLog);
        else if (sett == "sendWarn")
            msgtemp << "sendWarn = " << squidlib.general.atob<bool, string>(squidlib.settings.sendWarn);
        else if (sett == "systemCommandScriptWarn")
            msgtemp << "systemCommandScriptWarn = " << squidlib.general.atob<bool, string>(squidlib.settings.systemCommandScriptWarn);
        else if (sett == "all" || sett == "*")
            msgtemp << "sendLog = " << squidlib.general.atob<bool, string>(squidlib.settings.sendLog) << endl
                       << "sendWarn = " << squidlib.general.atob<bool, string>(squidlib.settings.sendWarn) << endl
                       << "systemCommandScriptWarn = " << squidlib.general.atob<bool, string>(squidlib.settings.systemCommandScriptWarn);
        else {
            squidlib.io.sendError("Unknown setting option '" + sett + "'");
            return 0;
        }
        squidlib.io.sendInfo(msgtemp.str());
    }
    else {
        squidlib.io.sendError("Unknown subcommand '" + rc + "'");
    }
}
int _System_sqcmd(string subcmd) {  if (!squidlib.general.ifstate()) return IFSTATES_FALSE;
    squidlib.io.sendLog("Run system command");
    system(subcmd.c_str());
    return 0;
}
int output(string subcmd) { if (!squidlib.general.ifstate()) return IFSTATES_FALSE;
    squidlib.io.sendOutput(subcmd, true);
    return 0;
}
int _Exit_sqcmd(string subcmd) {    if (!squidlib.general.ifstate()) return IFSTATES_FALSE;
    squidlib.io.sendOutput("Bye!\nPress any key to exit", false);
    getchar();
    return EXIT_MAIN;
}
int runfile(string subcmd) {    if (!squidlib.general.ifstate()) return IFSTATES_FALSE;
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
        squidlib.io.sendError("File '" + subcmd + "' does not exist");
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
            if (squidlib.general.ifstate())
                goto loopagain;
            else;
        else if (*i == "(end)" && squidlib.general.ifstate())   //文件关键字
            if (squidlib.general.ifstate())
                return 0;
            else;
        else if (squidlib.command.run(*i) == EXIT_MAIN)
            return EXIT_MAIN;
    }
    return 0;
}
int _Var_sqcmd(string subcmd) { if (!squidlib.general.ifstate()) return IFSTATES_FALSE;
    string temp_rc;
    string temp_cg1;
    string temp_cg2;
    string temp_cg3;
    stringstream trc(subcmd);
    trc >> temp_rc >> temp_cg1 >> temp_cg2 >> temp_cg3;
    if (temp_rc == "new" || temp_rc == "create" || temp_rc == "def" || temp_rc == "define") {
        if (squidlib.var_list.count(temp_cg1) == 1) {
            squidlib.io.sendWarn("The variable '" + temp_cg1 + "' was already exists");
        }
        else {
            squidlib.var_list.insert(make_pair(temp_cg1, 0));
            squidlib.io.sendLog("New variable '" + temp_cg1 + "' has been created");
        }
    }
    else if (temp_rc == "list") {
        map<string, double>::iterator it;
        stringstream msgtemp;
        msgtemp << "There are " << squidlib.var_list.size() << " variables exist:" << endl;
        for (it = squidlib.var_list.begin(); it != squidlib.var_list.end(); it++)
            msgtemp << it->first << " = " << it->second << endl;
        squidlib.io.sendInfo(msgtemp.str());
    }
    else if (temp_rc == "operation" || temp_rc == "ope") {
        if (squidlib.var_list.count(temp_cg1) == 0) {
            squidlib.io.sendWarn("The variable '" + temp_cg1 + "' does not exist");
        }
        else {
            double cg_temp = squidlib.general.atob<string, double>(temp_cg3);
            if (temp_cg2 == "+" || temp_cg2 == "add" || temp_cg2 == "plus") {
                squidlib.var_list[temp_cg1] += cg_temp;
                stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been added by " << cg_temp << " (now " << squidlib.var_list[temp_cg1] << ")";
                squidlib.io.sendLog(msgtemp.str());
            }
            else if (temp_cg2 == "-" || temp_cg2 == "remove" || temp_cg2 == "minus") {
                squidlib.var_list[temp_cg1] -= cg_temp;
                stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been removed by " << cg_temp << " (now " << squidlib.var_list[temp_cg1] << ")";
                squidlib.io.sendLog(msgtemp.str());
            }
            else if (temp_cg2 == "*" || temp_cg2 == "multiply") {
                squidlib.var_list[temp_cg1] *= cg_temp;
                stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been multiplied by " << cg_temp << " (now " << squidlib.var_list[temp_cg1] << ")";
                squidlib.io.sendLog(msgtemp.str());
            }
            else if (temp_cg2 == "/" || temp_cg2 == "divide") {
                if (cg_temp == 0) squidlib.io.sendWarn("Cannot be divided by 0");
                else {
                    squidlib.var_list[temp_cg1] /= cg_temp;
                    stringstream msgtemp;
                    msgtemp << "Variable '" << temp_cg1 << "' has been divided by " << cg_temp << " (now " << squidlib.var_list[temp_cg1] << ")";
                    squidlib.io.sendLog(msgtemp.str());
                }
            }
            else if (temp_cg2 == "=" || temp_cg2 == "set") {
                squidlib.var_list[temp_cg1] = cg_temp;
                stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been setted to " << squidlib.var_list[temp_cg1];
                squidlib.io.sendLog(msgtemp.str());
            }
            else if (temp_cg2 == "pow" || temp_cg2 == "power" || temp_cg2 == "^") {
                squidlib.var_list[temp_cg1] = pow(squidlib.var_list[temp_cg1], cg_temp);
                stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been setted to " << squidlib.var_list[temp_cg1];
                squidlib.io.sendLog(msgtemp.str());
            }
            else {
                squidlib.io.sendError("Unknown oprator '" + temp_cg2 + "'");
            }
        }
    }
    else if (temp_rc == "delete" || temp_rc == "del" || temp_rc == "undef" || temp_rc == "define") {
        if (squidlib.var_list.count(temp_cg1) == 1) {
            squidlib.var_list.erase(temp_cg1);
            squidlib.io.sendLog("The variable '" + temp_cg1 + "' has been deleted");
        }
        else {
            squidlib.io.sendWarn("The variable '" + temp_cg1 + "' dose not exist");
        }
    }
    else {
        stringstream msgtemp;
        msgtemp << "Unknown subcommand '" << temp_rc << "'";
        squidlib.io.sendError(msgtemp.str());
    }
    return 0;
}
int _If_sqcmd(string subcmd) {  if (!squidlib.general.ifstate()) return IFSTATES_FALSE;
    stringstream comps(subcmd);
    comps >> squidlib.if_status.x1 >> squidlib.if_status.oprt >> squidlib.if_status.x2;
    squidlib.if_status.enable = true;
    squidlib.io.sendLog("Conditional judgment has been enabled");
    return 0;
}
int _Endif_sqcmd(string subcmd) {
    if (!subcmd.empty()) return 1;
    squidlib.if_status.enable = false;
    squidlib.io.sendLog("Conditional judgment has been disabled");
    return 0;
}
int _Waitfor_sqcmd(string subcmd) { if (!squidlib.general.ifstate()) return IFSTATES_FALSE;
    int tm = squidlib.general.atob<string, int>(subcmd);
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
    if (!squidlib.general.if_state()) return IFSTATES_FALSE;
    这样当if命令创建的条件判断不通过时处理函数将不会执行并返回。
*/

//===添加命令处理函数结束===//

void regist_command(void) { //注册命令
    squidlib.cmd_register.clear();

    squidlib.regcmd("settings", _Settings_cmd);
    squidlib.regcmd("system", _System_sqcmd);
    squidlib.regcmd("output",output);
    squidlib.regcmd("print", output);
    squidlib.regcmd("echo", output);
    squidlib.regcmd("exit", _Exit_sqcmd);
    squidlib.regcmd("runfile", runfile);
    squidlib.regcmd("script", runfile);
    squidlib.regcmd("scr", runfile);
    squidlib.regcmd("var", _Var_sqcmd);
    squidlib.regcmd("variable", _Var_sqcmd);
    squidlib.regcmd("if", _If_sqcmd);
    squidlib.regcmd("(endif)", _Endif_sqcmd);
    squidlib.regcmd("wait", _Waitfor_sqcmd);
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
    system("title Squid Beta v0.2 - By MineCommander");
    squidlib.io.sendOutput("Squid Beta  v0b2\nCopyright MineCommander (C) 2020", false);
    regist_command();
    string inp_com;
    while(1)
    {
        cout << ">>>";
        getline(cin,inp_com);
        if (squidlib.command.run(inp_com) == EXIT_MAIN)
            return 0;
    }
}
