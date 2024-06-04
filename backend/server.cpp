#include <fstream>
#include "httplib.h"
using namespace std;

void init_header(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET,PUT,POST,DELETE");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
}

void handle_upload_reg(const httplib::Request& req, httplib::Response& res) {
    init_header(res);
    if (!req.has_file("reg")) {
        res.status = 400;
        res.set_content("No file uploaded", "text/plain");
        return;
    }

    const auto& file = req.get_file_value("reg");
    std::ofstream ofs("resources/reg.txt");

    if (!ofs) {
        res.status = 500;
        res.set_content("Failed to save file", "text/plain");
        return;
    }

    size_t pos = 0;
    string content = file.content;
    while ((pos = content.find("\r\n", pos)) != string::npos) {
        content.replace(pos, 2, "\n");
        pos += 1; // Move past the replaced '\n'
    }

    ofs << content;
    ofs.close();

    res.set_content("File uploaded successfully", "text/plain");
}

void handle_upload_src(const httplib::Request& req, httplib::Response& res) {
    init_header(res);
    if (!req.has_file("src")) {
        res.status = 400;
        res.set_content("No file uploaded", "text/plain");
        return;
    }

    const auto& file = req.get_file_value("src");
    std::ofstream ofs("resources/source.txt", ios::out);

    if (!ofs) {
        res.status = 500;
        res.set_content("Failed to save data", "text/plain");
        return;
    }

    size_t pos = 0;
    string content = file.content;
    while ((pos = content.find("\r\n", pos)) != string::npos) {
        content.replace(pos, 2, "\n");
        pos += 1; // Move past the replaced '\n'
    }

    ofs << content;
    ofs << '\n';
    ofs.close();

    res.set_content("Data save successfully", "text/plain");
}

void handle_lab1(const httplib::Request& req, httplib::Response& res) {
    init_header(res);

    system(".\\resources\\a.exe");
}

void nfa(const httplib::Request& req, httplib::Response& res) {
    init_header(res);

    ifstream nfa_file("resources/nfa.txt", ios::in);
    istreambuf_iterator<char> begin(nfa_file);
    istreambuf_iterator<char> end;
    string ans(begin, end);
    nfa_file.close();

    res.set_content(ans, "text/plain");
}

void dfa(const httplib::Request& req, httplib::Response& res) {
    init_header(res);

    ifstream dfa_file("resources/dfa.txt", ios::in);
    istreambuf_iterator<char> begin(dfa_file);
    istreambuf_iterator<char> end;
    string ans(begin, end);
    dfa_file.close();

    res.set_content(ans, "text/plain");
}

void mindfa(const httplib::Request& req, httplib::Response& res) {
    init_header(res);

    ifstream mindfa_file("resources/min.txt", ios::in);
    istreambuf_iterator<char> begin(mindfa_file);
    istreambuf_iterator<char> end;
    string ans(begin, end);
    mindfa_file.close();

    res.set_content(ans, "text/plain");
}

void gen_code(const httplib::Request& req, httplib::Response& res) {
    init_header(res);

    ifstream code_file("resources/code.cpp", ios::in);
    istreambuf_iterator<char> begin(code_file);
    istreambuf_iterator<char> end;
    string ans(begin, end);
    code_file.close();

    res.set_content(ans, "text/plain");
}


void test_code(const httplib::Request& req, httplib::Response& res) {
    init_header(res);

    system(".\\resources\\gcc\\bin\\g++.exe -o .\\resources\\test.exe .\\resources\\code.cpp");
    system(".\\resources\\test.exe");

    ifstream code_file("resources/ans.txt", ios::in);
    istreambuf_iterator<char> begin(code_file);
    istreambuf_iterator<char> end;
    string ans(begin, end);
    code_file.close();

    res.set_content(ans, "text/plain");
}

void handle_upload_rules(const httplib::Request& req, httplib::Response& res) {
    init_header(res);
    if (!req.has_file("rules")) {
        res.status = 400;
        res.set_content("No file uploaded", "text/plain");
        return;
    }

    const auto& file = req.get_file_value("rules");
    std::ofstream ofs("resources/rules.txt", ios::out);

    if (!ofs) {
        res.status = 500;
        res.set_content("Failed to save data", "text/plain");
        return;
    }

    size_t pos = 0;
    string content = file.content;
    while ((pos = content.find("\r\n", pos)) != string::npos) {
        content.replace(pos, 2, "\n");
        pos += 1; // Move past the replaced '\n'
    }

    ofs << content;
    ofs.close();

    res.set_content("Data save successfully", "text/plain");
}

void handle_lab2(const httplib::Request& req, httplib::Response& res) {
    init_header(res);

    system(".\\resources\\b.exe");
    ifstream rule_file("resources/new_rules.txt", ios::in);
    istreambuf_iterator<char> begin(rule_file);
    istreambuf_iterator<char> end;
    string ans(begin, end);
    rule_file.close();

    res.set_content(ans, "text/plain");
}

void ff(const httplib::Request& req, httplib::Response& res) {
    init_header(res);

    ifstream ff_file("resources/ff.txt", ios::in);
    istreambuf_iterator<char> begin(ff_file);
    istreambuf_iterator<char> end;
    string ans(begin, end);
    ff_file.close();

    res.set_content(ans, "text/plain");
}

void lr(const httplib::Request& req, httplib::Response& res) {
    init_header(res);

    ifstream lr_file("resources/lrdfa.txt", ios::in);
    istreambuf_iterator<char> begin(lr_file);
    istreambuf_iterator<char> end;
    string ans(begin, end);
    lr_file.close();

    res.set_content(ans, "text/plain");
}

void slr(const httplib::Request& req, httplib::Response& res) {
    init_header(res);

    ifstream slr_file("resources/slrat.txt", ios::in);
    istreambuf_iterator<char> begin(slr_file);
    istreambuf_iterator<char> end;
    string ans(begin, end);
    slr_file.close();

    res.set_content(ans, "text/plain");
}

void analyse_code(const httplib::Request& req, httplib::Response& res) {
    init_header(res);

    ifstream log_file("resources/analyse.txt", ios::in);
    istreambuf_iterator<char> begin(log_file);
    istreambuf_iterator<char> end;
    string ans(begin, end);
    log_file.close();

    res.set_content(ans, "text/plain");
}

void gen_tree(const httplib::Request& req, httplib::Response& res) {
    init_header(res);

    ifstream tree_file("resources/json.txt", ios::in);
    istreambuf_iterator<char> begin(tree_file);
    istreambuf_iterator<char> end;
    string ans(begin, end);
    tree_file.close();

    res.set_content(ans, "text/plain");
}

int main() {
    httplib::Server svr;

    // task1
    svr.Post("/upload_reg", handle_upload_reg);
    svr.Post("/upload_src", handle_upload_src);
    svr.Post("/lab1", handle_lab1);
    svr.Post("/dfa", dfa);
    svr.Post("/nfa", nfa);
    svr.Post("/min", mindfa);
    svr.Post("/gen_code", gen_code);
    svr.Post("/test_code", test_code);

    // task2
    svr.Post("/upload_bnf", handle_upload_rules);
    // svr.Post("/save_sentence", save_sentence);
    svr.Post("/lab2", handle_lab2);
    svr.Post("/ff", ff);
    svr.Post("/lr", lr);
    svr.Post("/slr", slr);
    svr.Post("/analyse_code", analyse_code);
    svr.Post("/gen_tree", gen_tree);

    svr.listen("localhost", 8000);

    return 0;
}
