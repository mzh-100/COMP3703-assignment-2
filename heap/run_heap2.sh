WeChat: cstutorcs
QQ: 749389476
Email: tutorcs@163.com
#!/bin/bash

sudo socat TCP-LISTEN:1339,nodelay,reuseaddr,fork,su=nobody EXEC:./bin/heap2,stderr;

