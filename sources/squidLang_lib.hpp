typedef std::vector<std::string> lcmd;
typedef int(*Fp)(const lcmd &args);
namespace sll {
    struct _tIfstate {
        float x1 = 0, x2 = 0;
        std::string oprt;
    };
    struct tVar {
        std::string name;
        double valve = 0;
    };
    const int EXIT_MAIN = 65536;
    const int IFSTATES_FALSE = 100001;
    const int MAXN = 2147483647;
    struct tCmdreg {
        std::string rootcmd;
        Fp func;
        int argcMin;
        int argcMax;
    };
    std::vector<tCmdreg> cmd_register;
    std::map<std::string, double> var_list;
    std::vector<_tIfstate> ifstatu;
    bool j_ifstate(_tIfstate stt) {
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
            || (stt.x1 < stt.x2&& oprter == 3)
            || (stt.x1 <= stt.x2 && oprter == 4)
            || (stt.x1 == stt.x2 && oprter == 5)
            || (stt.x1 != stt.x2 && oprter == 6);
    }
    bool ifstate_now() {
        if (ifstatu.empty()) return true;
        for (std::vector<_tIfstate>::iterator i = ifstatu.begin(); i != ifstatu.end(); i++) {
            if (!j_ifstate(*i)) return false;
        }
        return true;
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
    std::fstream logFile(("logs/" + getSysTimeData() + ".log").c_str(), std::ios::out);
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
        std::string compile_quote(std::string cmd)
        {
            std::string varname;
            short state = 0;
            char type;
            int ns = 0, ne = cmd.size() - 1;
            for (int i = 0; i < cmd.size(); i++) {
                if (quotetypes.find(cmd.substr(i, 1)) != std::string::npos && cmd[i + 1] == '{') {
                    type = cmd[i];
                    varname.clear();

                    state = 1;
                    ns = i;
                    i += 2;
                }
                else if (state == 1 && cmd[i] == '}') {
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
        int run(std::string command) {  //v0b3新功能：支持解析多行文本
            command += "\n";
            std::vector<lcmd> cmdlines;
            lcmd ltemp;
            std::string strbuf;
            char state = 0;
            for (std::string::iterator li = command.begin(); li != command.end(); li++) {
            	if (*li == '\n') {
            		if (!strbuf.empty()) {
            			ltemp.push_back(strbuf);
            			strbuf.clear();
            		}
            		state = 0;
            		if (!ltemp.empty() && ltemp[0].at(0) != '#') {
            	    	cmdlines.push_back(ltemp);
            	    	ltemp.clear();
            		}
            	}
            	else if (state == 0) {
                    if (*li == ' ')
                        if (!strbuf.empty()) {
                            ltemp.push_back(strbuf);
                            strbuf.clear();
                        }
                        else;
                    else if (*li == '"') {
                        if (!strbuf.empty()) {
                            ltemp.push_back(strbuf);
                            strbuf.clear();
                        }
                        state = 1;
                    }
                    else strbuf.push_back(*li);
            	}
            	else if (state == 1) {
                    if (*li == '"') {
                        if (!strbuf.empty()) {
                            ltemp.push_back(strbuf);
                            strbuf.clear();
                        }
                        state = 0;
                    }
        	    	else strbuf.push_back(*li);
            	}
            }
            
            for (std::vector<lcmd>::iterator i = cmdlines.begin(); i != cmdlines.end(); i++) {
                for (lcmd::iterator j = i->begin(); j != i->end(); j++) {
                    *j = compile_quote(*j);
                }
                bool scfl = false;
                for (std::vector<tCmdreg>::iterator cf = cmd_register.begin(); cf != cmd_register.end(); cf++) {
                    if (cf->rootcmd == (*i)[0]) {
                    	scfl = true;
                        if (i->size() >= cf->argcMin && i->size() <= cf->argcMax) {
                            if (cf->func(*i) == EXIT_MAIN)
                                return EXIT_MAIN;
                        }
                        else {
                            std::stringstream msgtemp;
                            msgtemp << "Incorrect parameters count: Detected " << i->size() << " but needed [" << cf->argcMin << "," << cf->argcMax << "]";
                            sendError(msgtemp.str());
                        }
                    }
                }
                if (!i->empty() && !scfl) sendError("Unknown command '" + (*i)[0] + "'");
            }
            return 0;
        }
    }   command;
    void regcmd(std::string cmdstr, //根目录字符串
                Fp cmdfp,           //函数指针
                int argcMin,        //需要的最少参数数量
                int argcMax) {      //需要的最大参数数量
                                    //若参数数量不处于[argcMin,argcMax]中将会报错。
        tCmdreg temp{ cmdstr,cmdfp,argcMin,argcMax };
        cmd_register.push_back(temp);
    }
}