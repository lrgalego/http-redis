nginx_src='/opt/nginx-src'
nginx='/opt/nginx'


project_dir=`pwd`

cd $nginx_src
./configure --without-http_rewrite_module --add-module=$project_dir --prefix=$nginx --with-debug
make
make install
sudo $nginx/sbin/nginx -s stop
sudo $nginx/sbin/nginx -c $project_dir/nginx.conf
cd $project_dir
