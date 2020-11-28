s="[Desktop Entry]\nName=HTYShutdown\nComment=Linux shutdown UI\nExec=`pwd`/HTYShutdown\nIcon=`pwd`/icon/shutdown.svg\nPath=`pwd`\nTerminal=false\nType=Application\nCategories=System;"
echo -e $s > HTYShutdown.desktop
cp `pwd`/HTYShutdown.desktop ~/.local/share/applications/HTYShutdown.desktop