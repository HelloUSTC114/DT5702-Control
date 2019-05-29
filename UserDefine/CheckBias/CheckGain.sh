cp $1 ../
cd ..
sudo /opt/root/bin/root -l `$1`
rm $1
cd UserDefine