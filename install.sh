if [ -n "$(uname -i | grep x86)" ]; then
	wget https://aka.pw/bpf-ecli -O ecli && chmod +x ecli && sudo mv ecil /usr/bin
	wget https://github.com/GorilaMond/lmp_cli/releases/download/lmp/lmp_x86 -O lmp && chmod +x lmp && sudo mv lmp /usr/bin
else
	echo Only support x86 plateform now.
fi
if [ -z "$(docker -v | grep version)" ]; then
	curl -fsSL https://get.docker.com | bash -s docker --mirror Aliyun
fi

