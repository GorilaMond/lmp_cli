#include <iostream>
#include <stdlib.h>
#include "clipp.h"

using namespace clipp;
using std::cout;
using std::string;

const string ECLI = "sudo EUNOMIA_REPOSITORY=https://linuxk"
                    "erneltravel.github.io/lmp/ EUNOMIA_HOM"
                    "E=/home/ubuntu/.lmp/ ecli ";
const string DOCKER = "docker run -it -v `pwd`/:/src/ yunwe"
                    "i37/ebpm:latest ";

int main(int argc, char *argv[]) {
    enum class mode {run, pull, init, build, gen_wasm_skel, build_wasm, help};
    mode selected = mode::help;
    std::vector<string> input;
    string dict, out;
    auto runMode = (
        command("run").set(selected, mode::run),
        values("file", input)
    ) % "使用一个名字直接跑起来，如果本地没有，就去网上对应的repo下载\n"
        "``lmp run opensnoop``\n"
        "使用一个名字和版本号\n"
        "``lmp run opensnoop:latest``\n"
        "使用一个http API\n"
        "``lmp run https://github.com/ebpf-io/raw/master/examples"
        "/opensnoop/app.wasm``\n"
        "使用一个本地路径\n"
        "``lmp run ./opensnoop/package.json``\n"
        "带参数\n"
        "``lmp run sigsnoop -h``\n";
    auto pullMode = (
        command("pull").set(selected, mode::pull),
        value("file_path", dict)
    ) % "去网上下载, run 命令实际上包含了 pull 命令\n"
        "``lmp pull opensnoop``";
    auto initMode = (
        command("init").set(selected, mode::init),
        value("project_name", dict)
    ) % "生成一个项目模板, 会创建一个新的<project_name>目录, 生成如下一些文"
        "件: .bpf.c .bpf.h README.md config.json .gitignore\n"
        "``lmp init opensnoop``";
    auto buildMode = (
        command("build").set(selected, mode::build)
    ) % "构建内核态程序, package.json 是编译生成的产物";
    auto gen_wasm_skelMode = (
        command("gen-wasm-skel").set(selected, mode::gen_wasm_skel)
    ) % "生成 wasm 用户态项目模板";
    auto build_wasmMode = (
        command("build-wasm").set(selected, mode::build_wasm)
    ) % "构建 wasm 用户态项目";
    auto cli = (
        (runMode | pullMode | initMode | buildMode |
        gen_wasm_skelMode | build_wasmMode | 
        command("help").set(selected,mode::help)),
        option("-v", "--version").call([]{cout << "version 1.0\n\n";}).doc("show version")
    );
    if(parse(argc, argv, cli)) {
        switch (selected)
        {
            case mode::run:
                out = string();
                for(auto t = input.begin(); t != input.end(); t++)
                    out += *t + " ";
                system((ECLI + "run " + out).c_str());
                break;
            case mode::pull:
                system((ECLI + "pull " + dict).c_str());
                break;
            case mode::init:
                system(("git clone https://github.com/eunomia-bp"
                    "f/ebpm-template && mv ebpm-template " + dict).c_str());
                break;
            case mode::build:
                system(DOCKER.c_str());
                break;
            case mode::gen_wasm_skel:
                system((DOCKER + "gen-wasm-skel").c_str());
                break;
            case mode::build_wasm:
                system((DOCKER + "build-wasm").c_str());
                break;
            default:
                cout << make_man_page(cli, argv[0]) << '\n';
                break;
        }
    } else
        cout << usage_lines(cli, "lmp") << '\n';
    return 0;
}
