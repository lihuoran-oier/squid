const int EXIT_MAIN = 65536;
const int IFSTATES_FALSE = 100001;
const int MAXN = 2147483647;
typedef int(*Fp)(string subcmd);

map<string, Fp> cmd_register;
map<string, double> var_list;

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
    class tcIfStu {
    private:
        ;
    public:
        float x1 = 0, x2 = 0;
        string oprt;
        bool enable = false;
        bool state(void) {
            int oprter = 0;
            if (oprt == ">" || oprt == "is_bigger_than")
                oprter = 1;
            else if (oprt == ">=" || oprt == "isnot_less_than")
                oprter = 2;
            else if (oprt == "<" || oprt == "is_less_than")
                oprter = 3;
            else if (oprt == "<=" || oprt == "isnot_bigger_than")
                oprter = 4;
            else if (oprt == "=" || oprt == "==" || oprt == "is")
                oprter = 5;
            else if (oprt == "!=" || oprt == "isnot")
                oprter = 6;
            if (enable == true)
                return     (x1 > x2 && oprter == 1)
                || (x1 >= x2 && oprter == 2)
                || (x1 < x2&& oprter == 3)
                || (x1 <= x2 && oprter == 4)
                || (x1 == x2 && oprter == 5)
                || (x1 != x2 && oprter == 6);
            else
                return true;
        }
    }   if_status;
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

        template <class Ta, class Tb>
        Tb atob(const Ta& t) {
            stringstream temp;
            temp << t;
            Tb i;
            temp >> i;
            return i;
        }
    }   general;

    class tcSqLio {
    private:
        tcSqLGen tempgen;
    public:
        class tcLSett {
        private:
            ;
        public:
            bool sendLog = true;
            bool sendWarn = true;
            bool systemCommandScriptWarn = true;
        }   settings;

        void sendLog(string msg) {
            if (settings.sendLog) cout << "[LOG] " << msg << endl;
            logFile << "[" << tempgen.getSysTime() << "][LOG] " << msg << endl;
        }
        void sendWarn(string msg) {
            if (settings.sendWarn) cout << "[WARN] " << msg << endl;
            logFile << "[" << tempgen.getSysTime() << "][WARN] " << msg << endl;
        }
        void sendError(string msg) {
            cout << "[ERROR] " << msg << endl;
            logFile << "[" << tempgen.getSysTime() << "][ERROR] " << msg << endl;
        }
        void sendInfo(string msg) {
            cout << "[INFO] " << msg << endl;
            logFile << "[" << tempgen.getSysTime() << "][INFO] " << msg << endl;
        }
        void sendOutput(string msg, bool log) {
            cout << msg << endl;
            if (log) logFile << "[" << tempgen.getSysTime() << "][OUTPUT] " << msg << endl;
        }
    }   io_and_settings;
    class tcSqLCmd {
    private:
        string quotetypes = "@$";
        tcSqLGen tempgen;
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
                if (quotetypes.find(cmd.substr(i, 1)) != string::npos && cmd[i + 1] == '<') {
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
                    cmd.replace(ns, ln, tempgen.atob<double, string>(var_list[varname]));
                else if (type == '@') {
                    if (varname == "endl")
                        cmd.replace(ns, ln, "\n");
                    else if (varname == "sysTimeStamp")
                        cmd.replace(ns, ln, tempgen.atob<int, string>(time(0)));
                    else if (varname == "sysTime")
                        cmd.replace(ns, ln, tempgen.getSysTime());
                    else
                        cmd.erase(ns, ln);
                }
                cmd = compile_quote(cmd);
            }
            return cmd;
        }
        int run(string command) {
            command = compile_quote(command);
            string subcmd = subcommand(command);
            stringstream rtcmdpe(command);
            string rootcmd;
            rtcmdpe >> rootcmd;
            if (cmd_register.count(rootcmd) == 1)
                return cmd_register[rootcmd](subcmd);
            else {
                squidlanglib.io_and_settings.sendError("Unknown command'" + rootcmd + "'");
            }

        }
    }   command;


    void regcmd(string cmdstr, Fp cmdfp) {
        cmd_register.insert(make_pair(cmdstr, cmdfp));
    }

}   squidlanglib;
