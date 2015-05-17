/*****************************************************************
 *
 * S H A R E D    O B J E C T    H A N D L I N G    L I B R A R Y
 *
 *****************************************************************/

kniznica 'shared' sluzi na spravu SHARED_MAX zdielanych pamatovych segmentov
nie je vlaknovo bezpecna, t.j. jej pouzitie je vyhradene iba pre 1 vlaknove programy
je bezpecna vzhladom na vytvaranie novych procesov (fork); pri forku sa v detskom procese korektne odpoja (nie zrusia!) vsetky zdielane oblasti
nakolko toto korektne zrusenie vyzaduje funkciu z kniznice pre vlakna, vysledny program treba linkovat s kniznicou 'pthread'

kompilacia: pridat na kompilacny riadok 'shared.c -lpthread'
napriklad: 'gcc -Wall -o main main.c shared.c -lpthread'

P O U Z I V A N I E
===================

kniznicu je potrebne v kazdom procese najprv inicializovat
nakolko kniznica vyuziva pre riadenie pristupu ku zdielanym segmentom mnozinu binarnych semaforov, tuto mnozinu je potrebne tiez inicializovat
avsak tato inicializacia semaforov sa moze urobit iba v jednom jedinom procese!
ak by sa totiz inicializacia robila vsade, moze prist k chybnej funkcnosti (ak sa uz v nejakom procese semafory pouzivaju)
pretoze operacia semop (v stave zablokovania, t.j. cakanie) nad semaforom skonci chybou vzdy, ked pride ku inicializacii vnutornej hodnoty semafora

nastavenie verbosity je mozne urobit kedykolvek pocas pouzivania i pred inicializaciou kniznice; a to i viacnasobne
kazde volanie meni platnost verbosity iba od okamihu volania

po inicializacii je mozne kniznicu pouzivat
najprv je treba vytvorit zdielany objekt volanim shared_create
volanie tejto funkcie vzdy s inym klucom ma za nasledok vytvorenie vzdy noveho zdielaneho objektu (ak nepride k nejakej chybe)
po uspesnom zavolani shared_create sa vrati id zdielaneho segmentu, a tento je zaroven pripojeny do virtualneho adresneho priestoru procesu
ak uz nejaky proces predtym zdielany objekt s takymto klucom vytvoril, 
aktualny proces ziska identifikator existujuceho zdielaneho segmentu (ak nepride ku chybe), a tento sa pripoji do virtualneho adresneho priestoru procesu

po pipojeni zdielaneho objektu je mozne nad nim robit operacie: zapis, citanie
a to jednak priamo, alebo aj riadene na zaklade binarneho semafora
prislusne funkcie vid dokumentacia
vyhoda riadeneho pristupu je, ze ked niekto zapisuje, iny musi cakat
horsie je, ze je to tak isto aj pri citani, takze nemozu viaceri naraz citat
schema:
   shared_write_locked() alebo shared_read_locked()

okrem funkcii na zapis a citanie kniznica poskytuje moznost uzamknutia semaforu alebo jeho uvolnenia pomocou vlastnych funkcii
to sa hodi v situacii, ked chceme postupne do pamate zapisovat napriklad znaky, a az po zapisani posledneho umoznit citanie
na to sa nehodi funkcia riadeneho zapisu, nakolko ta uzamkne zdielany objekt iba pre jedno volanie zapisu, nie viac
v takom pripade je nutne najprv oblast zamknut, potom zapisovat (viac krat) a napokon oblast odomknut
ak nam staci jedno volanie zapisu alebo citania, pouzijeme funkcie shared_write_locked() alebo shared_read_locked()
schema:
    shared_lock()
    while ( )
       shared_write() alebo shared_read()
    shared_unlock()

po ukonceni prace so zdielanym segmentom ho mozeme bud odpojit (shared_detach) a potom niekedy v buducnosti znovu pripojit (shared_attach)
posledny z procesov by mal zdielane objekty zrusit (shared_destroy)
zrusenie moze uspesne vykonat maximalne 1 proces; ostatne sa budu pokusat rusit zrusene objekty, co sposobi chybu

uplne na zaver, ak uz nik nebude pouzivat zdielane objekty, mozeme celu kniznicu upratat volanim shared_fini(),
ktore odpoji vsetky pripojene segmenty, zrusi ich a taktiez zrusi mnozinu semaforov

U K A Z K A   P O U Z I T I A
=============================

priklad pouzitia vid v programoch server.c a client.c

server inicializuje kniznicu, a inicializuje aj semafory
vytvori jeden zdielany segment
do zdielaneho segmentu chce postupne zapisovat znaky od 'a' po 'z' vratane, 
a klient si ich ma precitat az po zapisani posledneho
preto pouzijeme schemu: lock; write; write; ...; unlock
pred zapismi uzamkneme oblast, postupne do nej vpisujeme a na zaver ju odomkneme
ak sa niekto pokusi k oblasti pristupit (samozrejme ten druhy musi pouzit funkcie read_locked a write_locked, aby to fungovalo), ostane cakat,
az pokym neuvolnime binarny semafor danej oblasi
server po zapise bude cakat na to, pokym klient precita vsetky udaje
ak ich vsetky precita (co klient indikuje zapisanim znaku '*' hned na zaciatok oblasti), server zrusi oblast
a taktiez deaktivuje celu kniznicu

schema:
  shared_init(masterkey)
  id = shared_create(key)
  shared_lock(id)
  while ( )
      shared_write(id)
  shared_unlock(id)
  while ( 1 )
      if ( shared_read_lock(id) == '*' )
          break
  shared_destroy(id)
  shared_fini()

klient inicilizuje na svojej strane kniznicu, ale bez nastavovania hodnot semaforov!!!
pre experimenty je zaujimave vyskusat, ci bude navrh fungovat, ak aj klient bude inicializovat hodnoty semaforov... garantujem, ze nie
nejako som pri programovani na to pozabudol a potom som 2 hodiny hladal chybu, preco mi to nefunguje... teda obcas nefunguje ;)
po inicializacii sa pokusi pripojit zdielany segment
ak sa to podari, skusi ziskat vylucny pristup na citanie oblasti
kedze klient chce citat tiez viac krat, pouzijeme schemu: lock; read; read; read; ...; unlock
po precitani a spracovani vsetkych udajov klient zapise na zaciatok oblasti znak '*'
a odpoji zo svojho virtualneho adresneho priestoru zdielany segment
odpojenie sa kona este pri zamknutom semafore, pretoze...
ak by sme uvolnili semafor, moze prist k tomu, ze server bude pokracovat skor ako klient, a zrusi segment aj celu kniznicu
ak by v takomto pripade klient pokracoval, volanie odpojenia by vyvolalo chybu, nakolko by dany segment uz nejestvoval
zrusenie segmentov aj kniznice nechavame na server, takze po uvolneni semafora klient svoju cinnost skonci

schema:
  shared_init(masterkey, SHARED_NO_INITIALIZE)
  id = shared_create(key)
  shared_lock(id)
  while ( )
     shared_read(id)
  shared_write(id)
  shared_detach(id)
  shared_unlock(id)

podstatnou informaciou, ktoru musia oba programy zdielat, su kluce:
jeden pre pristup ku mnozine semaforov (masterkey), druhy pre pristup ku zdielanemu segmentu (key)
je zrejme, ze toto je iba jednoduchy priklad, a nedemonstruje moznosti kniznice - vytvorit az 128 roznych zdielanych segmentov
taktiez neukazuje zaludnosti a nedoriesene casti navrhu kniznice, ale to nechame na pripadneho zaujemcu  o pouzivanie ;)
ja osobne tuto kniznicu nepouzivam, urobil som ju iba pre objasnenie komunikacie medzi serverom a klientom pomocou zdielanej pamate :D :D :D

P O P I S   K N I Z N I C E
===========================

M A K R A
=========

SHARED_NO_INITIALIZE |
SHARED_INITIALIZE    |
----------------------
na bezpecny pristup ku zdielanym segmentom sa pouzivaju binarne semafory; pre kazdy segment jeden
tieto semafory je nutne inicializovat
celu kniznicu je treba pred prvym pouzitim v programe vzdy inicializovat pomocou funkcie 'shared_init'
tato inicializuje interne struktury kniznice, medzi inym aj pole semaforov pre zdielane segmenty
avsak hodnoty tychto semaforov sa mozu v celom projekte (napriklad klient-server aplikacia) inicializovat iba raz!!!
na urcenie, ci sa maju hodnoty semaforov inicializovat alebo nie, sluzi druhy parameter funkcie 'shared_init'
ak je uvedene makro 'SHARED_INITIALIZE', semafory sa inicializuju kazdy na hodnotu 1,
ak je uvedene makro 'SHARED_NO_INITIALIZE', semafory sa nebudu inicializovat

SHARED_NO_VERB |
SHARED_VERB    |
----------------
v pripade chyby je mozne zapnut podporu ladiacich vypisov o chybe do chybovej konzoly (pouziva sa perror)
za tymto ucelom je potrebne zapnut vypisovanie pomocou funkcie 'shared_set_verbose' s argumentom SHARED_VERB
implicitne je nastavena hodnota SHARED_NO_VERB, takze vypisy v pripade chyby nie su

C H Y B O V E   K O D Y
=======================

* SHARED_OK - ziadna chyba, vsetko ok
* SHARED_E_INIT - kniznica nie je inicializovana
* SHARED_E_INVAL - argument urcujuci cislo zdielaneho segmentu je mimo platneho rozsahu
* SHARED_E_ID - zdielany objekt s danym id nebol inicializovany
* SHARED_E_OBJCNT - dosiahnuty maximalny mozny pocet zdielanych objektov, nemozno vytvorit dalsi
* SHARED_E_SEM - chyba pri manipulacii so semaforom zdielaneho objektu
* SHARED_E_SHM - chyba pri manipulacii so zdielanym segmentom
* SHARED_E_PTR - zdielany segment (nie) je pripojeny - zavisi od typu operacie, vid popis funkcii
* SHARED_E_TOOBIG - citany/zapisovany buffer je vacsi nez velkost zdielaneho segmentu
* SHARED_E_ARG - argument funkcie je mimo platneho rozsahu

F U N K C I E
=============

void shared_set_verbose( int val ); 
-----------------------------------
popis: zapne/vypne vypisovanie chybovych sprav
'val': 
  SHARED_VERB - zapne vypisovanie chybovych sprav
  SHARED_NO_VERB - vypne vypisovanie chybovych sprav
chyby: v pripade nespravnej hodnoty argumentu 'val' funkcia nic nenastavuje

int shared_init( key_t key, int initialize );
---------------------------------------------
popis: inicializuje interne udajove struktury kniznice
  ak je hodnota argumentu 'initialize' SHARED_INITIALIZE', inicializuju sa hodnoty vsetkych semaforov kniznice pouzivane pre pristup ku zdielanym segmentom
  ak je hodnota argumentu 'initialize' SHARED_NO_INITIALIZE', inicializacia hodnot semaforov sa vynecha
'key': kluc, na zaklade ktoreho sa pristupuje ku kniznici (sluzi na pristup k mnozine semaforov pouzivanych kniznicou)
'initialize': na zaklade tejto hodnoty sa bud inicializuju alebo nie hodnoty semaforov v mnozine
vystup:
  SHARED_OK - v pripade, ze je vsetko ok
  SHARED_E_ARG - ak je chybna hodnota argumentu 'initialize'
  SHARED_E_SEM - ak sa vyskytne problem pri manipulacii s mnozinou semaforov

int shared_fini();
------------------
popis: uvolnuje interne udajove struktury kniznice
vystup:
  SHARED_OK - v pripade, ze je vsetko ok
  SHARED_E_SEM - ak sa vyskytne problem pri manipulacii s mnozinou semaforov

int shared_create( key_t key, int size );
-----------------------------------------
popis: vytvori novy zdielany segment o velkosti 'size'
  segment je identifikovany na zaklade kluca 'key'
  pri uspesnom vytvoreni je tento segment zaroven pripojeny do virtualneho adresneho priestoru procesu
  ak segment existuje, iba sa pripaja do virtualneho adresneho priestoru procesu
'key': kluc, na zaklade ktoreho sa spristupnuje zdielany segment
'size': velkost pripajaneho zdielaneho segmentu
vystup:
  v pripade, ze je vsetko ok, vrati sa id pripojeneho zdielaneho segmentu (t.j. nezaporne cislo pocnuc nulou)
  SHARED_E_INIT - ak nie je kniznica inicializovana
  SHARED_E_OBJCNT - ak uz nie je mozne vytvorit dalsi zdielany segment
  SHARED_E_SHM - v pripade chyby prace so zdielanym segmentom

int shared_attach( int id );
----------------------------
popis: pripoji existujuci zdielany segment kniznice do virtualneho adresneho priestoru procesu
'id': identifikator zdielaneho segmentu, ktory po vytvoreni vrati kniznicna funkcia 'shared_create'
vystup:
  SHARED_OK - ak je vsetko v poriadku
  SHARED_E_INIT - ak nie je kniznica inicializovana
  SHARED_E_INVAL - ak je id segmentu mimo platny rozsah
  SHARED_E_ID - ak segment s danym id nie je inicializovany (t.j. vytvoreny kniznicou)
  SHARED_E_SHM - v pripade chyby prace so zdielanym segmentom

int shared_detach( int id );
----------------------------
popis: odpojenie zdielaneho segmentu z virtualneho adresneho priestoru procesu
'id': identifikator zdielaneho segmentu, ktory po vytvoreni vrati kniznicna funkcia 'shared_create'
vystup:
  SHARED_OK - ak je vsetko v poriadku
  SHARED_E_INIT - ak nie je kniznica inicializovana
  SHARED_E_INVAL - ak je id segmentu mimo platny rozsah
  SHARED_E_ID - ak segment s danym id nie je inicializovany (t.j. vytvoreny kniznicou)
  SHARED_E_SHM - v pripade chyby prace so zdielanym segmentom

int shared_destroy( ind id );
-----------------------------
popis: zrusi zdielany segment, a zaroven ho odpoji z virtualneho adresneho priestoru procesu
'id': identifikator zdielaneho segmentu, ktory po vytvoreni vrati kniznicna funkcia 'shared_create'
vystup:
  SHARED_OK - ak je vsetko v poriadku
  SHARED_E_INIT - ak nie je kniznica inicializovana
  SHARED_E_INVAL - ak je id segmentu mimo platny rozsah
  SHARED_E_ID - ak segment s danym id nie je inicializovany (t.j. vytvoreny kniznicou)
  SHARED_E_SHM - v pripade chyby prace so zdielanym segmentom

int shared_lock( int id );
--------------------------
popis: uzamkne binarny semafor pre zdielny segment s danym id
'id': identifikator zdielaneho segmentu, ktory po vytvoreni vrati kniznicna funkcia 'shared_create'
  zamknut semafor je mozne, aj ked nie je pripojeny do virtualneho adresneho priestoru procesu
  dovod je ten, ze manipulacia so segmentom by mala prebiehat pri zamknutom semafore,
  aby kazdy proces mohol uspesne dokoncit operaciu, ktoru zacal, t.j. aj pripojenie alebo odpojenie
vystup:
  SHARED_OK - ak je vsetko v poriadku
  SHARED_E_INIT - ak nie je kniznica inicializovana
  SHARED_E_INVAL - ak je id segmentu mimo platny rozsah
  SHARED_E_ID - ak segment s danym id nie je inicializovany (t.j. vytvoreny kniznicou)
  SHARED_E_SEM - v pripade chyby  prace so semaforom zdruzenym so segmentom

int shared_unlock( int id );
----------------------------
popis: odomkne binarny semafor pre zdielny segment s danym id
'id': identifikator zdielaneho segmentu, ktory po vytvoreni vrati kniznicna funkcia 'shared_create'
  odomknut semafor je mozne, aj ked nie je pripojeny do virtualneho adresneho priestoru procesu
  dovod je ten, ze manipulacia so segmentom by mala prebiehat pri zamknutom semafore,
  aby kazdy proces mohol uspesne dokoncit operaciu, ktoru zacal, t.j. aj pripojenie alebo odpojenie
vystup:
  SHARED_OK - ak je vsetko v poriadku
  SHARED_E_INIT - ak nie je kniznica inicializovana
  SHARED_E_INVAL - ak je id segmentu mimo platny rozsah
  SHARED_E_ID - ak segment s danym id nie je inicializovany (t.j. vytvoreny kniznicou)
  SHARED_E_SEM - v pripade chyby  prace so semaforom zdruzenym so segmentom

int shared_write( int id, int from, const void *buf, int size );
----------------------------------------------------------------
popis: zapis do zdielaneho segmentu s danym id; zapis nie je riadeny semaforom!!!
  'id': identifikator zdielaneho segmentu, ktory po vytvoreni vrati kniznicna funkcia 'shared_create'
  'from': offset, od ktoreho zacina zapis do segmentu (0 znamena zaciatok segmentu)
  'buf': zdrojova adresa udajov, ktore sa zapisu do zdielaneho segmentu
  'size': velkost zapisovanych udajov, udava sa v bajtoch
vystup:
  SHARED_OK - ak je vsetko v poriadku
  SHARED_E_INIT - ak nie je kniznica inicializovana
  SHARED_E_INVAL - ak je id segmentu mimo platny rozsah
  SHARED_E_ID - ak segment s danym id nie je inicializovany (t.j. vytvoreny kniznicou)
  SHARED_E_PTR - ak segment nie je pripojeny do virtualneho adresneho priestoru procesu
  SHARED_E_ARG - ak je 'from' mimo povoleny interval <0; velkost_objektu)
  SHARED_E_TOOBIG - ak su zapisovane udaje dlhsie ako priestor v zdielanom segmente

int shared_read( int id, int from, void *buf, int size );
---------------------------------------------------------
popis: citanie zo zdielaneho segmentu s danym id; citanie nie je riadene semaforom!!!
  'id': identifikator zdielaneho segmentu, ktory po vytvoreni vrati kniznicna funkcia 'shared_create'
  'from': offset, od ktoreho zacina citanie zo segmentu (0 znamena zaciatok segmentu)
  'buf': cielova adresa pamatoveho miesta, kam sa zapisu udaje zo zdielaneho segmentu
  'size': velkost citanych udajov, udava sa v bajtoch
vystup:
  SHARED_OK - ak je vsetko v poriadku
  SHARED_E_INIT - ak nie je kniznica inicializovana
  SHARED_E_INVAL - ak je id segmentu mimo platny rozsah
  SHARED_E_ID - ak segment s danym id nie je inicializovany (t.j. vytvoreny kniznicou)
  SHARED_E_PTR - ak segment nie je pripojeny do virtualneho adresneho priestoru procesu
  SHARED_E_ARG - ak je 'from' mimo povoleny interval <0; velkost_objektu)
  SHARED_E_TOOBIG - ak je pozadovana dlzka citanych udajov dlhsia ako priestor v zdielanom segmente

int shared_write_locked( int fd, int from, const void *buf, int size );
-----------------------------------------------------------------------
popis: zapis do zdielaneho segmentu s danym id; zapis je riadeny semaforom!!! t.j. zapis je mozny iba ak je semafor otvoreny
  argumenty aj navratove hodnoty ako pri funkcii 'shared_write', plus
vystup SHARED_E_SEM - ak nastane chyba pri manipulacii so semaforom

int shared_read_locked( int fd, int from, void *buf, int size );
----------------------------------------------------------------
popis: citanie zo zdielaneho segmentu s danym id; citanie je riadene semaforom!!! t.j. citanie je mozne iba ak je semafor otvoreny
  argumenty aj navratove hodnoty ako pri funkcii 'shared_read', plus
vystup SHARED_E_SEM - ak nastane chyba pri manipulacii so semaforom

