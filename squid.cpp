#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<cmath>
#include<vector>
#include<ctime>
#include<map>
using namespace std;

const int EXIT_MAIN = 65536;
const int IFSTATES_FALSE = 100001;
const int MAXN = 2147483647;
struct _ifst_str {
    float x1 = 0, x2 = 0;
    string oprt;
    bool enable = false;
}    if_states;
typedef int(*Fp)(string subcmd);
map<string, double> var_list;
map<string, Fp> cmd_register;
struct tStn {
    bool sendLog = true;
    bool sendWarn = true;
    bool systemCommandScriptWarn = true;
}    setting_status;

string getSysTimeData(void) {
    time_t t = time(0);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y%m%d-%H%M%S", localtime(&t));
    return tmp;
}

string sysTimeData(getSysTimeData());
ofstream logFile(("logs/" + sysTimeData + ".log").c_str());

string sysTime(void) {
    time_t t = time(0);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%H:%M:%S", localtime(&t));
    return tmp;
}

void sendLog(string msg) {
    if (setting_status.sendLog) cout << "[LOG] " << msg << endl;
    logFile << "[" << sysTime() << "][LOG] " << msg << endl;
}

void sendWarn(string msg) {
    if (setting_status.sendWarn) cout << "[WARN] " << msg << endl;
    logFile << "[" << sysTime() << "][WARN] " << msg << endl;
}

void sendError(string msg) {
    cout << "[ERROR] " << msg << endl;
    logFile << "[" << sysTime() << "][ERROR] " << msg << endl;
}

void sendInfo(string msg) {
    cout << "[INFO] " << msg << endl;
    logFile << "[" << sysTime() << "][INFO] " << msg << endl;
}

void sendOutput(string msg, bool log) {
    cout << msg << endl;
    if (log) logFile << "[" << sysTime() << "][OUTPUT] " << msg << endl;
}



string subcommand(string command)
{
    int state = 0;
    for (int i = 0; i < command.size(); i++) {
        if (command[i] != ' ' && state == 0)
            state = 1;
        else if (command[i] == ' ' && state == 1)
            state = 2;
        else if (command[i] != ' ' && state == 2)
            return command.substr(i, command.size());
    }
    return "";
}

template <class Ta,class Tb>
Tb atob(const Ta &t) {
    stringstream temp;
    temp << t;
    Tb i;
    temp >> i;
    return i;
}

void _regcmd(string cmdstr,Fp cmdfp) {
    cmd_register.insert(make_pair(cmdstr, cmdfp));
}

string compile_var(string cmd)
{
    string varname;
    int state = 0;
    char type;
    int ns = 0, ne = cmd.size() - 1;
    for (int i = 0; i < cmd.size(); i++) {
        if ((cmd[i] == '$' || cmd[i] == '@') && cmd[i + 1] == '<') {
            type = cmd[i];
            varname.clear();
            
            state = 1;
            ns = i;
            i += 2;
        }
        else if (state == 1 && cmd[i] == '>') {
            state = 2;
            ne = i;
            break;
        }
        varname.push_back(cmd[i]);
    }
    if (state == 2) {
        int ln = ne - ns + 1;
        if (type == '$')
            cmd.replace(ns, ln, atob<double, string>(var_list[varname]));
        else if (type == '@') {
            if (varname == "endl")
                cmd.replace(ns, ln, "\n");
            else if (varname == "sysTimeStamp")
                cmd.replace(ns, ln, atob<int, string>(time(0)));
            else if (varname == "sysTime")
                cmd.replace(ns, ln, sysTime());
            else
                cmd.erase(ns, ln);
        }
        cmd = compile_var(cmd);
    }
    return cmd;
}

bool ifstate(void) {
    int oprt = 0;
    if (if_states.oprt == ">" || if_states.oprt == "is_bigger_than")
        oprt = 1;
    else if (if_states.oprt == ">=" || if_states.oprt == "isnot_less_than")
        oprt = 2;
    else if (if_states.oprt == "<" || if_states.oprt == "is_less_than")
        oprt = 3;
    else if (if_states.oprt == "<=" || if_states.oprt == "isnot_bigger_than")
        oprt = 4;
    else if (if_states.oprt == "=" || if_states.oprt == "==" || if_states.oprt == "is")
        oprt = 5;
    else if (if_states.oprt == "!=" || if_states.oprt == "isnot")
        oprt = 6;
    if (if_states.enable == true)
        return (if_states.x1 > if_states.x2 && oprt == 1) || (if_states.x1 >= if_states.x2 && oprt == 2) || (if_states.x1 < if_states.x2&& oprt == 3) || (if_states.x1 <= if_states.x2 && oprt == 4) || (if_states.x1 == if_states.x2 && oprt == 5) || (if_states.x1 != if_states.x2 && oprt == 6);
    else
        return true;
}
int run_command(string command) {
    string subcmd;
    int state = 0;
    for (int i = 0; i < command.size(); i++) {
        if (state == 0 && command[i] != ' ')
            state = 1;
        if (state == 1 && command[i] == ' ')
            state = 2;
        if (state == 2 && command[i] != ' ') {
            subcmd = command.substr(i, MAXN);
            break;
        }
    }
    stringstream rtcmdpe(command);
    string rootcmd;
    rtcmdpe >> rootcmd;
    if (cmd_register.count(rootcmd) == 1)
        return cmd_register[rootcmd](subcmd);
    else {
        stringstream msgtemp;
        msgtemp << "Unknown command'" << rootcmd << "'";
        sendError(msgtemp.str());
    }

}
//===添加命令处理函数开始===//

int settings(string subcmd) {
    if (!ifstate()) return IFSTATES_FALSE;

    stringstream cmdvcp(subcmd);
    string rc, sett, state;
    cmdvcp >> rc >> sett >> state;
    if (rc == "m" || rc == "modify") {
        if (sett == "sendLog") {
            if (state == "on" || state == "true")
                setting_status.sendLog = true;
            else if (state == "off" || state == "false")
                setting_status.sendLog = false;
            else {
                sendError("Unknown state '" + state + "'");
                return 0;
            }
        }
        else if (sett == "sendWarn") {
            if (state == "on" || state == "true")
                setting_status.sendWarn = true;
            else if (state == "off" || state == "false")
                setting_status.sendWarn = false;
            else {
                sendError("Unknown state '" + state + "'");
                return 0;
            }
        }
        else if (sett == "systemCommandScriptWarn") {
            if (state == "on" || state == "true")
                setting_status.systemCommandScriptWarn = true;
            else if (state == "off" || state == "false")
                setting_status.systemCommandScriptWarn = false;
            else {
                sendError("Unknown state '" + state + "'");
                return 0;
            }
        }
        else {
            sendError("Unknown setting option '" + sett + "'");
            return 0;
        }
        sendLog("Setting option '" + sett + "' has been set to '" + state + "'");
        return 0;
    }
    else if (rc == "q" || rc == "query") {
        stringstream msgtemp;
        if (sett == "sendLog")
            msgtemp << "sendLog = " << (setting_status.sendLog ? "true" : "false");
        else if (sett == "sendWarn")
            msgtemp << "sendWarn = " << (setting_status.sendWarn ? "true" : "false");
        else if (sett == "systemCommandScriptWarn")
            msgtemp << "systemCommandScriptWarn = " << (setting_status.systemCommandScriptWarn ? "true" : "false");
        else if (sett == "all" || sett == "*")
            msgtemp << "sendLog = " << (setting_status.sendLog ? "true" : "false") << endl
                       << "sendWarn = " << (setting_status.sendWarn ? "true" : "false") << endl
                       << "systemCommandScriptWarn = " << (setting_status.systemCommandScriptWarn ? "true" : "false");
        else {
            sendError("Unknown setting option '" + sett + "'");
            return 0;
        }
        sendInfo(msgtemp.str());
    }
    else {
        sendError("Unknown subcommand '" + rc + "'");
    }
}
int _System_sqcmd(string subcmd) {
    if (!ifstate()) return IFSTATES_FALSE;

    sendLog("Run system command '" + subcmd + "'");
    system(subcmd.c_str());
    return 0;
}
int output(string subcmd) {
    if (!ifstate()) return IFSTATES_FALSE;

    sendOutput(subcmd, true);
    return 0;
}
int _Exit_sqcmd(string subcmd) {
    if (!ifstate()) return IFSTATES_FALSE;

    sendOutput("Bye!\nPress any key to exit", false);
    getchar();
    return EXIT_MAIN;
}
int runfile(string subcmd) {
    if (!ifstate()) return IFSTATES_FALSE;

    string temp;
    ifstream rf(subcmd.c_str());
    if (rf) {
        while (!rf.eof()) {
            getline(rf, temp);
            if (run_command(compile_var(temp)) == EXIT_MAIN)
                return EXIT_MAIN;
        }
    }
    else {
        stringstream msgtemp;
        msgtemp << "File '" << subcmd << "' does not exist" << endl;
        sendError(msgtemp.str());
        return 0;
    }
    rf.close();
}
int _Var_sqcmd(string subcmd) {
    if (!ifstate()) return IFSTATES_FALSE;

    string temp_rc;
    string temp_cg1;
    string temp_cg2;
    string temp_cg3;
    stringstream trc(subcmd);
    trc >> temp_rc >> temp_cg1 >> temp_cg2 >> temp_cg3;
    if (temp_rc == "new" || temp_rc == "create" || temp_rc == "def" || temp_rc == "define") {
        if (var_list.count(temp_cg1) == 1) {
            sendWarn("The variable '" + temp_cg1 + "' was already exists");
        }
        else {
            var_list.insert(make_pair(temp_cg1, 0));
            sendLog("New variable '" + temp_cg1 + "' has been created");
        }
    }
    else if (temp_rc == "list") {
        map<string, double>::iterator it;
        stringstream msgtemp;
        msgtemp << "There are " << var_list.size() << " variables exist:" << endl;
        for (it = var_list.begin(); it != var_list.end(); it++)
            msgtemp << it->first << " = " << it->second << endl;
        sendInfo(msgtemp.str());
    }
    else if (temp_rc == "operation" || temp_rc == "ope") {
        if (var_list.count(temp_cg1) == 0) {
            sendWarn("The variable '" + temp_cg1 + "' does not exist");
        }
        else {
            double cg_temp = atob<string, double>(temp_cg3);
            if (temp_cg2 == "+" || temp_cg2 == "add" || temp_cg2 == "plus") {
                var_list[temp_cg1] += cg_temp;
                stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been added by " << cg_temp << " (now " << var_list[temp_cg1] << ")";
                sendLog(msgtemp.str());
            }
            else if (temp_cg2 == "-" || temp_cg2 == "remove" || temp_cg2 == "minus") {
                var_list[temp_cg1] -= cg_temp;
                stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been removed by " << cg_temp << " (now " << var_list[temp_cg1] << ")";
                sendLog(msgtemp.str());
            }
            else if (temp_cg2 == "*" || temp_cg2 == "multiply") {
                var_list[temp_cg1] *= cg_temp;
                stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been multiplied by " << cg_temp << " (now " << var_list[temp_cg1] << ")";
                sendLog(msgtemp.str());
            }
            else if (temp_cg2 == "/" || temp_cg2 == "divide") {
                if (cg_temp == 0) sendWarn("Cannot be divided by 0");
                else {
                    var_list[temp_cg1] /= cg_temp;
                    stringstream msgtemp;
                    msgtemp << "Variable '" << temp_cg1 << "' has been divided by " << cg_temp << " (now " << var_list[temp_cg1] << ")";
                    sendLog(msgtemp.str());
                }
            }
            else if (temp_cg2 == "=" || temp_cg2 == "set") {
                var_list[temp_cg1] = cg_temp;
                stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been setted to " << var_list[temp_cg1];
                sendLog(msgtemp.str());
            }
            else if (temp_cg2 == "pow" || temp_cg2 == "power" || temp_cg2 == "^") {
                var_list[temp_cg1] = pow(var_list[temp_cg1], cg_temp);
                stringstream msgtemp;
                msgtemp << "Variable '" << temp_cg1 << "' has been setted to " << var_list[temp_cg1];
                sendLog(msgtemp.str());
            }
            else {
                stringstream msgtemp;
                msgtemp << "Unknown oprator '" << temp_cg2 << "'";
                sendError(msgtemp.str());
            }
        }
    }
    else if (temp_rc == "delete" || temp_rc == "del" || temp_rc == "undef" || temp_rc == "define") {
        if (var_list.count(temp_cg1) == 1) {
            var_list.erase(temp_cg1);
            sendLog("The variable '" + temp_cg1 + "' has been deleted");
        }
        else {
            sendWarn("The variable '" + temp_cg1 + "' dose not exist");
        }
    }
    else {
        stringstream msgtemp;
        msgtemp << "Unknown subcommand '" << temp_rc << "'";
        sendError(msgtemp.str());
    }
    return 0;
}
int _If_sqcmd(string subcmd) {
    if (!ifstate()) return IFSTATES_FALSE;

    stringstream comps(subcmd);
    comps >> if_states.x1 >> if_states.oprt >> if_states.x2;
    if_states.enable = true;
    sendLog("Conditional judgment has been enabled");
    return 0;
}
int _Endif_sqcmd(string subcmd) {
    if (!subcmd.empty()) return 1;
    if_states.enable = false;
    sendLog("Conditional judgment has been disabled");
    return 0;
}
int _Waitfor_sqcmd(string subcmd) {
    int tm = atob<string, int>(subcmd);
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
    if (!ifstate()) return IFSTATES_FALSE;
    这样当if命令创建的条件判断不通过时处理函数将不会执行并返回。
*/

//===添加命令处理函数结束===//

void regist_command(void) { //注册命令
    cmd_register.clear();

    _regcmd("settings", settings);
    _regcmd("system", _System_sqcmd);
    _regcmd("output",output);
    _regcmd("exit", _Exit_sqcmd);
    _regcmd("runfile", runfile);
    _regcmd("script", runfile);
    _regcmd("scr", runfile);
    _regcmd("var", _Var_sqcmd);
    _regcmd("variable", _Var_sqcmd);
    _regcmd("if", _If_sqcmd);
    _regcmd("(endif)", _Endif_sqcmd);
    _regcmd("wait", _Waitfor_sqcmd);
    /*
        请使用这个格式来注册命令：
        _regcmd("根命令字符串", 函数名指针);
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
    system("title Squid Beta v0.2 - By MineCommander");
    sendOutput("Squid Beta  v0b2\nCopyright MineCommander (C) 2020", false);
    regist_command();
    string inp_com;
    while(1)
    {
        cout << ">>>";
        getline(cin,inp_com);
        inp_com = compile_var(inp_com);
        if (run_command(inp_com) == EXIT_MAIN)
            return 0;
    }
}
