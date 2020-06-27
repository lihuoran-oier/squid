const int EXIT_MAIN = 65536;
const int IFSTATES_FALSE = 100001;
const int MAXN = 2147483647;
typedef int(*Fp)(std::string subcmd);

std::map<std::string, Fp> cmd_register;
std::map<std::string, double> var_list;
std::stack<_tIfstate> ifstatu;

std::string _tempgstd(void) {
    time_t t = time(0);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y%m%d-%H%M%S", localtime(&t));
    return tmp;
}
std::ofstream logFile(("logs/" + _tempgstd() + ".log").c_str());

namespace sll {
        bool ifstate_now() {
            if (ifstatu.empty()) return true;
            _tIfstate stt = ifstatu.top();
            int oprter = 0;
            if (stt.oprt == ">" || stt.oprt == "is_bigger_than")
                oprter = 1;
            else if (stt.oprt == ">=" || stt.oprt == "isnot_less_than")
                oprter = 2;
            else if (stt.oprt == "<" || stt.oprt == "is_less_than")
                oprter = 3;
            else if (stt.oprt == "<=" || stt.oprt == "isnot_bigger_than")
                oprter = 4;
            else if (stt.oprt == "=" || stt.oprt == "==" || stt.oprt == "is")
                oprter = 5;
            else if (stt.oprt == "!=" || stt.oprt == "isnot")
                oprter = 6;
            return (stt.x1 > stt.x2 && oprter == 1)
            || (stt.x1 >= stt.x2 && oprter == 2)
            || (stt.x1 < stt.x2 && oprter == 3)
            || (stt.x1 <= stt.x2 && oprter == 4)
            || (stt.x1 == stt.x2 && oprter == 5)
            || (stt.x1 != stt.x2 && oprter == 6);
        }
        std::string getSysTimeData(void) {
            time_t t = time(0);
            char tmp[64];
            strftime(tmp, sizeof(tmp), "%Y%m%d-%H%M%S", localtime(&t));
            return tmp;
        }
        std::string getSysTime(void) {
            time_t t = time(0);
            char tmp[64];
            strftime(tmp, sizeof(tmp), "%H:%M:%S", localtime(&t));
            return tmp;
        }

        template <class Ta, class Tb>
        Tb atob(const Ta& t) {
            std::stringstream temp;
            temp << t;
            Tb i;
            temp >> i;
            return i;
        }
        struct tcLSett {
            bool sendLog = true;
            bool sendWarn = true;
            bool safeMode = true;
        }   settings;
        void sendLog(std::string msg) {
            if (settings.sendLog) std::cout << "[LOG] " << msg << std::endl;
            logFile << "[" << getSysTime() << "][LOG] " << msg << std::endl;
        }
        void sendWarn(std::string msg) {
            if (settings.sendWarn) std::cout << "[WARN] " << msg << std::endl;
            logFile << "[" << getSysTime() << "][WARN] " << msg << std::endl;
        }
        void sendError(std::string msg) {
            std::cout << "[ERROR] " << msg << std::endl;
            logFile << "[" << getSysTime() << "][ERROR] " << msg << std::endl;
        }
        void sendInfo(std::string msg) {
            std::cout << "[INFO] " << msg << std::endl;
            logFile << "[" << getSysTime() << "][INFO] " << msg << std::endl;
        }
        void sendOutput(std::string msg, bool log) {
            std::cout << msg << std::endl;
            if (log) logFile << "[" << getSysTime() << "][OUTPUT] " << msg << std::endl;
        }

    class tcSqLCmd {
    private:
        std::string quotetypes = "@$";
    public:
        std::string subcommand(std::string command)
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
        std::string compile_quote(std::string cmd)
        {
            std::string varname;
            int state = 0;
            char type;
            int ns = 0, ne = cmd.size() - 1;
            for (int i = 0; i < cmd.size(); i++) {
                if (quotetypes.find(cmd.substr(i, 1)) != std::string::npos && cmd[i + 1] == '<') {
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
                    cmd.replace(ns, ln, atob<double, std::string>(var_list[varname]));
                else if (type == '@') {
                    if (varname == "endl")
                        cmd.replace(ns, ln, "\n");
                    else if (varname == "sysTimeStamp")
                        cmd.replace(ns, ln, atob<int, std::string>(time(0)));
                    else if (varname == "sysTime")
                        cmd.replace(ns, ln, getSysTime());
                    else
                        cmd.erase(ns, ln);
                }
                cmd = compile_quote(cmd);
            }
            return cmd;
        }
        int run(std::string command) {
            command = compile_quote(command);
            std::string subcmd = subcommand(command);
            std::stringstream rtcmdpe(command);
            std::string rootcmd;
            rtcmdpe >> rootcmd;
            if (cmd_register.count(rootcmd) == 1)
                return cmd_register[rootcmd](subcmd);
            else {
                sendError("Unknown command '" + rootcmd + "'");
            }

        }
    }   command;
    void regcmd(std::string cmdstr, Fp cmdfp) {
        cmd_register.insert(make_pair(cmdstr, cmdfp));
    }
    int endifed = 0;
}