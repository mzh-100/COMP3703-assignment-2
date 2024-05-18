WeChat: cstutorcs
QQ: 749389476
Email: tutorcs@163.com
#!/bin/bash

sudo socat TCP-LISTEN:1338,nodelay,reuseaddr,fork,su=nobody EXEC:./bin/heap1,stderr;

