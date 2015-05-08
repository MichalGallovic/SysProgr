Programy demonstruju synchronizaciu procesov rodic-detsky proces s pomocou asynchronneho odchytenie synchronizacnych
signalov SIGUSER1, SIGUSER2. Signaly nesmu byt blokovane. Synchronizacia procesov je vo vypisoch - za vypisom procesu, 
ktory signal vysle by mal nasledovat vypis procesu, ktory signal obdrzi. Procesy cakaju na funkcii sigsuspend.
Premenna x je pouzita na zistenie ci uz signal prisiel inak by sa proces, ktory este necaka na funkcii sigsuspend
o jeho prichode nedozvedel a signal sa strati. 
Program ma vsak jednu zasadnu chybu. Ak nastane prepnutie
medzi procesmi medzi testovanim x a funkciou sigsuspend synchronizacia zlyha.
Dovod: napr. ak proces zisti ze signal este neprisiel (x=0) a je prepnute na iny
proces, ktory nasledne signal vysle a potom nastane opat spatne prepnute na povodny proces, tento pokracuje
vykonom  funkcii sigsuspend, poslany signal sa medzi tym stratil, kedze proces, ktoremu bol urceny
nebol v tom case aktivny, vysledkom je deadlock (oba procesy ostanu cakat na funkcii sigsuspend).