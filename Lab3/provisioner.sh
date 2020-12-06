sudo apt-get update
sudo apt-get install -y git vim-nox python-setuptools python-all-dev flex bison traceroute
cd ~
git clone git://github.com/mininet/mininet
cd mininet
./util/install.sh -fnv
cd ~
git clone http://github.com/noxrepo/pox
cd ~
git clone git://github.com/dound/ltprotocol.git
cd ltprotocol 
sudo python setup.py install
cd ~
git clone https://bitbucket.org/huangty/cs144_bufferbloat.git
