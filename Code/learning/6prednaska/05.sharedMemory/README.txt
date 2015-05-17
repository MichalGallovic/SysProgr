
Program dokumentuje synchronizáciu troch procesov pomocou semafora pri pristupe k zdielanej pamati (ZP).
Dva procesy do pamati zapisuju a jeden z pamati cita. Procesy su vytvorene v main funkcii a ich
vykonny kod tvoria skompilovane programi write1.c, write2.c a reader.c.
write1 zapisuje do pamati a,b,...z
write2  A,B,...Z
reader vypise obsah ZP
Poznámky
Pristup k ZP ma vzdy len jeden proces.
Signaly su v programe odchytene funkciou signal s reinstalaciou handlera.  
Miesto funkcie signal mozno pouzit funkciu sigaction a v tom pripade
handler netreba reinstalovat. 
Spustenie programu: ./main
Ukoncenie programu CTRL-C 