const int EXIT_MAIN = 65536;
const int IFSTATES_FALSE = 100001;
const int MAXN = 2147483647;
typedef int(*Fp)(string subcmd);

string _tempgstd(void) {
    time_t t = time(0);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y%m%d-%H%M%S", localtime(&t));
    return tmp;
}
ofstream logFile(("logs/" + _tempgstd() + ".log").c_str());

class tcSqlib {
private:
    ;
public:
    class tcSqLio {
    private:
        ;
    public:
        void sendLog(string msg) {
            if (squidlib.settings.sendLog) cout << "[LOG] " << msg << endl;
            logFile << "[" << squidlib.general.getSysTime() << "][LOG] " << msg << endl;
        }
        void sendWarn(string msg) {
            if (squidlib.settings.sendWarn) cout << "[WARN] " << msg << endl;
            logFile << "[" << squidlib.general.getSysTime() << "][WARN] " << msg << endl;
        }
        void sendError(string msg) {
            cout << "[ERROR] " << msg << endl;
            logFile << "[" << squidlib.general.getSysTime() << "][ERROR] " << msg << endl;
        }
        void sendInfo(string msg) {
            cout << "[INFO] " << msg << endl;
            logFile << "[" << squidlib.general.getSysTime() << "][INFO] " << msg << endl;
        }
        void sendOutput(string msg, bool log) {
            cout << msg << endl;
            if (log) logFile << "[" << squidlib.general.getSysTime() << "][OUTPUT] " << msg << endl;
        }
    }   io;
    class tcSqLGen {
    private:
        ;
    public:
        string getSysTimeData(void) {
            time_t t = time(0);
            char tmp[64];
            strftime(tmp, sizeof(tmp), "%Y%m%d-%H%M%S", localtime(&t));
            return tmp;
        }
        string getSysTime(void) {
            time_t t = time(0);
            char tmp[64];
            strftime(tmp, sizeof(tmp), "%H:%M:%S", localtime(&t));
            return tmp;
        }
        bool ifstate(void) {
            int oprt = 0;
            if (squidlib.if_status.oprt == ">" || squidlib.if_status.oprt == "is_bigger_than")
                oprt = 1;
            else if (squidlib.if_status.oprt == ">=" || squidlib.if_status.oprt == "isnot_less_than")
                oprt = 2;
            else if (squidlib.if_status.oprt == "<" || squidlib.if_status.oprt == "is_less_than")
                oprt = 3;
            else if (squidlib.if_status.oprt == "<=" || squidlib.if_status.oprt == "isnot_bigger_than")
                oprt = 4;
            else if (squidlib.if_status.oprt == "=" || squidlib.if_status.oprt == "==" || squidlib.if_status.oprt == "is")
                oprt = 5;
            else if (squidlib.if_status.oprt == "!=" || squidlib.if_status.oprt == "isnot")
                oprt = 6;
            if (squidlib.if_status.enable == true)
                return     (squidlib.if_status.x1 > squidlib.if_status.x2 && oprt == 1)
                || (squidlib.if_status.x1 >= squidlib.if_status.x2 && oprt == 2)
                || (squidlib.if_status.x1 < squidlib.if_status.x2&& oprt == 3)
                || (squidlib.if_status.x1 <= squidlib.if_status.x2 && oprt == 4)
                || (squidlib.if_status.x1 == squidlib.if_status.x2 && oprt == 5)
                || (squidlib.if_status.x1 != squidlib.if_status.x2 && oprt == 6);
            else
                return true;
        }
        template <class Ta, class Tb>
        Tb atob(const Ta& t) {
            stringstream temp;
            temp << t;
            Tb i;
            temp >> i;
            return i;
        }
    }   general;
    class tcIfStu {
    private:
        ;
    public:
        float x1 = 0, x2 = 0;
        string oprt;
        bool enable = false;
    }   if_status;
    class tcLSett {
    private:
        ;
    public:
        bool sendLog = true;
        bool sendWarn = true;
        bool systemCommandScriptWarn = true;
    }   settings;
    class tcSqLCmd {
    private:
        ;
    public:
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
        string compile_quote(string cmd)
        {
            string varname;
            int state = 0;
            char type;
            int ns = 0, ne = cmd.size() - 1;
            for (int i = 0; i < cmd.size(); i++) {
                if (squidlib.quotetypes.find(cmd.substr(i, 1)) != string::npos && cmd[i + 1] == '<') {
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
                    cmd.replace(ns, ln, squidlib.general.atob<double, string>(squidlib.var_list[varname]));
                else if (type == '@') {
                    if (varname == "endl")
                        cmd.replace(ns, ln, "\n");
                    else if (varname == "sysTimeStamp")
                        cmd.replace(ns, ln, squidlib.general.atob<int, string>(time(0)));
                    else if (varname == "sysTime")
                        cmd.replace(ns, ln, squidlib.general.getSysTimeData());
                    else
                        cmd.erase(ns, ln);
                }
                cmd = compile_quote(cmd);
            }
            return cmd;
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
            if (squidlib.cmd_register.count(rootcmd) == 1)
                return squidlib.cmd_register[rootcmd](subcmd);
            else {
                squidlib.io.sendError("Unknown command'" + rootcmd + "'");
            }

        }
    }   command;


    void regcmd(string cmdstr, Fp cmdfp) {
        squidlib.cmd_register.insert(make_pair(cmdstr, cmdfp));
    }
    
    map<string, double> var_list;
    map<string, Fp> cmd_register;
    string quotetypes = "@$";

}   squidlib;