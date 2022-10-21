# 使用lmp-cli开启eBPF之旅

##### **eBPF是什么?**

eBPF是扩展伯克利包过滤器（extended Berkeley Packet Filter）的缩写。Linux内核已经发了很长一段时间，但是，修改或扩展内核并不容易，除非您知道如何给内核打补丁。如果您熟悉Kubernetes的自定义资源或Envoy过滤器，您就会理解基于特定场景构建扩展是多么重要。ebpf为Linux内核提供了可扩展性，使开发人员能够对Linux内核进行编程，以便根据他们的业务需求快速构建智能的或丰富的功能。ebpf程序对Linux内核的作用类似于web组装模块对Envoy的作用。它们允许开发人员轻松地扩展内核，并在内核中以沙箱程序的形式运行他们的ebpf代码，而无需更改内核源代码或加载内核模块。

##### **用于构建您的ebpf程序的选项**

在编译构建第一个eBPF程序时的几个选择。

**BPF 编译器集合 (BCC)**

BCC是一个用于创建高效内核跟踪和操作ebpf程序的工具包，需要Linux 4.1及以上的版本。虽然BCC的设计是为了使BPF程序更容易编写，但它需要用C编写内核插装，将其包装为普通字符串，放入Python或Lua的用户空间程序中。如果您在任何eBPF python示例中同时看到C代码和python代码，请不要感到惊讶。如果您不是像我这样的Linux用户，那么很难找到一个可以编译和测试第一个ebpf程序的环境。

在编译并运行了第一个ebpf程序之后，您可能还想知道它的可移植性如何。您可以将编译后的二进制文件运行在4.1以上的任何内核上吗？还是需要为想要运行的每个内核版本编译它？当部署并执行eBPF程序时，BCC调用它的嵌入式Clang/LLVM，提取本地内核头文件(必须确保从正确的kernel-devel包中安装到系统上)，并在Linux内核上动态地编译纯字符串。虽然这种方法将BPF代码定制为特定的内核，但它可能有一些主要的缺点，例如随应用程序分发的库很笨重、资源利用率很高、需要内核头文件等等。但是，有其他的办法可以解决这些问题。

###### 进入**BPF CO-RE和libbpf**

BPF CO-RE (Compile Once Run Everywhere)旨在解决上述可移植性问题，您只需要编译一次，而不需要为每个内核版本编译它。libbpf是一组用于构建BPF应用程序的可选工具，它是作为BPF程序加载器引入的，它会根据主机的特定内核调整BPF程序代码。它解析和匹配所有BTF（BPF类型格式）类型和字段，根据需要更新必要的偏移量和其他可重定位数据，以确保BPF程序的逻辑在主机上的特定内核中正确运行。如果一切正常，您将为目标主机上内核生成一个BPF程序，就像您的程序是专门为它编译的一样。

在使用libbpf的过程中，您可以借助辅助宏来编写纯C代码，以消除繁琐的部分。真正巧妙的是，写的就是执行的，不再需要把BPF代码包装成Python或Lua中的普通字符串。这种方法将开销降至最低，消除了严重的依赖性，并使BPF更加实用。

**使用eunomia-bpf简化eBPF开发**

eunomia-bpf 是一个 eBPF 程序的轻量级开发加载框架，包含了一个用户态动态加载框架/运行时库，以及一个简单的编译 WASM 和 eBPF 字节码的工具链容器。大致来说，我们在 WASM 运行时和用户态的 libbpf 中间多加了一层抽象层（eunomia-bpf 库），使得一次编译、到处运行的 eBPF 代码可以从 JSON 对象中动态加载。JSON 对象会在编译时被包含在 WASM 模块中，因此在运行时，我们可以通过解析 JSON 对象来获取 eBPF 程序的信息，然后动态加载 eBPF 程序。通过 WASM module 打包和分发 eBPF 字节码，同时在 WASM 虚拟机内部控制整个 eBPF 程序的加载和执行，我们就可以将二者的优势结合起来，让任意 eBPF 程序能有如下特性：

* **可移植：**让 eBPF 工具和应用完全平台无关、可移植，不需要进行重新编译即可以跨平台分发。
* **隔离性：**借助 WASM 的可靠性和隔离性，让 eBPF 程序的加载和执行、以及用户态的数据处理流程更加安全可靠。
* **包管理：**借助 WASM 的生态和工具链，完成 eBPF 程序或工具的分发、管理、加载等工作，目前 eBPF 程序或工具生态可能缺乏一个通用的包管理或插件管理系统。
* **敏捷性：**对于大型的 eBPF 应用程序，可以使用 WASM 作为插件扩展平台：扩展程序可以在运行时直接从控制平面交付和重新加载。这不仅意味着每个人都可以使用官方和未经修改的应用程序来加载自定义扩展，而且任何 eBPF 程序的错误修复和/或更新都可以在运行时推送和/或测试，而不需要更新和/或重新部署一个新的二进制。
* **轻量级：**WebAssembly 微服务消耗 1% 的资源，与 Linux 容器应用相比，冷启动的时间是 1%：我们也许可以借此实现 eBPF as a service，让 eBPF 程序的加载和执行变得更加轻量级、快速、简便易行。

，包含了一个小型的 WASM 运行时模块和 eBPF 动态装载的功能

##### **使用lmp-cli开启eBPF之旅**

让我们开始通过lmp-cli创建、运行或下载一个简单的程序。在这里，我们使用基于eunomia-bpf库的一个简单的命令行工具lmp，概述如何从四个步骤开始运行。

###### **1. 准备你的环境**

ebpf本身是一种Linux内核技术，因此任何实际的BPF程序都必须在Linux内核中运行。我建议您从内核5.4或更新的版本开始。从SSH终端，检查内核版本，并确认您已经启用了CONFIG_DEBUG_INFO_BTF：

```bash
uname -r
cat /boot/config-$(uname -r) | grep CONFIG_DEBUG_INFO_BTF
```

你会看到类似这样的输出：

```bash
a@a-virtual-machine:~$ uname -r
5.15.0-48-generic
a@a-virtual-machine:~$ cat /boot/config-$(uname -r) | grep CONFIG_DEBUG_INFO_BTF
CONFIG_DEBUG_INFO_BTF=y
CONFIG_DEBUG_INFO_BTF_MODULES=y
```

安装lmp命令行：

```bash
wget https://github.com/GorilaMond/lmp_cli/releases/download/lmp/install.sh && sudo sh ./install.sh
```

###### **2. 构建程序**

使用`lmp init`创建你的第一个ebpf项目，接受所有默认选项以保持简单：

```bash
lmp init hello
```

成功创建内核项目后，您将看到如下类似的输出：

```bash
a@a-virtual-machine:~$ lmp init hello
Cloning into 'ebpm-template'...
remote: Enumerating objects: 99, done.
remote: Counting objects: 100% (99/99), done.
remote: Compressing objects: 100% (61/61), done.
remote: Total 99 (delta 36), reused 45 (delta 15), pack-reused 0
Receiving objects: 100% (99/99), 15.37 KiB | 925.00 KiB/s, done.
Resolving deltas: 100% (36/36), done.
a@a-virtual-machine:~$
a@a-virtual-machine:~$ cd hello/
a@a-virtual-machine:~/hello$ ll
total 36
drwxrwxr-x  4 a a 4096 10月 17 23:18 ./
drwxr-x--- 17 a a 4096 10月 17 23:18 ../
-rw-rw-r--  1 a a 2910 10月 17 23:18 bootstrap.bpf.c
-rw-rw-r--  1 a a  392 10月 17 23:18 bootstrap.bpf.h
-rw-rw-r--  1 a a  221 10月 17 23:18 config.json
drwxrwxr-x  8 a a 4096 10月 17 23:18 .git/
drwxrwxr-x  3 a a 4096 10月 17 23:18 .github/
-rw-rw-r--  1 a a   21 10月 17 23:18 .gitignore
-rw-rw-r--  1 a a 2400 10月 17 23:18 README.md

```

模板中默认的跟踪点为`"tp/sched/sched_process_exec"`和`"tp/sched/sched_process_exit"`，用来跟踪新程序的执行和退出，这里不做修改。

构建内核项目，如下所示。保存您的更改，并使用`sudo lmp build`构建内核程序，以创建一个名为package.json的对象。

```shell
a@a-virtual-machine:~/hello$ sudo lmp build
[sudo] password for a: 
make
  BPF      .output/client.bpf.o
  GEN-SKEL .output/client.skel.h
  CC       .output/client.o
  CC       .output/cJSON.o
  CC       .output/create_skel_json.o
  BINARY   client
  DUMP_LLVM_MEMORY_LAYOUT 
  DUMP_EBPF_PROGRAM 
  FIX_TYPE_INFO_IN_EBPF 
  GENERATE_PACKAGE_JSON 
```

###### **3. 运行**内核程序

使用`lmp run package.json`运行内核程序，没有用户端程序对数据的处理，该框架下内核程序将会输出所有被output的数据：

```shell
a@a-virtual-machine:~/hello$ sudo lmp run ./package.json 
running and waiting for the ebpf events from ring buffer...
time pid ppid exit_code duration_ns comm filename exit_event 
```

一开始您不会看到任何数据，只有当内核的跟踪点被触发时，这里是新的进程被创建或退出时，才会输出数据。这里新建了一个虚拟终端，输出了如下数据：

```bash
23:31:31 111788 109955 0 0 bash /bin/bash 0 
23:31:31 111790 111788 0 0 lesspipe /usr/bin/lesspipe 0 
23:31:31 111791 111790 0 0 basename /usr/bin/basename 0 
23:31:31 111791 111790 0 14829468 basename  1 
23:31:31 111793 111792 0 0 dirname /usr/bin/dirname 0 
23:31:31 111793 111792 0 8045108 dirname  1 
23:31:31 111792 111790 0 0 lesspipe  1 
23:31:31 111790 111788 0 46731288 lesspipe  1 
23:31:31 111794 111788 0 0 dircolors /usr/bin/dircolors 0 
23:31:31 111794 111788 0 10118087 dircolors  1
```

###### 4. 添加用户态程序

在构建好的内核项目文件夹内，使用`sudo lmp gen-wasm-skel`生成一个wasm用户态项目模板，app.c、eunomia-include、ewasm-skel.h 这些文件会被生成。ewasm-skel.h是被打包为头文件的内核程序，app.c是用户态程序的模板文件，我们可以修改它来进行自定义的数据处理，这里不做修改。

```bash
a@a-virtual-machine:~/hello$ sudo lmp gen-wasm-skel
[sudo] password for a: 
make
  BPF      .output/client.bpf.o
  GEN-SKEL .output/client.skel.h
  CC       .output/client.o
  CC       .output/cJSON.o
  CC       .output/create_skel_json.o
  BINARY   client
  DUMP_LLVM_MEMORY_LAYOUT 
  DUMP_EBPF_PROGRAM 
  FIX_TYPE_INFO_IN_EBPF 
  GENERATE_PACKAGE_JSON 
  GEN-WASM-SKEL 
```

使用`sudo lmp build-wasm`构建用户态程序，生成app.wasm文件

```bash
a@a-virtual-machine:~/hello$ sudo lmp build-wasm
make
  BPF      .output/client.bpf.o
  GEN-SKEL .output/client.skel.h
  CC       .output/client.o
  CC       .output/cJSON.o
  CC       .output/create_skel_json.o
  BINARY   client
  DUMP_LLVM_MEMORY_LAYOUT 
  DUMP_EBPF_PROGRAM 
  FIX_TYPE_INFO_IN_EBPF 
  GENERATE_PACKAGE_JSON 
  BUILD-WASM 
cd ../build-wasm && ./build.sh
build app.wasm success
ls: cannot access '*.cpp': No such file or directory
```

使用`lmp run app.wasm`运行用户态程序：

```bash
a@a-virtual-machine:~/hello$ lmp run app.wasm 
running and waiting for the ebpf events from ring buffer...
{"pid":112665,"ppid":109955,"exit_code":0,"duration_ns":0,"comm":"bash","filename":"/bin/bash","exit_event":0}
{"pid":112667,"ppid":112665,"exit_code":0,"duration_ns":0,"comm":"lesspipe","filename":"/usr/bin/lesspipe","exit_event":0}
{"pid":112668,"ppid":112667,"exit_code":0,"duration_ns":0,"comm":"basename","filename":"/usr/bin/basename","exit_event":0}
{"pid":112668,"ppid":112667,"exit_code":0,"duration_ns":19701623,"comm":"basename","filename":"","exit_event":1}
{"pid":112672,"ppid":112669,"exit_code":0,"duration_ns":0,"comm":"dirname","filename":"/usr/bin/dirname","exit_event":0}
{"pid":112672,"ppid":112669,"exit_code":0,"duration_ns":5966058,"comm":"dirname","filename":"","exit_event":1}
{"pid":112669,"ppid":112667,"exit_code":0,"duration_ns":0,"comm":"lesspipe","filename":"","exit_event":1}
{"pid":112667,"ppid":112665,"exit_code":0,"duration_ns":39178441,"comm":"lesspipe","filename":"","exit_event":1}
{"pid":112673,"ppid":112665,"exit_code":0,"duration_ns":0,"comm":"dircolors","filename":"/usr/bin/dircolors","exit_event":0}
{"pid":112673,"ppid":112665,"exit_code":0,"duration_ns":8306920,"comm":"dircolors","filename":"","exit_event":1}

```

###### 更多应用

您可以无需任何编译，使用`lmp run <name>`直接运行lmp仓库的小程序，在本地找不到文件时，其中包括了`lmp pull <name>`命令：

```bash
a@a-virtual-machine:~/hello$ lmp run sigsnoop
[sudo] password for a: 
download with curl: https://linuxkerneltravel.github.io/lmp/sigsnoop/app.wasm
curl: (22) The requested URL returned error: 404
download with curl: https://linuxkerneltravel.github.io/lmp/sigsnoop/package.json
running and waiting for the ebpf events from perf event...
time pid tpid sig ret comm 
00:21:41 109955 112863 28 0 gnome-terminal- 
00:21:41 109955 112862 28 0 gnome-terminal- 
00:21:41 109955 112861 28 0 gnome-terminal- 
00:21:41 112864 112865 28 0 sudo 
00:21:41 112864 112865 28 0 sudo 
00:21:41 112864 -112865 28 0 sudo 
00:21:41 109955 112863 28 0 gnome-terminal- 
00:21:41 109955 112862 28 0 gnome-terminal- 
00:21:41 109955 112861 28 0 gnome-terminal- 
00:21:41 112864 112865 28 0 sudo 
00:21:41 112864 112865 28 0 sudo 
00:21:41 112864 -112865 28 0 sudo 
```

##### **关于ebpf和lmp-cli的讨论结束了**

通过这些简单的lmp-cli初始化、构建、运行、列出和推送命令，我们很高兴将类似docker的体验引入到eBPF中，这样开发人员不仅可以轻松地构建eBPF程序，还可以与他人共享他们的eBPF程序。

要参与eBPF和eunomia-bpf，请查看开源的eunomia-bpf存储库，加入社区lmp的讨论或注册即将到来的活动。

##### 推荐

[eunomia-bpf: 一个ebpf程序动态加载框架](https://github.com/eunomia-bpf/eunomia-bpf)

[LMP project: Linux 显微镜](https://github.com/linuxkerneltravel/lmp)

[当 WASM 遇见 eBPF：使用 WebAssembly 编写、分发、加载运行 eBPF 程序 | 龙蜥技术](https://mp.weixin.qq.com/s/ryT7OqWngCjcCkfeSKjutA)
