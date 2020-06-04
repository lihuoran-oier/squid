#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<cmath>
#include<vector>
#include<map>
using namespace std;

const int EXIT_MAIN = 65536;
const int IFSTATUS_FALSE = 100001;
const int MAXN = 2147483647;
bool _boardcast = true;
struct _ifst_str {
    float x1 = 0, x2 = 0;
    string oprt;
    bool enable = false;
};
_ifst_str if_status;
typedef int(*Fp)(string subcmd);
map<string, double> var_list;
map<string, Fp> cmd_register;

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

double str2dbl(string str) {
    if (str.size() == 0) return 0;
    else {
        stringstream strm(str);
        double temp;
        strm >> temp;
        return temp;
    }
}

string dbl2str(double flt) {
    stringstream strm;
    strm << flt;
    string temp(strm.str());
    return temp;
}

void _regcmd(string cmdstr,Fp cmdfp) {
    cmd_register.insert(make_pair(cmdstr, cmdfp));
}

string compile_var(string cmd)
{
    string varname;
    int state = 0;
    int ns = 0, ne = cmd.size() - 1;
    for (int i = 0; i < cmd.size(); i++) {
        if (cmd[i] == '$' && cmd[i + 1] == '<') {
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
        cmd.replace(ns, ln, dbl2str(var_list[varname]));
        cmd = compile_var(cmd);
    }
    return cmd;
}

bool ifstatu(void) {
    int oprt = 0;
    if (if_status.oprt == ">" || if_status.oprt == "is_bigger_than")
        oprt = 1;
    else if (if_status.oprt == ">=" || if_status.oprt == "isnot_less_than")
        oprt = 2;
    else if (if_status.oprt == "<" || if_status.oprt == "is_less_than")
        oprt = 3;
    else if (if_status.oprt == "<=" || if_status.oprt == "isnot_bigger_than")
        oprt = 4;
    else if (if_status.oprt == "=" || if_status.oprt == "==" || if_status.oprt == "is")
        oprt = 5;
    else if (if_status.oprt == "!=" || if_status.oprt == "isnot")
        oprt = 6;
    if (if_status.enable == true)
        return (if_status.x1 > if_status.x2 && oprt == 1) || (if_status.x1 >= if_status.x2 && oprt == 2) || (if_status.x1 < if_status.x2&& oprt == 3) || (if_status.x1 <= if_status.x2 && oprt == 4) || (if_status.x1 == if_status.x2 && oprt == 5) || (if_status.x1 != if_status.x2 && oprt == 6);
    else
        return true;
}
int run_command(string command) {
    string subcmd;
    if (ifstatu()) {
        for (int i = 0; i < command.size(); i++)
            if (command[i] == '\\' && command[i + 1] == 'n')
                command.replace(i, 2, "\n");
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
        else
            cout << "[ERROR] Unknown command'" << rootcmd << "'" << endl;
    }
    else
        if(command=="(endif)")
            return cmd_register[command](subcmd);
        else
            return IFSTATUS_FALSE;
}
//===添加命令处理函数开始===//

int boardcast(string subcmd) {
    if (subcmd == "on" || subcmd == "true") {
        _boardcast = true;
        if (_boardcast) cout << "Boardcast command result now on" << endl;
    }
    else if (subcmd == "off" || subcmd == "false") {
        _boardcast = false;
        if (_boardcast) cout << "这条语句永远不会被执行2333" << endl;
    }
    else {
        cout << "[ERROR] Unknown statu '" << subcmd << "'" << endl;
    }
    return 0;
}
int _System_sqcmd(string subcmd) {
    if (_boardcast) cout << "Run system command" << endl;
    system(subcmd.c_str());
    return 0;
}
int output(string subcmd) {
    cout << subcmd << endl;
    return 0;
}
int _Exit_sqcmd(string subcmd) {
    cout << "Bye!\nPress any key to exit" << endl;
    getchar();
    return EXIT_MAIN;
}
int runfile(string subcmd) {
    string temp;
    ifstream rf(subcmd.c_str(), ios::_Nocreate);
    if (rf) {
        while (!rf.eof()) {
            getline(rf, temp);
            return (run_command(temp));
        }
    }
    else {
        cout << "[ERROR] File '" << subcmd << "' does not exist" << endl;
        return 0;
    }
    rf.close();
}
int _Var_sqcmd(string subcmd) {
    string temp_rc;
    string temp_cg1;
    string temp_cg2;
    string temp_cg3;
    stringstream trc(subcmd);
    trc >> temp_rc >> temp_cg1 >> temp_cg2 >> temp_cg3;
    if (temp_rc == "new") {
        if (var_list.count(temp_cg1) == 1) {
            if (_boardcast) cout << "The variable '" << temp_cg1 << "' was already exists" << endl;
        }
        else {
            var_list.insert(make_pair(temp_cg1, 0));
            if (_boardcast) cout << "New variable '" << temp_cg1 << "' has been created" << endl;
        }
    }
    else if (temp_rc == "list") {
        map<string, double>::iterator it;
        cout << "[INFO] There are " << var_list.size() << " variables exist:" << endl;
        for (it = var_list.begin(); it != var_list.end(); it++)
            cout << it->first << " = " << it->second << endl;
    }
    else if (temp_rc == "operation" || temp_rc == "ope") {
        if (var_list.count(temp_cg1) == 0) {
            if (_boardcast) cout << "The variable '" << temp_cg1 << "' does not exist" << endl;
        }
        else {
            double cg_temp = str2dbl(temp_cg3);
            if (temp_cg2 == "+" || temp_cg2 == "add" || temp_cg2 == "plus") {
                var_list[temp_cg1] += cg_temp;
                if (_boardcast) cout << "Variable '" << temp_cg1 << "' has been added by " << cg_temp << " (now " << var_list[temp_cg1] << ")" << endl;
            }
            else if (temp_cg2 == "-" || temp_cg2 == "remove" || temp_cg2 == "minus") {
                var_list[temp_cg1] -= cg_temp;
                if (_boardcast) cout << "Variable '" << temp_cg1 << "' has been removed by " << cg_temp << " (now " << var_list[temp_cg1] << ")" << endl;
            }
            else if (temp_cg2 == "*" || temp_cg2 == "multiply") {
                var_list[temp_cg1] *= cg_temp;
                if (_boardcast) cout << "Variable '" << temp_cg1 << "' has been multiplied by " << cg_temp << " (now " << var_list[temp_cg1] << ")" << endl;
            }
            else if (temp_cg2 == "/" || temp_cg2 == "divide") {
                if (cg_temp == 0) if (_boardcast) cout << "Cannot be divided by 0" << endl; else;
                else {
                    var_list[temp_cg1] /= cg_temp;
                    if (_boardcast) cout << "Variable '" << temp_cg1 << "' has been divided by " << cg_temp << " (now " << var_list[temp_cg1] << ")" << endl;
                }
            }
            else if (temp_cg2 == "=" || temp_cg2 == "set") {
                var_list[temp_cg1] = cg_temp;
                if (_boardcast) cout << "Variable '" << temp_cg1 << "' has been setted to " << var_list[temp_cg1] << endl;
            }
            else if (temp_cg2 == "pow" || temp_cg2 == "power" || temp_cg2 == "^") {
                var_list[temp_cg1] = pow(var_list[temp_cg1], cg_temp);
                if (_boardcast) cout << "Variable '" << temp_cg1 << "' has been setted to " << var_list[temp_cg1] << endl;
            }
            else {
                cout << "[ERROR] Unknown oprator '" << temp_cg2 << "'" << endl;
            }
        }
    }
    else {
        cout << "[ERROR] Unknown subcommand '" << temp_rc << "'" << endl;
    }
    return 0;
}
int _If_sqcmd(string subcmd) {
    stringstream comps(subcmd);
    comps >> if_status.x1 >> if_status.oprt >> if_status.x2;
    if_status.enable = true;
    if (_boardcast) cout << "Conditional judgment has been enabled" << endl;
    return 0;
}
int _Endif_sqcmd(string subcmd) {
    if_status.enable = false;
    if (_boardcast) cout << "Conditional judgment has been disabled" << endl;
    return 0;
}

/*
    请使用这个格式来添加命令处理函数：
    int 函数名(string subcmd){...}
    subcmd是子命令字符串
*/

//===添加命令处理函数结束===//

void regist_command(void) { //注册命令
    cmd_register.clear();

    _regcmd("boardcast", boardcast);
    _regcmd("system", _System_sqcmd);
    _regcmd("output",output);
    _regcmd("exit", _Exit_sqcmd);
    _regcmd("runfile", runfile);
    _regcmd("var", _Var_sqcmd);
    _regcmd("if", _If_sqcmd);
    _regcmd("(endif)", _Endif_sqcmd);
    /*
        请使用这个格式来注册命令：
        _regcmd("根命令字符串", 函数名指针);
        若执行的根命令与字符串匹配，将会调用函数名指针指向的函数
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
    cout << "Squid Beta  v0b1" << endl << "Copyright MineCommander (C) 2020" << endl;
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
