// author: 133NSON
// description: 编译原理课程项目任务2
// date: 2024-5-1
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <stack>
#include <cctype>
#include <algorithm>
using namespace std;

enum action_type {
    SHIFT,
    REDUCE,
    ACCEPT,
    CONFLICT
};
typedef pair<char, string> Rule;
vector<Rule> rules;
map<char, set<char>> first;
map<char, set<char>> follow;
vector<Rule> lrpj;
struct State {
    int id;
    set<Rule> srules;
};
vector<State> lrpj_sets;
set<char> noend_sym_table;
set<char> end_sym_table;
set<char> all_sym_table;
map<pair<int, char>, int> goto_table;
struct Action {
    action_type type;
    int next;
    Action(action_type a, int n = -1): type(a), next(n) {}
    Action(): type(SHIFT), next(-1) {}
};
map<pair<int, char>, Action> action_table;
char S;
string slr_error;

bool check_itst(const set<char>& set1, const set<char>& set2) { // check if two sets have intersection
    for (char element : set2) {
        if (set1.find(element) != set1.end()) {
            return true;
        }
    }
    return false;
}

template<class T>
bool check_have(char key, T &t) {    // check if the key already in the template class
    auto it = t.find(key);
    if (it != t.end()) return true;
    else return false;
}

bool check_action_have(const pair<int, char>& key) {
    return (action_table.find(key) != action_table.end());
}

bool check_shift_have(int id) {
    for (auto x : end_sym_table) {
        if (check_action_have({id, x})) return true;
    }
    return false;
}

bool check_goto_have(const pair<int, char>& key) {
    return (goto_table.find(key) != goto_table.end());
}

void construct_first(char key) {    // solve the first set of the key symbol
    if (check_have(key, first)) return;   // if the key has been solved, return
    vector<Rule> pending_rules;
    for (auto x : rules) {
        if (x.first == key) {
            for (auto y : x.second) {   // loop until find a symbol of which first set not includes '#'
                if (y == key) {
                    pending_rules.push_back(x);
                    break;
                }
                if (noend_sym_table.find(y) != noend_sym_table.end()) {
                    if (!check_have(y, first)) construct_first(y);
                    first[key].insert(first[y].begin(), first[y].end());
                    if (!check_have('#', first[y]))
                        break;
                } else {
                    first[key].insert(y);
                    break;
                }
            }
        }
    }
    for (auto x : pending_rules) {
        for (auto y : x.second) {   // loop until find a symbol of which first set not includes '#'
            if (noend_sym_table.find(y) != noend_sym_table.end()) {
                if (!check_have(y, first)) construct_first(y);
                first[key].insert(first[y].begin(), first[y].end());
                if (!check_have('#', first[y]))
                    break;
            } else {
                first[key].insert(y);
                break;
            }
        }
    }
}

void construct_follow(char key) {
    if (check_have(key, follow)) return;
    if (key == S) follow[key].insert('#');
    for (auto x : rules) {
        size_t pos;
        if ((pos = x.second.find(key)) != string::npos) {
            if (pos == x.second.size() - 1) {   // if the key symbol is the last symbol of the rule
                if (!check_have(x.first, follow)) construct_follow(x.first);
                follow[key].insert(follow[x.first].begin(), follow[x.first].end());
                continue;
            }
            for (auto y : x.second.substr(pos+1)) { // loop until find a symbol of  which first set not includes '#'
                if (noend_sym_table.find(y) != noend_sym_table.end()) {
                    set<char> tmp = first[y];
                    tmp.erase('#');
                    follow[key].insert(tmp.begin(), tmp.end());
                    if (!check_have('#', first[y])) break;
                    if (y == x.second[x.second.size() - 1]) {   // check if the symbol is the last one of the rule
                        if (!check_have(x.first, follow)) construct_follow(x.first);
                        follow[key].insert(follow[x.first].begin(), follow[x.first].end()); // add its follow set to the follow set of the key
                    }
                } else {
                    follow[key].insert(y);
                    break;
                }
            }
        }
    }
}

void construct_lrpj() {
    lrpj.push_back({'S', "." + string(1, rules[0].first)});
    lrpj.push_back({'S', string(1, rules[0].first) + "."});
    for (auto x : rules) {
        char key = x.first;
        for (size_t i = 0; i <= x.second.length(); i++) {
            string val = x.second;
            if (val == "#") {
                lrpj.push_back({key, "."});
                continue;
            }
            val.insert(i, 1, '.');
            lrpj.push_back({key, val});
        }
    }
}

set<Rule> construct_closure(const set<Rule> &lrpj_set) {
    set<Rule> closure = lrpj_set;

    while (true) {
        set<Rule> new_pjs;
        size_t prev = closure.size();
        for (auto x : closure) {
            for (size_t dot_pos = x.second.find('.'); dot_pos != (x.second.size() - 1); dot_pos++) {
                char next_sym = x.second[dot_pos + 1];
                if (noend_sym_table.find(next_sym) != noend_sym_table.end()) {
                    for (auto y : lrpj) {
                        if (y.first == next_sym && y.second.find('.') == 0) {
                            Rule new_pj(y);
                            new_pjs.insert(new_pj);
                        }
                    }
                    if (first[next_sym].find('#') == first[next_sym].end())
                        break;
                } else
                    break;
            }
        }
        closure.insert(new_pjs.begin(), new_pjs.end());
        size_t curr = closure.size();
        if (prev == curr) break;
    }
    return closure;
}

set<Rule> construct_goto(const set<Rule> &lrpj_set, char sym) {
    set<Rule> goto_set;

    for (auto x : lrpj_set) {
        size_t dot_pos = x.second.find('.');
        if (dot_pos == (x.second.size() - 1)) continue;  // the dot is at the last position of the rule
        char next_sym = x.second[dot_pos + 1];
        if (next_sym == sym) {
            string tmp = x.second;
            tmp[dot_pos] = next_sym;    // change dot to the next position
            tmp[dot_pos+1] = '.';
            Rule new_rule{x.first, tmp};
            goto_set.insert(new_rule);
        }
    }
    return construct_closure(goto_set);
}

int get_rule_id(Rule obj) {   // get the id of the state, if the state not exsits, return -1
    for (size_t i = 0; i < rules.size(); i++) {
        if (rules[i] == obj) return i;
    }
    return -1;
}

int get_state_id(set<Rule> obj) {   // get the id of the state, if the state not exsits, return -1
    for (auto x : lrpj_sets) {
        if (x.srules == obj) return x.id;
    }
    return -1;
}

set<Rule> get_state_rule(int id) {
    for (auto x : lrpj_sets) {
        if (x.id == id) return x.srules;
    }
    return {};
}

void construct_lrdfa() {
    State start_state;
    start_state.srules = construct_closure({lrpj[0]});
    int id = 0;
    start_state.id = id++;
    lrpj_sets.push_back(start_state);

    while (true) {
        size_t prev = lrpj_sets.size();
        for (auto x : lrpj_sets) {
            for (auto y : end_sym_table) {
                set<Rule> goto_set = construct_goto(x.srules, y);
                if (!goto_set.empty() && (get_state_id(goto_set) == -1)) {
                    State new_sta;
                    new_sta.id = id++;
                    new_sta.srules = goto_set;
                    lrpj_sets.push_back(new_sta);
                }
            }
            for (auto y : noend_sym_table) {
                set<Rule> goto_set = construct_goto(x.srules, y);
                if (!goto_set.empty() && (get_state_id(goto_set) == -1)) {
                    State new_sta;
                    new_sta.id = id++;
                    new_sta.srules = goto_set;
                    lrpj_sets.push_back(new_sta);
                }
            }
        }
        size_t curr = lrpj_sets.size();
        if (prev == curr) break;    // the lr(0) project sets' size is not changing, stop processing
    }

    // construct action table
    for (auto x : lrpj_sets) {
        for (auto y : end_sym_table) {
            pair<int, char> p(x.id, y);
            set<Rule> goto_set = construct_goto(x.srules, y);
            if (!goto_set.empty()) {
                int next_id = get_state_id(goto_set);
                if (next_id == -1) continue;
                Action a(SHIFT, next_id);
                action_table[p] = a;
            }
            int cnt = 0;
            int cf = -1;
            for (auto z : x.srules) {
                if ((z.second.back() == '.')) {
                    cnt++;
                    if (check_shift_have(x.id) || cnt > 1) {
                        cf = 1;
                        break;
                    } else cf = 0;
                }
            }
            if (cf != -1) {
                Action a(REDUCE, cf);
                action_table[{x.id, '^'}] = a;
            }
        }
    }

    // construct goto table
    for (auto x : lrpj_sets) {
        for (auto y : noend_sym_table) {
            set<Rule> goto_set = construct_goto(x.srules, y);
            if (!goto_set.empty()) {
                int next_id = get_state_id(goto_set);
                if (next_id == -1) continue;
                pair<int, char> p(x.id, y);
                goto_table[p] = next_id;
            }
        }
    }
}

bool construct_slrat() {
    map<pair<int, char>, Action> conflict_table;  // separate all the reduce actions to the set
    map<pair<int, char>, Action> reduce_table;  // separate all the reduce actions to the set
    for (auto x : action_table) {
        if (x.second.type == REDUCE && x.second.next == 1)
            conflict_table.insert(x);    // initial conflict table
        else if (x.second.type == REDUCE && x.second.next == 0)
            reduce_table.insert(x);     // initial reduce table
    }
    map<pair<int, char>, Action> action_table_plus;
    for (auto x : conflict_table) {
        set<char> check_conflict;
        for (auto y : action_table) {
            if (y.first.first == x.first.first && y.second.type == SHIFT) check_conflict.insert(x.first.second);
        }
        auto tmp = get_state_rule(x.first.first);
        for (auto y : tmp) {
            size_t pos = y.second.find('.');
            if (pos == (y.second.size() - 1)) { // the dot is at the last position of the rule, trigger reducing
                set<char> left_follow = follow[y.first];
                if (check_itst(check_conflict, left_follow)) {
                    slr_error = "conflict follow symbol found!(REDUCE)";
                    // return false;
                } else {
                    check_conflict.insert(left_follow.begin(), left_follow.end());
                    Rule r = y;
                    r.second.erase(r.second.find('.'), 1);
                    if (r.second.empty()) r.second = "#";
                    int id = get_rule_id(r);
                    if (id == 0) {  // use the index 0 rule to reduce
                        action_table_plus.insert({{x.first.first, '#'}, {ACCEPT, id}});
                        continue;
                    }
                    for (auto y : left_follow) action_table_plus.insert({{x.first.first, y}, {REDUCE, id}});
                }
            }
        }
    }
    for (auto x : reduce_table) {
        auto tmp = get_state_rule(x.first.first);
        auto it = tmp.begin();
        Rule tmp_rule = *it;
        tmp_rule.second.erase(tmp_rule.second.find('.'), 1);
        if (tmp_rule.second.empty()) tmp_rule.second = "#";
        int id = get_rule_id(tmp_rule);
        if (id == 0) {  // use the index 0 rule to reduce
            action_table_plus.insert({{x.first.first, '#'}, {ACCEPT, id}});
            continue;
        }
        for (auto y : follow[tmp_rule.first])
            action_table_plus.insert({{x.first.first, y}, {REDUCE, id}});
    }
    action_table.insert(action_table_plus.begin(), action_table_plus.end());
    return true;

}

map<string, char> w2c;
map<char, string> c2w;
vector<string> rules_fixed;
unsigned char cur_adj_c = 128;
struct tree_node {
    int id;
    string value;
    vector<tree_node> children;
};

void print_tree(const vector<tree_node>& nodes, int depth = 0) {
    for (const auto& node : nodes) {
        for (int i = 0; i < depth; ++i) {
            cout << "  "; // 用于缩进显示层级关系
        }
        cout << "id: " << node.id << ", val: " << node.value << endl;
        print_tree(node.children, depth + 1); // 递归打印子节点
    }
}

void convert_to_json(const tree_node& node, std::ostream& out, int indent = 0) {
    out << std::string(indent, ' ') << "{\n";
    out << std::string(indent, ' ') << "  \"id\": " << node.id << ",\n";
    out << std::string(indent, ' ') << "  \"value\": \"" << node.value << "\",\n";

    out << std::string(indent, ' ') << "  \"children\": [\n";
    for (size_t i = 0; i < node.children.size(); ++i) {
        convert_to_json(node.children[i], out, indent + 4);
        if (i < node.children.size() - 1) {
            out << ",";
        }
        out << "\n";
    }
    out << std::string(indent, ' ') << "  ]\n";

    out << std::string(indent, ' ') << "}";
}

void analyze_code() {
    int id_cnt = 1;
    ifstream input_file("resources/ans.txt", ios::in);
    if (!input_file)
        exit(1);
    vector<pair<char, string>> input;
    string raw;
    while (getline(input_file, raw)) {
        string token, buf;
        istringstream iss(raw);
        iss >> token >> buf;
        input.push_back({w2c[token], buf});
    }
    input_file.close();
    input.push_back({'#', "\x00"});
    vector<int> states;
    vector<char> symbols;
    vector<tree_node> nodes;
    states.push_back(0);
    ofstream output_file("resources/analyse.txt", ios::out);
    output_file << "<table>" << endl;
    output_file << "<tr>" << endl;
    output_file << "<th>Step</th>" << endl;
    output_file << "<th>State Stack</th>" << endl;
    output_file << "<th>Symbol Stack</th>" << endl;
    output_file << "<th>Input (Token)</th>" << endl;
    output_file << "<th>Input (Buf)</th>" << endl;
    output_file << "<th>Action</th>" << endl;
    output_file << "</tr>" << endl;
    for (int i = 0; ; i++) {
        output_file << "<tr>" << endl;
        output_file << "<td>" << i << "</td>";
        output_file << "<td>";
        for (auto x : states) {
            output_file << x << " ";
        }
        output_file << "</td>";
        output_file << "<td>";
        for (auto x : symbols) {
            output_file << c2w[x];
            if (x != symbols[symbols.size() - 1])
                output_file << " ";
        }
        output_file << "</td>";
        output_file << "<td>";
        for (auto x : input) {
            output_file << c2w[x.first];
            if (x != input[input.size() - 1])
                output_file << " ";
        }
        output_file << "</td>";
        output_file << "<td>";
        for (auto x : input) {
            output_file << x.second;
            if (x != input[input.size() - 1])
                output_file << " ";
        }
        output_file << "</td>";
        int curr_state = states.back();
        char next_input = input[0].first;
        pair<int, char> p(curr_state, next_input);
        if (!check_action_have(p)) {
            output_file << "<td>ERROR</td>";
            output_file << "</tr>" << endl;
            break;
        }
        if (action_table[p].type == SHIFT) {
            tree_node buf{id_cnt++, input[0].second};
            tree_node token{id_cnt++, c2w[input[0].first], {buf}};
            nodes.push_back(token);
            input.erase(input.begin());
            int t = action_table[p].next;
            symbols.push_back(next_input);
            states.push_back(t);
            output_file << "<td>SHIFT s" << t << "</td>";
            output_file << "</tr>" << endl;
        } else if (action_table[p].type == REDUCE) {
            Rule rule = rules[action_table[p].next];
            vector<tree_node> tmp;
            for (size_t i = nodes.size() - rule.second.size(); i < nodes.size(); i++)
                tmp.push_back(nodes[i]);
            nodes.erase(nodes.end() - rule.second.size(), nodes.end());
            symbols.erase(symbols.end() - rule.second.size(), symbols.end());
            char key = rule.first;
            symbols.push_back(key);
            nodes.push_back({id_cnt++, c2w[key], tmp});
            states.erase(states.end() - rule.second.size(), states.end());
            curr_state = states.back();
            p = {curr_state, key};
            if (!check_goto_have(p)) {
                cerr << "ERROR" << endl;
                output_file << "<td>ERROR</td>";
                output_file << "</tr>" << endl;
                break;
            }
            states.push_back(goto_table[p]);
            output_file << "<td>REDUCE " << c2w[rule.first] << " -> ";
            for (auto x : rule.second) {
                output_file << c2w[x];
                if (x != rule.second[rule.second.size()-1])
                    output_file << " ";
            }
            output_file << "</td>";
            output_file << "</tr>" << endl;
        } else if (action_table[p].type == ACCEPT) {
            output_file << "<td>ACCEPT</td>";
            output_file << "</tr>" << endl;
            break;
        }
    }
    output_file << "<table>" << endl;
    output_file.close();
    ofstream json_file("resources/json.txt", ios::out);
    convert_to_json(nodes[0], json_file);
    json_file.close();
}

int main() {
    ifstream src_file("resources/rules.txt", ios::in);
    string rule_str;
    w2c["empty"] = '#';
    w2c["START"] = 'S';
    while (getline(src_file, rule_str)) {  // read rules from file
        string tmp;
        string rule_fixed;
        size_t start = rule_str.find("->") + 3;
        string left = rule_str.substr(0, start - 4);
        if (w2c.find(left) == w2c.end()) {
            w2c[left] = cur_adj_c;
            noend_sym_table.insert(cur_adj_c);
            rule_fixed = cur_adj_c++;
        } else {
            noend_sym_table.insert(w2c[left]);
            rule_fixed = w2c[left];
        }
        rule_fixed += "->";
        istringstream iss(rule_str.substr(start));
        while (iss >> tmp) {
            if (tmp == "|") {
                rule_fixed += "|";
                continue;
            }
            if (w2c.find(tmp) == w2c.end()) {
                w2c[tmp] = cur_adj_c;
                all_sym_table.insert(cur_adj_c);
                rule_fixed += cur_adj_c++;
            } else {
                all_sym_table.insert(w2c[tmp]);
                rule_fixed += w2c[tmp];
            }
        }
        rules_fixed.push_back(rule_fixed);
    }
    set_difference(all_sym_table.begin(), all_sym_table.end(), noend_sym_table.begin(), noend_sym_table.end(), inserter(end_sym_table, end_sym_table.begin()));
    for (const auto& pair : w2c)
        c2w[pair.second] = pair.first;
    src_file.close();
    for (auto rule_fixed : rules_fixed) {  // read rules from filu
        size_t start = 3, end = 0;
        noend_sym_table.insert(rule_fixed[0]);
        while ((end = rule_fixed.find('|', start)) != std::string::npos) {    // split rule strings with |
            Rule tmp(rule_fixed[0], rule_fixed.substr(start, end - start));
            rules.push_back(tmp);
            start = end + 1;
        }
        Rule tmp(rule_fixed[0], rule_fixed.substr(start));
        rules.push_back(tmp);
    }

    S = rules[0].first;

    for (auto x : rules) construct_first(x.first);
    for (auto x : rules) construct_follow(x.first);
    set<char> tmp;
    for (auto x : rules) tmp.insert(x.first);
    ofstream ff_file("resources/ff.txt", ios::out);
    ff_file << "<table>";
    ff_file << "<tr><td>Symbol</td><td>First Set</td><td>Follow Set</td></tr>";
    for (auto x : tmp) { // print the first set and follow set of each symbol into the file "ff.txt"
        ff_file << "<tr><td>" << c2w[x] << "</td><td>{";
        for (auto it = first[x].begin(); it != first[x].end(); it++) {
            ff_file << " " << c2w[*it];
            if (next(it) != first[x].end()) ff_file << ",";
        }
        ff_file << " }</td><td>{";
        for (auto it = follow[x].begin(); it != follow[x].end(); it++) {
            ff_file << " " << c2w[*it];
            if (next(it) != follow[x].end()) ff_file << ",";
        }
        ff_file << " }</td></tr>" << endl;
    }
    ff_file.close();

    end_sym_table.erase('#');   // ignore '#' when constructing DFA
    construct_lrpj();

    construct_lrdfa();
    ofstream lr_file("resources/lrdfa.txt", ios::out);  // write HTML format DFA to the file
    lr_file << "<table>" << endl;
    lr_file << "<tr>" << endl;
    lr_file << "<th rowspan=\"2\">State</th>" << endl;
    lr_file << "<th rowspan=\"2\">Action</th>" << endl;
    lr_file << "<th rowspan=\"2\">Rule</th>" << endl;
    lr_file << "<th colspan=\"" << end_sym_table
    .size() << "\">Input</th>" << endl;
    lr_file << "<th colspan=\"" << noend_sym_table.size() << "\">Goto</th>" << endl;
    lr_file << "<th rowspan=\"2\">Content</th>" << endl;
    lr_file << "</tr>" << endl;
    lr_file << "<tr>" << endl;
    for (auto x : end_sym_table)
        lr_file << "<td>" << c2w[x] << "</td>" << endl;
    for (auto x : noend_sym_table)
        lr_file << "<td>" << c2w[x] << "</td>" << endl;
    lr_file << "</tr>" << endl;
    for (size_t i = 0; i < lrpj_sets.size(); i++) {
        lr_file << "<tr>" << endl;
        lr_file << "<td>" << i << "</td>" << endl;
        string as;
        pair<int, char> p(i, '^');
        if (action_table[p].next == -1)
            lr_file << "<td>SHIFT</td>" << endl;
        else if (action_table[p].next == 0)
            lr_file << "<td>REDUCE</td>" << endl;
        else if (action_table[p].next == 1)
            lr_file << "<td>CONFLICT</td>" << endl;
        set<Rule> tmp_rule = get_state_rule(i);
        lr_file << "<td>" << endl;
        for (auto x : tmp_rule) {
            string tmp = x.second;
            if (tmp.back() == '.') {
                tmp.erase(tmp.find('.'), 1);
                lr_file << c2w[x.first] << " -> ";
                for (auto xx : tmp) {
                    if (xx == '.')
                        lr_file << xx;
                    else {
                        if (xx != tmp[tmp.size()-1])
                            lr_file << c2w[xx] << " ";
                        else
                            lr_file << c2w[xx];
                    }
                }
                lr_file << "<br>";
            }
        }
        lr_file << endl << "</td>" << endl;
        for (auto x : end_sym_table) {
            if (check_action_have({i, x})) {
                lr_file << "<td>" << action_table[{i, x}].next << "</td>" << endl;
            }
            else
                lr_file << "<td></td>" << endl;
        }
        for (auto x : noend_sym_table) {
            if (check_goto_have({i, x}))
                lr_file << "<td>" << goto_table[{i, x}] << "</td>" << endl;
            else
                lr_file << "<td></td>" << endl;
        }
        set<Rule> tmp = get_state_rule(i);
        lr_file << "<td>" << endl;
        for (auto x : tmp) {
            lr_file << c2w[x.first] << " -> ";
            for (auto xx : x.second) {
                if (xx != x.second[x.second.size()-1])
                    lr_file << c2w[xx] << " ";
                else
                    lr_file << c2w[xx];
            }
            lr_file << "<br>";
        }
        lr_file << endl << "</td>" << endl << "</tr>" << endl;
    }
    lr_file << "</table>" << endl;
    lr_file.close();

    rules.insert(rules.begin(), {'S', string(1, S)});   // expand the rules
    ofstream rule_file("resources/new_rules.txt", ios::out);  // write new rules to the file
    for (size_t i = 0; i < rules.size(); i++) {
        rule_file << i << ": " << c2w[rules[i].first] << " -> ";
        for (auto x : rules[i].second) {
            if (x != rules[i].second[rules[i].second.size()-1])
                rule_file << c2w[x] << " ";
            else
                rule_file << c2w[x];
        }
        rule_file << endl;
    }
    rule_file.close();
    S = 'S';
    construct_first(S);
    construct_follow(S);

    ofstream slr_file("resources/slrat.txt", ios::out);  // write HTML format AT to the file
    if (!construct_slrat()) {
        slr_file << slr_error << endl;
        slr_file.close();
        return 0;
    }
    end_sym_table.insert('#');
    slr_file << "<table>" << endl;
    slr_file << "<tr>" << endl;
    slr_file << "<th rowspan=\"2\">State</th>" << endl;
    slr_file << "<th colspan=\"" << end_sym_table
    .size() << "\">Input</th>" << endl;
    slr_file << "<th colspan=\"" << noend_sym_table.size() << "\">Goto</th>" << endl;
    slr_file << "<th rowspan=\"2\">Content</th>" << endl;
    slr_file << "</tr>" << endl;
    slr_file << "<tr>" << endl;
    for (auto x : end_sym_table)
        slr_file << "<td>" << c2w[x] << "</td>" << endl;
    for (auto x : noend_sym_table)
        slr_file << "<td>" << c2w[x] << "</td>" << endl;
    slr_file << "</tr>" << endl;
    for (size_t i = 0; i < lrpj_sets.size(); i++) {
        slr_file << "<tr>" << endl;
        slr_file << "<td>" << i << "</td>" << endl;
        for (auto x : end_sym_table) {
            if (check_action_have({i, x})) {
                string a;
                if (action_table[{i, x}].type == SHIFT)
                    slr_file << "<td>" << "s" << action_table[{i, x}].next << "</td>" << endl;
                else if
                    (action_table[{i, x}].type == REDUCE) slr_file << "<td>" << "r" << action_table[{i, x}].next << "</td>" << endl;
                else if
                    (action_table[{i, x}].type == ACCEPT) slr_file << "<td>" << "acc" << "</td>" << endl;
            } else slr_file << "<td></td>" << endl;
        }
        for (auto x : noend_sym_table) {
            if (check_goto_have({i, x}))
                slr_file << "<td>" << goto_table[{i, x}] << "</td>" << endl;
            else slr_file << "<td></td>" << endl;
        }
        set<Rule> tmp = get_state_rule(i);
        slr_file << "<td>" << endl;
        for (auto x : tmp) {
            slr_file << c2w[x.first] << " -> ";
            for (auto xx : x.second) {
                if (xx == '.')
                    slr_file << xx;
                else {
                    if (xx != x.second[x.second.size()-1])
                        slr_file << c2w[xx] << " ";
                    else
                        slr_file << c2w[xx];
                }
            }
            slr_file << "<br>";
        }
        slr_file << endl << "</td>" << endl << "</tr>" << endl;
    }
    slr_file << "</table>" << endl;
    slr_file.close();

    analyze_code();
}
