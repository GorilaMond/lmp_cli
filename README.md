# lmp-startup

A small program that can make the use of LMP more convenient.

## Make

If you have cloned the repo and entered the lmp_cli directory, you can run `make` to build the program.Then, there is a executable file named lmp, you can use it by pointing it's path.
After `make`, you could run `make install` to copy it to `/usr/bin`, so that you can run lmp any path.
You can run `make uninstall` to delete it in `/usr/bin`, and run `make clean` to delete it in lmp_cli directory.

## Dependency

You can run commands below to install these dependencies.
```bash
if [ -n "$(uname -i | grep x86)" ]; then
	wget https://aka.pw/bpf-ecli -O ecli && chmod +x ecli && sudo mv ecil /usr/bin
else
	echo "Only support x86 plateform now."
fi
if [ -z "$(docker -v | grep version)" ]; then
	curl -fsSL https://get.docker.com | bash -s docker --mirror Aliyun
fi
```

ecli
https://github.com/eunomia-bpf/eunomia-bpf/tree/master/ecli#readme
```bash
# download the release from https://github.com/eunomia-bpf/eunomia-bpf/releases/latest/download/ecli
wget https://aka.pw/bpf-ecli -O ecli && chmod +x ecli && sudo mv ecil /usr/bin
```
docker
```bash
curl -fsSL https://get.docker.com | bash -s docker --mirror Aliyun
```

## Details

This is a LMP command line initiator to invoke ecli commands:

### User

First, we have a use case for developers who want to use an ebpf binary or program but doesn't know how/where to find it:
Run directly.

```bash
# Use a name and run it directly. If it's not available locally, download it from the corresponding repo on the web
$ lmp run opensnoop
...

# Use a name and version number
$ lmp run opensnoop:latest
...

# Use an http API
$ lmp run https://github.com/ebpf-io/raw/master/examples/opensnoop/app.wasm
...

# Use a local path
$ lmp run ./opensnoop/package.json
...

# With parameters
$ lmp run sigsnoop -h
Usage: sigsnoop [-h] [-x] [-k] [-n] [-p PID] [-s SIGNAL]
Trace standard and real-time signals.


    -h, --help  show this help message and exit
    -x, --failed  failed signals only
    -k, --killed  kill only
    -p, --pid=<int>  target pid
    -s, --signal=<int>  target signal
```

In fact, the run command contains the pull command. If the local ebpf file is not available, it will be downloaded from the Internet. If the local EBPF file is available, it will be directly used:

```bash
$ lmp pull opensnoop
$ lmp run opensnoop
...
```

Or he or she can search the web and download it (see the next chapter).

You can switch sources, such as from github to an lmp static server:

```bash
# Download from lmp static server, prefix path changed to https://lmp.ebpf.io
LMP_REPOSITORY=https://lmp.ebpf.io lmp run opensnoop
```

ecli has implemented the related functions, including run and pull, only need to have an initiator to call ecli on the command line when lmp run.

``lmp run xxx`` is equivalent to
```bash
sudo EUNOMIA_REPOSITORY=https://linuxkerneltravel.github.io/lmp/ EUNOMIA_HOME=/home/ubuntu/.lmp/ ./ecli run xxx
```
``lmp pull xxx`` is equivalent to
```bash
sudo EUNOMIA_REPOSITORY=https://linuxkerneltravel.github.io/lmp/ EUNOMIA_HOME=/home/ubuntu/.lmp/ ./ecli pull xxx
```

The same goes for the rest. The LMP command line only needs to execute the corresponding command, similar to C language using system("ecli run xxx") such.

### Developer

Our second role is a developer who wants to create a universal eBPF/WASM precompiled binary, distribute it on any machine and operating system, and load it dynamically to run. This is useful for command-line tools or anything you can run directly in the Shell, or as a plug-in for large projects.

Generate ebpf data files.

```bash
# Generate a project template
$ lmp init opensnoop
init project opensnoop success.

# A new directory is created
$ cd opensnoop
$ ls # The following files are generated
opensnoop.bpf.c opensnoop.bpf.h README.md config.json .gitignore

# Build a kernel-state program
$ lmp build
$ ls # The following files are generated. package.json is the result of compilation
opensnoop.bpf.c opensnoop.bpf.h README.md config.json package.json .gitignore

# Generate a wasm user mode project template
$ lmp gen-wasm-skel
make
  GENERATE_PACKAGE_JSON
  GEN-WASM-SKEL
$ ls # The following files are generated
app.c eunomia-include ewasm-skel.h package.json README.md  opensnoop.bpf.c  opensnoop.bpf.h

$ lmp build-wasm
make
  GENERATE_PACKAGE_JSON
  BUILD-WASM
build app.wasm success

$ lmp run app.wasm -h
Usage: opensnoop [-h] [-x] [-k] [-n] [-p PID]
```

gen-wasm-skel provide the C language version of the WASM development framework, it contains the following files:

- ewasm-skel.h：The header file of the user-mode WebAssembly development framework contains the precompiled bytecode of eBPF program and the auxiliary information of eBPF program framework for dynamic loading.
- eunomia-include：Some header-only library functions and auxiliary files to aid development.
- app.c：The main code of user mode WebAssembly program contains the main logic of eBPF program and the data processing flow of eBPF program.

The code that users need to write in app.c should be pure, normal C code, without any knowledge of the underlying WASM implementation. You can develop with the framework without knowing anything about WASM.

Among them:
``lmp build`` is equivalent to
```bash
docker run -it -v `pwd`/:/src/ yunwei37/ebpm:latest 
```
``lmp init hello-world`` is equivalent to
```bash
git clone https://github.com/eunomia-bpf/ebpm-template && mv ebpm-template hello-world
```
``lmp gen-wasm-skel`` is equivalent to
```bash
docker run -it -v `pwd`/:/src/ yunwei37/ebpm:latest gen-wasm-skel
```
And so on. The above ecli and docker commands have been implemented, so a command line wrapper and initiator are needed to hide the complexity of the underlying commands.

## Recommend

The relevant framework eunomia-bpf: An eBPF program Dynamic Loading Framework: https://github.com/eunomia-bpf/eunomia-bpf

The details for Linux Microscope LMP project: https://github.com/linuxkerneltravel/lmp

The details for clipp project: https://github.com/muellan/clipp#an-example-from-docopt
