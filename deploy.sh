nginx_src='/opt/nginx-src'
nginx='/opt/nginx'


project_dir=`pwd`

cd $nginx_src
./configure --add-module=$project_dir  --with-ld-opt="-L /usr/local/lib" --prefix=$nginx --with-debug
make
make install
sudo $nginx/sbin/nginx -s stop
sudo $nginx/sbin/nginx
cd $project_dir
