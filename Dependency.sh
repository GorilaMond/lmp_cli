if [ -n "$(uname -i | grep x86)" ]; then
	if [ ! -e /usr/bin/ecli ]; then
		wget https://aka.pw/bpf-ecli -O ecli && chmod +x ecli && sudo mv ecil /usr/bin
	else
		echo ecli exist.
	fi
	if [ ! -e /usr/bin/ecli ]; then
		echo install error
		exit
	fi
else
	echo Only support x86 plateform now.
	exit
fi
if [ -z "$(docker -v | grep version)" ]; then
	curl -fsSL https://get.docker.com | bash -s docker --mirror Aliyun
else
	echo docker exist.
fi
if [ -z "$(docker -v | grep version)" ]; then
	echo install error
	exit
fi
echo Dependencies have completed.
