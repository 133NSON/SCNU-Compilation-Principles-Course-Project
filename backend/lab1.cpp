// author: 张峻霖
// description: 编译原理课程项目任务1
// date: 2024-5-1
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stack>
#include <vector>
#include <set>
#include <algorithm>
using namespace std;

struct Link_node {
    int state_num;
    char worker;
    Link_node* next;
    bool accept = false;
    Link_node(int s = -1, char w = '#', Link_node* n = nullptr): state_num(s), worker(w), next(n){}
};

struct NFA {
    Link_node* start;
    Link_node* end;
    NFA(Link_node* s = nullptr, Link_node* e = nullptr): start(s), end(e) {}
};

stack<NFA*> nfa_stack;

struct Next_id {
    int id;
    char worker;
    Next_id(int i, char w): id(i), worker(w) {}
};

struct DFA {
    set<int> state_set;
    char worker;
    DFA* next;
    bool accept;
    int id;
    vector<Next_id*> nid;
    DFA(char w = '#', DFA* n = nullptr, bool b = false): worker(w), next(n), accept(b) {}
};

struct minDFA {
    int id; // 自身的最小DFA id
    bool init;
    bool accept;
    vector<Next_id> DFAid; // DFA状态集合
    minDFA(int i = 0, bool ii = false, bool a = false): id(i), init(ii), accept(a) {}
};

struct Link_list{
    // Link_list();
    set<char> alphabet;
    vector<Link_node*> adjacency_list; //NFA邻接表
    Link_node* first;
    vector<DFA*> dfa_list;
    DFA* dfa_first;
    set<minDFA*> minDFA_set;
    vector<minDFA> minDFA_list;

    void clear() {
        alphabet.clear();
        adjacency_list.clear();
        first = nullptr;
        dfa_list.clear();
        dfa_first = nullptr;
        minDFA_set.clear();
        minDFA_list.clear();
    }

    NFA* addNFA(NFA* s, NFA* e) {
        Link_node* a = new Link_node(e->start->state_num, '^');
        adjacency_list[s->end->state_num]->next = a;
        e->start->worker = '^';
        NFA* new_n = new NFA(s->start, e->end);
        return new_n;
    }

    NFA* orNFA(int& state_cnt, NFA *s, NFA *e){
        Link_node* new_s = new Link_node(state_cnt++, '#');
        adjacency_list.push_back(new_s);

        s->start->worker = '^';
        e->start->worker = '^';
        Link_node* new_etable = new Link_node(e->start->state_num, e->start->worker);
        Link_node* new_stable = new Link_node(s->start->state_num, s->start->worker);
        new_stable->next = new_etable;
        new_s->next = new_stable;

        Link_node* new_e = new Link_node(state_cnt++, '^');
        adjacency_list.push_back(new_e);

        Link_node* new_etable2 = new Link_node(new_e->state_num, new_e->worker);
        adjacency_list[s->end->state_num]->next = new_etable2;
        Link_node* new_etable3 = new Link_node(new_e->state_num, new_e->worker);
        adjacency_list[e->end->state_num]->next = new_etable3;

        NFA* new_n = new NFA(new_s, new_e);
        return new_n;
    }

    NFA* closureNFA(int& state_cnt, NFA* s) {
        Link_node* new_s = new Link_node(state_cnt++, '#');
        s->start->worker = '^';
        Link_node* new_stable = new Link_node(s->start->state_num, s->start->worker);
        new_s->next = new_stable;
        adjacency_list.push_back(new_s);

        Link_node* new_e = new Link_node(state_cnt++, '^');
        adjacency_list.push_back(new_e);

        Link_node* new_etable = new Link_node(new_e->state_num, '^');
        Link_node* new_etable2 = new Link_node(new_e->state_num, '^');
        adjacency_list[s->end->state_num]->next = new_etable;
        new_stable->next = new_etable2;

        Link_node* new_stable2 = new Link_node(s->start->state_num, s->start->worker);
        new_etable->next = new_stable2;

        NFA* new_n = new NFA(new_s, new_e);
        return new_n;
    }

    NFA* selectNFA(int &state_cnt, NFA *s) {
        Link_node* new_s = new Link_node(state_cnt++, '#');
        s->start->worker = '^';
        Link_node* new_stable = new Link_node(s->start->state_num, s->start->worker);
        new_s->next = new_stable;
        adjacency_list.push_back(new_s);//加入邻接表数组
        //新的终态并连接
        Link_node* new_e = new Link_node(state_cnt++, '^');
        adjacency_list.push_back(new_e);//加入邻接表数组

        Link_node* new_etable = new Link_node(new_e->state_num, '^');
        Link_node* new_etable2 = new Link_node(new_e->state_num, '^');
        adjacency_list[s->end->state_num]->next = new_etable;//旧终态连接
        new_stable->next = new_etable2;//新初态连接

        //压入新的NFA部分
        NFA* new_n = new NFA(new_s, new_e);
        return new_n;
    }

    NFA* closureNFA_op(int& state_num, NFA* s) {
        //新的初态并连接
        Link_node* new_s = new Link_node(state_num++, '#');
        s->start->worker = '^';
        Link_node* new_stable = new Link_node(s->start->state_num, s->start->worker);
        new_s->next = new_stable;
        adjacency_list.push_back(new_s);//加入邻接表数组

        //新的终态并连接
        Link_node* new_e = new Link_node(state_num++, '^');
        adjacency_list.push_back(new_e);//加入邻接表数组

        Link_node* new_etable = new Link_node(new_e->state_num, '^');
        adjacency_list[s->end->state_num]->next = new_etable;//旧终态连接

        //旧初态和旧终态的连接
        Link_node* new_stable2 = new Link_node(s->start->state_num, s->start->worker);
        new_etable->next = new_stable2;

        //压入新的NFA部分
        NFA* new_n = new NFA(new_s, new_e);
        return new_n;
    }

    void dfsEpsilon(Link_node* p, set<int>& f) {
        while(p != nullptr) {
            if(p->worker == '^' || p->worker == '#') {
                f.insert(p->state_num);
                if(p->next != nullptr)
                    dfsEpsilon(adjacency_list[p->next->state_num], f);
            } else
                return;
            p = p->next;
        }
    }

    void dfsChar(DFA* p, char c, set<int>& s) {
        vector<int> t;
        set<int>::iterator iter;
        for (iter = p->state_set.begin(); iter != p->state_set.end(); iter++) {
            if (s.count(adjacency_list[*iter]->state_num))//不重复添加
                continue;
            Link_node* a = adjacency_list[*iter]->next;//查找在该状态中的所有结点的邻接点
            while (a != nullptr) {
                if(a->worker == c && !count(t.begin(), t.end(), adjacency_list[*iter]->state_num))  //如果是通过一个字符c到达的结点则放入t
                    t.push_back(a->state_num);
                a = a->next;
            }
        }
        //已有的集合经过一个字符c能到达的点也需要加入集合
        set<int>::iterator iter1;
        for (iter1 = s.begin(); iter1 != s.end(); iter1++) {
            Link_node* a = adjacency_list[*iter1]->next;
            while(a != nullptr)
            {
                if(a->worker == c)
                    s.insert(a->state_num);
                a = a->next;
            }
        }

        for (size_t i = 0;i < t.size();i++)//遍历所有a状态能通过字符c到达的结点
        {
            Link_node* a = adjacency_list[t[i]]->next;//遍历这些结点的邻接点
            while(a != nullptr)
            {
                // if(a->state_num == 11) qDebug()<<"11";
                dfsEpsilon(adjacency_list[a->state_num], s);//把通过一个字符c能到达的结点的能通过epsilon到达的结点加入集合s
                a = a->next;
            }
        }
        for (const int &i: t)
            s.insert(i);
    }

    void initDFA() {
        first = nfa_stack.top()->start;
        dfa_first = new DFA();
        dfa_first->id = 0;
        dfsEpsilon(first, dfa_first->state_set);
        dfa_list.push_back(dfa_first);
    }

    bool check_have(set<int> s) {
        for (auto x : dfa_list) {
            if (x->state_set == s) {
                return true;
            }
        }
        return false;
    }

    void constructDFA() {
        initDFA(); //初始化表头结点
        vector<char> alphabet_;
        alphabet_.assign(alphabet.begin(), alphabet.end());
        int id_cnt = 1;
        for (size_t j = 0;j < dfa_list.size();j++) {
            for (size_t i = 0;i < alphabet_.size();i++) {
                DFA* d = new DFA();
                dfsChar(dfa_list[j], alphabet_[i], d->state_set);

                if(d->state_set.size() != 0) {
                    d->worker = alphabet_[i]; //经过的字符 alphabet[i]
                    d->next = dfa_list[j]->next;
                    dfa_list[j]->next = d;
                }
                else {
                    delete d;
                    continue;
                }
                DFA* new_d = new DFA();
                new_d->state_set = d->state_set; //复制一份 d 的信息，不能直接连接到邻接表中
                new_d->worker = d->worker;

                if (new_d->state_set.size() != 0  && dfa_list[j]->state_set != new_d->state_set && !check_have(d->state_set)) {
                    new_d->id = d->id = id_cnt++;
                    dfa_list.push_back(new_d);
                }
            }
        }
        for (auto x : dfa_list) {
            if (count(x->state_set.begin(), x->state_set.end(), adjacency_list.size() - 1))
                x->accept = true;
        }
        for (size_t i = 0; i < dfa_list.size(); i++) {
            for (auto x = dfa_list[i]->next; x; x = x->next) {
                for (auto xx : dfa_list) {
                    if (x->state_set == xx->state_set) {
                        x->id = xx->id;
                    }
                }
                Next_id* tmp = new Next_id(x->id, x->worker);
                dfa_list[i]->nid.push_back(tmp);
            }
        }
    }

    void construct_minDFA() {
        vector<set<int>> minDFA_set;    // 终态DFA的id
        minDFA_set.push_back({}); // 终态0
        minDFA_set.push_back({}); // 非终态1
        for (auto x : dfa_list) {
            if (x->accept)
                minDFA_set[0].insert(x->id);
            else
                minDFA_set[1].insert(x->id);
        }
        size_t og_size = minDFA_set.size();
        while (true) {
            for (auto itr = minDFA_set.begin(); itr != minDFA_set.end(); itr++) {
                auto x = *itr;
                for (auto c : alphabet) {
                    set<int> tmp;
                    auto it = minDFA_set.end();
                    for (auto i : x) {
                        for (auto xx : dfa_list[i]->nid) {
                            if (xx->worker == c) {
                                if (it == minDFA_set.end()) {
                                    for (auto itt = minDFA_set.begin(); itt != minDFA_set.end(); itt++) {
                                        auto xxx = *itt;
                                        if (count(xxx.begin(), xxx.end(), xx->id)) {
                                            it = itt;
                                            if (it == minDFA_set.end()) {
                                                cerr << "ERROR" << endl;
                                                exit(1);
                                            }
                                            break;
                                        }
                                    }
                                    tmp.insert(i);
                                } else {
                                    auto itt = minDFA_set.begin();
                                    for (; itt != minDFA_set.end(); itt++) {
                                        auto xxx = *itt;
                                        if (count(xxx.begin(), xxx.end(), xx->id)) {
                                            break;
                                        }
                                    }
                                    if (itt == it) {
                                        tmp.insert(i);
                                    }
                                }
                                break;
                            }
                        }
                    }
                    set<int> diff;
                    for (auto i : x) {
                        if (!count(tmp.begin(), tmp.end(), i)) {
                            diff.insert(i);
                        }
                    }
                    if (!tmp.empty() && !diff.empty()) {
                        minDFA_set.erase(itr);
                        minDFA_set.push_back(tmp);
                        minDFA_set.push_back(diff);
                        itr = minDFA_set.begin();
                        break;
                    }
                }
            }
            if (og_size == minDFA_set.size())
                break;
            else
                og_size = minDFA_set.size();
        }
        for (size_t i = 0; i < minDFA_set.size(); i++) {
            minDFA tmp(i);
            minDFA_list.push_back(tmp);
        }
        for (size_t i = 0; i < minDFA_list.size(); i++) {
            int DFA_idx = *(minDFA_set[i].begin());
            minDFA_list[i].accept = dfa_list[DFA_idx]->accept;
            if (dfa_list[DFA_idx]->worker == '#') {
                minDFA_list[i].init = true;
            }
            for (auto x = dfa_list[DFA_idx]->next; x; x = x->next) {
                int tmp_id = x->id;
                char tmp_worker = x->worker;
                int min_DFA_nid;
                for (size_t ii = 0; ii < minDFA_list.size(); ii++) {
                    if (count(minDFA_set[ii].begin(), minDFA_set[ii].end(), tmp_id)) {
                        min_DFA_nid = ii;
                        break;
                    }
                }
                Next_id tmp_nid(min_DFA_nid, tmp_worker);
                minDFA_list[i].DFAid.push_back(tmp_nid);
            }
        }
    }
} link_table;

bool check_highest_p(char c) {
    return c == '*' || c == '?';
}

bool in_charset(char c) {
    return isalnum(c) || c == '+' || c == '-' || c == '.' || c == '{' || c == '}' || c == '[' || c == ']' || c == '<' || c == '>' || c == ':' || c == '=' || c == '%'|| c == '/' || c == ';' || c == '_';
}

string handle_raw(string raw) {
    string ret;
    size_t length = raw.length();
    for (size_t i = 0;i < length; i++) {
        if (raw[i] == '\\') {
            ret += raw[i++];
            ret += raw[i];
            if (i < length - 1 && (in_charset(raw[i+1]) || raw[i+1] == '\\' || raw[i+1] == '(' ))
                ret += '&';
        } else {
            ret += raw[i];
            if (i < length - 1 && ((in_charset(raw[i]) && (in_charset(raw[i+1]) || raw[i+1] == '\\')) || (in_charset(raw[i]) && raw[i+1] == '(') || (check_highest_p(raw[i]) && (in_charset(raw[i+1]) || raw[i+1] == '\\')) || (check_highest_p(raw[i]) && raw[i+1] == '(') || (raw[i] == ')' && (in_charset(raw[i+1]) || raw[i+1] == '\\')) || (raw[i] == ')' && raw[i+1] == '(')))
                ret += '&';
        }
    }
    return ret;
}

int priority(char c) {
    switch (c) {
        case '*':
        case '?':
            return 4;
        case '&':
            return 3;
        case '|':
            return 2;
        case '(':
            return 1;
        default:
            cerr << "INVALID CHARACTER DETECTED" << endl;
            exit(1);
    }
}

string operands;

void re2pe(string re) {
    operands.clear();
    stack<char> operators;
    int i = 0;
    int length = re.length();

    while(i < length) {
        if (re[i] == '\\') {
            operands += re[i];
            operands += re[++i];
        }
        else if(in_charset(re[i]) || check_highest_p(re[i]))
            operands += re[i];
        else if(re[i] == '(')
            operators.push(re[i]);
        else if (re[i] == ')') {
            while(operators.top() != '(') {
                operands += operators.top();
                operators.pop();
            }
            operators.pop();
        }
        else if ((re[i] == '&') || (re[i] == '|')) {
            if (!operators.empty()) {
                char t = operators.top();
                if(priority(t) >= priority(re[i])) {
                    operands += operators.top();
                    operators.pop();
                }
            }
            operators.push(re[i]);//把当前运算符入栈
        }
        if (i == length-1) {
            while (!operators.empty()) {
                operands += operators.top();
                operators.pop();
            }
        }
        i++;
    }
}

void constructNFA() {
    int state_cnt = 0;
    while (!nfa_stack.empty())
        nfa_stack.pop();
    for (size_t i = 0; i < operands.size(); i++) {
        char c = operands[i];
        if (c == '\\') {
            i++;
            c = operands[i];
            Link_node* s = new Link_node(state_cnt++);
            link_table.adjacency_list.push_back(s);
            Link_node* e = new Link_node(state_cnt, c);
            Link_node* ee = new Link_node(state_cnt++, c);
            link_table.adjacency_list.push_back(e);
            NFA* new_n = new NFA(s, e);
            nfa_stack.push(new_n);
            s->next = ee;
            link_table.alphabet.insert(c);
        } else if (in_charset(c)) {
            Link_node* s = new Link_node(state_cnt++);
            link_table.adjacency_list.push_back(s);
            Link_node* e = new Link_node(state_cnt, c);
            Link_node* ee = new Link_node(state_cnt++, c);
            link_table.adjacency_list.push_back(e);
            NFA* new_n = new NFA(s, e);
            nfa_stack.push(new_n);
            s->next = ee;
            link_table.alphabet.insert(c);
        } else if (c == '&') {
            NFA* e = nfa_stack.top();
            nfa_stack.pop();
            NFA* s = nfa_stack.top();
            nfa_stack.pop();
            NFA* new_n = link_table.addNFA(s, e);
            nfa_stack.push(new_n);
            delete e;
            delete s;
        } else if (c == '|') {
            NFA* e = nfa_stack.top();
            nfa_stack.pop();
            NFA* s = nfa_stack.top();
            nfa_stack.pop();
            NFA* new_n = link_table.orNFA(state_cnt, s, e);
            nfa_stack.push(new_n);
            delete e;
            delete s;
        } else if (c == '*') {
            NFA* s = nfa_stack.top();
            nfa_stack.pop();
            NFA* new_n = link_table.closureNFA(state_cnt, s);
            nfa_stack.push(new_n);
            delete s;
        } else if (c == '?') {
            NFA* s = nfa_stack.top();
            nfa_stack.pop();
            NFA* new_n = link_table.selectNFA(state_cnt, s);
            nfa_stack.push(new_n);
            delete s;
        } else {
            cerr << "INVALID CHARACTER DETECTED" << endl;
            exit(1);

        }
    }
}

vector<string> id_arr;  // 标识符
vector<vector<minDFA>> minDFA_list_arr; // 每个标识符的最小DFA
string code;    // 词法分析程序代码

string construct_code() {
    int start_state;
    set<int> end;
    code = "#include <iostream>\n";
    code += "#include <fstream>\n";
    code += "using namespace std;\n";
    // code += "ifstream src_file(\"resources/source.txt\", ios::in);\n";
    // code += "ofstream ans_file(\"resources/ans.txt\", ios::out);\n";
    code += "ifstream src_file(\"resources/source.txt\", ios::in);\n";
    code += "ofstream ans_file(\"resources/ans.txt\", ios::out);\n";
    code += "string buf;\n";
    code += "string buf_suc;\n";
    code += "string buf_err;\n";
    code += "string token;\n";
    code += "string token_suc;\n";
    code += "int read_cnt;\n";
    code += R"(
void skip_Whitespace() {
    char c;
    while (src_file.get(c)) {
		read_cnt++;
		if (c == '\n') read_cnt++;
        if (!isspace(c)) {
            read_cnt--;
            src_file.unget();
            break;
        }
    }
}

)";
    for (size_t j = 0; j < minDFA_list_arr.size(); j++) {
        for (auto x : minDFA_list_arr[j]) {
            if (x.init)
                start_state = x.id;
            if (x.accept) {
                end.insert(x.id);
            }
        }
        code += "bool check_" + id_arr[j] + "() {\n";
        code += "\tint state = " + to_string(start_state) + ";\n";
        code += "\tchar c;\n";
        code += "\twhile((c = src_file.peek()) != EOF) {\n";
        code += "\t\tswitch(state) {\n";
        for (size_t i = 0; i < minDFA_list_arr[j].size(); i++) {
            code += "\t\t\tcase " + to_string(i) + ":\n";
            if (minDFA_list_arr[j][i].DFAid.empty()) {
                code += "\t\t\t\ttoken = \"" + id_arr[j] + "\";\n";
                code += "\t\t\t\treturn true;\n";
                continue;
            }
            code += "\t\t\t\tswitch(c) {\n";
            for (auto x : minDFA_list_arr[j][i].DFAid) {
                code += "\t\t\t\t\tcase '" + string(1, x.worker) + "': \n" + "\t\t\t\t\t\tstate = " + to_string(x.id) + ";\n" + "\t\t\t\t\t\tbuf += c;\n" + "\t\t\t\t\t\tsrc_file.get(c);\n" + "\t\t\t\t\t\tbreak;\n";
            }
            code += "\t\t\t\t\tdefault :\n";
            if (minDFA_list_arr[j][i].accept == true) {
                code += "\t\t\t\t\t\ttoken = \"" + id_arr[j] + "\";\n";
                code += "\t\t\t\t\t\treturn true;\n";
            }
            else
                code += "\t\t\t\t\t\treturn false;\n";
            code += "\t\t\t\t}\n";
            code += "\t\t\t\tbreak;\n";
        }
        code += "\t\t}\n";
        code += "\t}\n";
        code += "}\n\n";
    }
    code += "int main() {\n";
    code += "\tbool flag;\n";
    code += "\tchar c;\n";
    code += "\twhile((c = src_file.peek()) != EOF) {\n";
    code += "\t\ttoken_suc.clear();\n";
    code += "\t\tbuf_suc.clear();\n";
    for (size_t i = 0; i < minDFA_list_arr.size(); i++) {
        code += "\t\tif (!check_" + id_arr[i] + "())\n";
        code += "\t\t\tbuf_err = buf;\n";
        code += "\t\telse if (buf.size() > buf_suc.size()) {\n";
        code += "\t\t\tbuf_suc = buf;\n";
        code += "\t\t\ttoken_suc = token;\n";
        code += "\t\t}\n";
        code += "\t\tbuf.clear();\n";
        code += "\t\tsrc_file.seekg(read_cnt, ios::beg);\n";
    }
    code += "\t\tif (buf_suc.empty()) {\n";
    code += "\t\t\tans_file << buf << \"UNKNOWN \" << buf_err << endl;\n";
    code += "\t\t\texit(1);\n";
    code += "\t\t} else\n";
    code += "\t\t\tans_file << token_suc << \" \" << buf_suc << endl;\n";
    code += "\t\tread_cnt += buf_suc.size();\n";
    code += "\t\tsrc_file.seekg(read_cnt, ios::beg);\n";
    code += "\t\tskip_Whitespace();\n";
    code += "\t}\n";
    code += "}\n";
    return code;
}

int main() {
    string id;
    string raw;
    string tmp;
    ifstream src_file("resources/reg.txt", ios::in);
    ofstream nfa_file("resources/nfa.txt", ios::out);
    ofstream dfa_file("resources/dfa.txt", ios::out);
    ofstream min_dfa_file("resources/min.txt", ios::out);
    ofstream code_file("resources/code.cpp", ios::out);
    if (!src_file || !nfa_file || !dfa_file || !code_file || !min_dfa_file)
        exit(1);

    while (getline(src_file, raw)) {
        stringstream ss(raw);
        ss >> id >> raw;
        id_arr.push_back(id);
        string re = handle_raw(raw);
        re2pe(re);

        link_table.clear();

        constructNFA();
        vector<char> vec;
        vec.assign(link_table.alphabet.begin(), link_table.alphabet.end());
        vec.push_back('^');
        nfa_file << "<p style=\"color: yellow;\">" << id << "</p>";
        nfa_file << "<table>";
        nfa_file << "<tr>";
        nfa_file << " <td> </td><td> </td>";
        for (size_t i = 0; i < vec.size(); i++)
            nfa_file << "<td>" << vec[i] << "</td>";
        nfa_file << "</tr>";
        link_table.adjacency_list[link_table.adjacency_list.size() - 1]->accept = true;
        for (auto x : link_table.adjacency_list) {
            nfa_file << "<tr>";
            if (x->worker == '#')
                nfa_file << "<td>-</td>";
            else if(x->accept)
                nfa_file << "<td>+</td>";
            else
                nfa_file << "<td> </td>";
            nfa_file << "<td>" << x->state_num << "</td>";
            for (auto xx : vec) {
                vector<int> tmp;
                for (auto nfa = x->next; nfa; nfa = nfa->next) {
                    if (nfa->worker == xx) {
                        tmp.push_back(nfa->state_num);
                    }
                }
                nfa_file << "<td>";
                for (auto i : tmp) {
                    if (i != tmp.back())
                        nfa_file << i << ",";
                    else
                        nfa_file << i;
                }
                if (xx != vec.back())
                    nfa_file << "</td>";
            }
            if (x != link_table.adjacency_list.back())
                nfa_file << "</tr>";
        }
        nfa_file << "</table>";

        link_table.constructDFA();
        dfa_file << "<p style=\"color: yellow;\">" << id << "</p>";
        dfa_file << "<table>";
        dfa_file << "<tr>";
        dfa_file << " <td> </td><td> </td>";
        vec.pop_back();
        for (size_t i = 0; i < vec.size(); i++) {
            auto x = vec[i];
            dfa_file << "<td>" << x << "</td>";
        }
        dfa_file << "</tr>";
        for (size_t i = 0; i < link_table.dfa_list.size(); i++) {
            DFA* a = link_table.dfa_list[i];
            dfa_file << "<tr>";
            if (a->accept)
                dfa_file << "<td>+</td>";
            else if (a->worker == '#')
                dfa_file << "<td>-</td>";
            else
                dfa_file << "<td> </td>";
            dfa_file << "<td>{";
            size_t cnt = 0;
            for (auto& e : a->state_set) {
                dfa_file << e;
                if (cnt != a->state_set.size() - 1) {
                    dfa_file << ",";
                }
                cnt++;
            }
            dfa_file << "}</td>";
            for (auto xx : vec) {
                bool flag = true;
                for (auto dfa = a->next; dfa; dfa = dfa->next)  {
                    if (dfa->worker == xx) {
                        dfa_file << "<td>{";
                        size_t ii = 0;
                        for (auto it = dfa->state_set.begin(); it != dfa->state_set.end(); it++, ii++) {
                            auto e = *it;
                            dfa_file << e;
                            if (ii != dfa->state_set.size() - 1)
                                dfa_file << ",";
                        }
                        flag = false;
                        dfa_file << "}</td>";
                        break;
                    }
                }
                if (flag)
                    dfa_file << "<td></td>";
            }
            dfa_file << "</tr>";
        }
        dfa_file << "</table>";

        link_table.construct_minDFA();
        minDFA_list_arr.push_back(link_table.minDFA_list);
        min_dfa_file << "<p style=\"color: yellow;\">" << id << "</p>";
        min_dfa_file << "<table>";
        min_dfa_file << "<tr>";
        min_dfa_file << " <td> </td><td> </td>";
        for (size_t i = 0; i < vec.size(); i++)
            min_dfa_file << "<td>" << vec[i] << "</td>";
        min_dfa_file << "</tr>";
        for (size_t i = 0; i < link_table.minDFA_list.size(); i++) {
            auto x = link_table.minDFA_list[i];
            min_dfa_file << "<tr>";
            if (x.init)
                min_dfa_file << "<td>-</td>";
            else if (x.accept)
                min_dfa_file << "<td>+</td>";
            else
                min_dfa_file << "<td> </td>";
            min_dfa_file << "<td>" << x.id << "</td>";
            for (auto c : vec) {
                bool flag = true;
                for (auto xx : x.DFAid) {
                    if (xx.worker == c) {
                        min_dfa_file << "<td>" << xx.id << "</td>";
                        flag = false;
                        break;
                    }
                }
                if (flag) {
                    min_dfa_file << "<td></td>";
                }
            }
            min_dfa_file << "</tr>";
        }
        min_dfa_file << "</table>";
    }
    string code = construct_code();
    code_file << code;

    nfa_file.close();
    dfa_file.close();
    min_dfa_file.close();
    code_file.close();
}
